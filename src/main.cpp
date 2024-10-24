#include <fstream>
#include <iostream>
#include <string>
#include "RobogenSlicer.h"

int main(int argc, char* argv[]) {
    const char* filename = argc < 2 ? "Elbow.stl" : argv[1];
    RobogenSlicer slicer(filename);  // Create RobogenSlicer object
    
    slicer.performSlicing();          // Call the performSlicing method
    return 0;
}
