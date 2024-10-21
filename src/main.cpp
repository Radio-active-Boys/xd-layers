#include <fstream>
#include <iostream>
#include <string>

#include "RobogenSlicer.h"  // Ensure this path is correct
using namespace std;

int main(int argc, char* argv[])
{
    const char* filename = argc < 2 ? "cube.stl" : argv[1];
    RobogenSlicer Rslicer(filename);  // Create an instance of RobogenSlicer
    Rslicer.slicing();  // Call the slicing method

    return 0;
}
