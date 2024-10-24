#include "Slicing.h"
#include <cmath>
#include <iostream>

#include "Configurations.h"
using namespace std;

Slicing::Slicing(Stl* stl, Layer* layer) : Slicing(*stl, *layer) {}

Slicing::Slicing(Stl& stl, Layer& layer)
    : z(layer.getZ()), loops(layer.getLoops()), triangles(stl.getLevel(z))
{

    intersection();
    mkLoop();
    Gcode g(layer);

}


// return true if this pair of vectors cross this level
inline bool Slicing::isCross(const Vec3d& v1, const Vec3d& v2) const
{  // not include at the vertex
    return (v1.z + thr < z && z < v2.z - thr) ||
           (v2.z + thr < z && z < v1.z - thr);
}

// check if the cut through both triangles' sides
bool Slicing::isSectionExist(Cross& cr) const
{
    for (const auto& temp : intersections)
        if (temp.equal(cr)) return true;
    return false;
}

// push every sections into the vetor
// Total as 10 different cross cases, side-side for 3, vertex-side for 3,
// vertex-vertex for 3, through whole facet for 1.
// More rare cases been checked more, saving whole execution time.
void Slicing::intersection()
{

    intersections.reserve(triangles.size() >> 1);
    for (const auto& t : triangles)
    {
        if (isCross(t.v1, t.v2))
        {
            if (isCross(t.v1, t.v3))
            {  // Cross v1--v2 and v1--v3
                intersections.push_back(
                    Cross(Vec2d(t.v1, t.v2, z), Vec2d(t.v1, t.v3, z)));
                continue;
            }
            else if (fabs(z - t.v3.z) < thr)
            {  // Cross v1--v2 and v3
                intersections.push_back(
                    Cross(Vec2d(t.v1, t.v2, z), Vec2d(t.v3.x, t.v3.y)));
                continue;
            }
            else
            {  // definitely cross v1--v2 and v2--v3
                intersections.push_back(
                    Cross(Vec2d(t.v1, t.v2, z), Vec2d(t.v2, t.v3, z)));
                continue;
            }
        }
        else if (isCross(t.v1, t.v3))
        {
            if (fabs(z - t.v2.z) < thr)
            {  // Cross v1--v3 and v2
                intersections.push_back(
                    Cross(Vec2d(t.v1, t.v3, z), Vec2d(t.v2.x, t.v2.y)));
                continue;
            }
            else
            {  // Cross v1--v3 and v2--v3
                intersections.push_back(
                    Cross(Vec2d(t.v1, t.v3, z), Vec2d(t.v2, t.v3, z)));
                continue;
            }
        }
        else if (fabs(z - t.v1.z) < thr)
        {
            if (isCross(t.v2, t.v3))
            {  // Cross v1 and v2--v3
                intersections.push_back(
                    Cross(Vec2d(t.v1.x, t.v1.y), Vec2d(t.v2, t.v3, z)));
                continue;
            }
            else if (fabs(z - t.v2.z) < thr)
            {
                if (fabs(z - t.v3.z) < thr)  // Through the whole facet
                    continue;
                else
                {  // v1 and v2
                    Cross cr(Vec2d(t.v1.x, t.v1.y), Vec2d(t.v2.x, t.v2.y));
                    if (!isSectionExist(cr)) intersections.push_back(cr);
                    continue;
                }
            }
            else if (fabs(z - t.v3.z) < thr)
            {  // v1 and v3, still need "if()", because may only cross one
               // vertex(v1)
                Cross cr(Vec2d(t.v1.x, t.v1.y), Vec2d(t.v3.x, t.v3.y));
                if (!isSectionExist(cr)) intersections.push_back(cr);
                continue;
            }
        }
        else if (fabs(z - t.v2.z) < thr)
        {
            if (fabs(z - t.v3.z) < thr)
            {  // v2 and v3
                Cross cr(Vec2d(t.v2.x, t.v2.y), Vec2d(t.v3.x, t.v3.y));
                if (!isSectionExist(cr)) intersections.push_back(cr);
                continue;
            }
        }
    }

}

