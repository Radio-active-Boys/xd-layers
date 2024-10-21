#include "RobogenSlicer.h"
#include "Slicer.h"
#include <iostream>

RobogenSlicer::RobogenSlicer(const std::string& filename) : filename_(filename) {}

void RobogenSlicer::Rslicing() {
    // Your slicing logic here
    std::cout << "Slicing file: " << filename_ << std::endl;
    Slicer slicer(filename_);  // Adjust this as necessary if Slicer needs different parameters
    slicer.slicing();
}
