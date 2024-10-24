#pragma once

/*
        @author: Xinqi Bao
        Main class to manage the whole program.
        Include reading file, slicing each layer and writing to file,
        using mult-threads.
*/

#include <cstring>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include "Configurations.h"
#include "Layer.h"
#include "STL.h"
#include <atomic>
class Slicer
{
private:
    Stl stl;
    int numLayer;
    std::atomic<int> finishedCount = 0;
  // count for each finished layer
    bool* checkLayers;
    std::string stl_target;     // original stl file name
    std::string gcode_target;   // the name for outputting gcode file
    std::vector<Layer> layers;  // contain each layer with loops and gcode
    std::thread threads[NUM_THREADS];
    std::mutex checkMutex;
    std::mutex countMutex;
    std::mutex layerMutex;  // NEW: Protects access to layers
    void threadLayer();

public:
    // Constructors
    Slicer(std::string& filename);
    Slicer(const char* filename);
    
    // Destructor
    ~Slicer(); // Declare the destructor here

    // Member functions
    void slicing();
};