// Combine intersections to loops
// TODO: vector.erase() take to much time, need to be optimied
void Slicing::mkLoop()
{
    if (intersections.empty()) return;
#if 0  // implement with subLoops
    Vec2d p1(intersections.back().p1);
    Vec2d p2(intersections.back().p2);
    Loop lp(p1, intersections.size() >> 1);
    lp.add(p2);
    intersections.erase(intersections.end() - 1);
    while (!intersections.empty())
    {
        if (!lp.isCompleted())
        {
            /**/
            for (int i = intersections.size() - 1; i >= 0; i--)
            {
                if (p2.equal(intersections[i].p1))
                {
                    p1 = p2;
                    p2 = intersections[i].p2;
                    intersections.erase(intersections.begin() + i);
                    lp.add(p2);
                    break;
                }
                else if (p2.equal(intersections[i].p2))
                {
                    p1 = p2;
                    p2 = intersections[i].p1;
                    intersections.erase(intersections.begin() + i);
                    lp.add(p2);
                    break;
                }
            }
            /*
            int i = 0;
            for (const auto& temp : intersections) {
                    if (p2.equal(temp.p1)) {
                            p1 = p2;
                            p2 = temp.p2;
                            intersections.erase(intersections.begin() + i);
                            lp.add(p2);
                            break;
                    }
                    else if (p2.equal(temp.p2)) {
                            p1 = p2;
                            p2 = temp.p1;
                            intersections.erase(intersections.begin() + i);
                            lp.add(p2);
                            break;
                    }
                    i++;
            }*/
        }
else
{
    cout << "189 - Entering new loop" << endl;

    // Check if there are any loops to process
    if (!loops.empty())
    {
        bool isSubLoop = false;
        for (int i = loops.size() - 1; i >= 0; i--)
        {
            cout << "193 - Checking loop " << i << endl;
            isSubLoop = loops[i].checkSubLoop(lp);
            if (isSubLoop) 
            {
                cout << "195 - SubLoop detected, breaking" << endl;
                break;
            }
            if (lp.checkSubLoop(loops[i]))
            {
                cout << "199 - Erasing subloop " << i << endl;
                loops.erase(loops.begin() + i);
            }
        }
        if (!isSubLoop)
        {
            cout << "204 - Pushing lp into loops" << endl;
            loops.push_back(lp);
        }
    }
    else
    {
        cout << "210 - Loops empty, adding first lp" << endl;
        loops.push_back(lp);
    }

    cout << "214 - Updating p1 and p2" << endl;

    // Safe check before updating p1 and p2
    if (intersections.size() > 1)
    {
        p1 = intersections.back().p1;
        p2 = intersections.back().p2;
        cout << "p1: (" << p1.x << ", " << p1.y << "), p2: (" << p2.x << ", " << p2.y << ")" << endl;

        // Create a new Loop from the last intersection points
        lp = Loop(p1, intersections.size() >> 1);
        lp.add(p2);

        // Safely erase the last intersection
        if (!intersections.empty())
        {
            cout << "Erasing last intersection" << endl;
            intersections.pop_back();
        }

        cout << "Remaining intersections: " << intersections.size() << endl;
    }
    else if (intersections.size() == 1)
    {
        cout << "Only one intersection left, processing and erasing it" << endl;

        // Handle the last intersection safely
        p1 = intersections.back().p1;
        p2 = intersections.back().p2;
        cout << "p1: (" << p1.x << ", " << p1.y << "), p2: (" << p2.x << ", " << p2.y << ")" << endl;

        // Process the last intersection and then remove it
        lp = Loop(p1, 1);  // Last loop created from the last point
        lp.add(p2);

        intersections.pop_back();  // Erase the last intersection

        cout << "Remaining intersections: 0" << endl;
        cout << "No more intersections, exiting loop" << endl;
        
        // Exit the loop, or return from the function, since no more intersections
        return;  // Or break, depending on how this block is structured
    }
    else
    {
        cout << "No intersections left, exiting loop" << endl;
        return;  // Exit the loop when no intersections remain
    }
}



    }
    bool isSubLoop = false;
    for (int i = loops.size() - 1; i >= 0; i--)
    {
        isSubLoop = loops[i].checkSubLoop(lp);
        if (isSubLoop) break;
        if (lp.checkSubLoop(loops[i]))
        {
            // lp.subLoops.push_back(loops[i]);
            loops.erase(loops.begin() + i);
        }
    }
    if (!isSubLoop) loops.push_back(lp);
#endif
#if 0
    cout<<"mkLoop Enter if 2 "<<endl;
	//Vec2d p1(intersections.front().p1);
	//Vec2d p2(intersections.front().p2);
	Vec2d p1(intersections.back().p1);
	Vec2d p2(intersections.back().p2);
	loops.push_back(Loop(p1, intersections.size() >> 1));
	loops.back().add(p2);
	//intersections.erase(intersections.begin());
	intersections.pop_back();
	while (!intersections.empty()) {
		if (!loops.back().isCompleted()) {
			/**/
			for (int i = intersections.size()-1; i >= 0; i--) {
				if (p2.equal(intersections[i].p1)) {
					p1 = p2;
					p2 = intersections[i].p2;
					intersections.erase(intersections.begin() + i);
					loops.back().add(p2);
					break;
				}
				else if (p2.equal(intersections[i].p2)) {
					p1 = p2;
					p2 = intersections[i].p1;
					intersections.erase(intersections.begin() + i);
					loops.back().add(p2);
					break;
				}
			}
			/*
			int i = 0;
			for (const auto& temp : intersections) {
				if (p2.equal(temp.p1)) {
					p1 = p2;
					p2 = temp.p2;
					intersections.erase(intersections.begin() + i);
					loops.back().add(p2);
					break;
				}
				else if (p2.equal(temp.p2)) {
					p1 = p2;
					p2 = temp.p1;
					intersections.erase(intersections.begin() + i);
					loops.back().add(p2);
					break;
				}
				i++;
			}*/
		}
		else {	//creat a new Loop
			//p1 = intersections.front().p1;
			//p2 = intersections.front().p2;
			p1 = intersections.back().p1;
			p2 = intersections.back().p2;
			loops.push_back(Loop(p1, intersections.size() >> 1));
			loops.back().add(p2);
			//intersections.erase(intersections.begin());
			intersections.pop_back();
		}
	}
#endif
#if 1  // Why take more time??
	int size = intersections.size();
	bool* bools = new bool[size];
	for (int i = 0; i < size; i++)
		bools[i] = true;
	Vec2d p1(intersections.front().p1);
	Vec2d p2(intersections.front().p2);
	loops.push_back(Loop(p1, intersections.size() >> 1));
	loops.back().add(p2);
	bools[0] = false;
	for (int i = 1; i < size; i++) {
		if (!loops.back().isCompleted()) {
			for (int k = 0; k < size; k++) {
				if (!bools[k])
					continue;
				if (p2.equal(intersections[k].p1)) {
					p1 = p2;
					p2 = intersections[k].p2;
					loops.back().add(p2);
					bools[k] = false;
					break;
				}
				else if (p2.equal(intersections[k].p2)) {
					p1 = p2;
					p2 = intersections[k].p1;
					loops.back().add(p2);
					bools[k] = false;
					break;
				}
			}
		}
		else {
			for (int k = 0; k < size; k++) {
				if (!bools[k])
					continue;
				Vec2d p1(intersections[k].p1);
				Vec2d p2(intersections[k].p2);
				loops.push_back(Loop(p1, intersections.size() >> 1));
				loops.back().add(p2);
				bools[k] = false;
			}
		}
	}
	delete bools;
#endif
    for (auto& lp : loops) lp.optimize();
}

void Slicing::printVector() const
{
    cout << "\nZ as " << z << ":\tcontain " << intersections.size() << "\n";
    for (const auto& i : intersections)
    {
        cout << i << '\n';
    }
}

void Slicing::printLoop() const
{
    for (int i = 0; i < loops.size(); i++)
    {
        cout << "\nLoop " << i << ", Z as " << z << '\n';
        loops.at(i).print();
        cout << "\n\n";
    }
}

