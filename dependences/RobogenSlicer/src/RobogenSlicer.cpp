#include "RobogenSlicer.h"
#include "Slicer.h"
#include <iostream>

RobogenSlicer::RobogenSlicer(const std::string& filename) 
    : filename_(filename), slicer_(filename.c_str())       
{
}

void RobogenSlicer::performSlicing() {
    std::cout << "Slicing file: " << filename_ << std::endl;
    slicer_.slicing();  // Call the slicing method from Slicer
}
