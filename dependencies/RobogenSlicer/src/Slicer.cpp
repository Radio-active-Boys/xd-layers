#include "Slicer.h"

#include <fstream>
#include <cassert>
#include "BufferWrite.h"
#include "Slicing.h"
#include <iostream>
#include <thread>
#include <vector> // Include for std::vector

using namespace std;

Slicer::Slicer(std::string& filename) : Slicer(filename.c_str()) {}

Slicer::Slicer(const char* filename) 
    : checkLayers(nullptr), finishedCount(0) // Initialize checkLayers and finishedCount
{


    // Determine if the STL file is binary
    int extensionLen = 3;
    bool isBinary;
    if (filename[strlen(filename) - 1] == 'b')
    {
        isBinary = true;
        extensionLen++;
    }
    else
        isBinary = false;

    // Change the file extension from ".stl" to ".gcode"
    stl_target = stl_filePath;
    stl_target = filename;
    gcode_target = gcode_filePath;
    gcode_target = filename;
    gcode_target.append("gcode");
    remove(gcode_target.c_str());  // Remove old gcode file

    std::cout << "\tFile: " << filename << "\nReading...";
    std::cout << "\nNumber of triangles: " << stl.read(stl_target, isBinary);
    std::cout << "\nRead finish\n";
}

Slicer::~Slicer() 
{
    delete[] checkLayers; // Free dynamically allocated memory
}

void Slicer::slicing()
{
    double zmax = stl.getZmax();
    double zmin = stl.getZmin();
    numLayer = static_cast<int>((zmax - zmin - zStart) / zgap + 1);
    // Check that numLayer is valid and non-negative
    assert(numLayer > 0 && "Number of layers must be positive.");
    layers.reserve(numLayer);
    std::cout<<"Layer Size "<<layers.size()<<std::endl;

    // Allocate memory for checkLayers
    checkLayers = new bool[numLayer];
    std::fill_n(checkLayers, numLayer, false); // Initialize all to false

    // Create layers
    for (double z = zmin + zStart; z < zmax; z += zgap)
    {
        layers.push_back(Layer(z));
    }
    std::cout<<"Layer Size "<<layers.size()<<std::endl;
    
    std::vector<std::thread> threads(NUM_THREADS); // Use std::vector for automatic cleanup

    // Start threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        threads[i] = std::thread(&Slicer::threadLayer, this);
    }

    // Check if threads are valid and joinable before calling join()
    for (int i = 0; i < NUM_THREADS; i++)
    {
        if(threads[i].joinable())
        {
            threads[i].join();
        }
    }



    cout << "\nWriting...";
#if 0  // Write to gcode file directly
    ofstream f(gcode_target, ios_base::out | ios_base::app);
    for (const auto& layer : layers)
        layer.commandsOut(f);
#endif
#if 1  // Write to writing buffer
    BufferWrite buffer(gcode_target);
    for (const auto& layer : layers) 
    {
        layer.commandsOut(buffer);
    }
    buffer.flush();
#endif
    cout << "\nDone!\n";
}

// Multithreaded management function
void Slicer::threadLayer()
{
    for (int index = 0; index < numLayer; index++)
    {
        checkMutex.lock();
        if (checkLayers[index])
        {
            checkMutex.unlock();
            continue;
        }

        // Check index validity
        if (index < 0 || index >= numLayer) 
        {
            checkMutex.unlock();
            continue;  // Invalid index, skip this iteration
        }

        checkLayers[index] = true;  // Set to true while locked
        checkMutex.unlock();

        // Synchronize access to layers[index]
        // Protect the layer access since it's shared across threads
        {
            std::lock_guard<std::mutex> layerLock(layerMutex);  // Lock layer access
            // std::cout<<"Thread is calling Slicing.h "<<std::endl;
            Slicing cl(stl, layers[index]);
        }

        // Update finishedCount safely
        countMutex.lock();
        finishedCount++;
        cout << "\r" << finishedCount << "/" << numLayer;
        countMutex.unlock();
    }
}
