#ifndef ROBOGENSLICER_H
#define ROBOGENSLICER_H

#include <string>
#include "Slicer.h"  // Include Slicer header to use its functionalities

class RobogenSlicer {
public:
    // Constructor
    RobogenSlicer(const std::string& filename);

    // Method to perform slicing
    void performSlicing();  // New method to encapsulate slicing logic

private:
    std::string filename_;
    Slicer slicer_;  // Member variable to hold Slicer object
};


#endif // ROBOGENSLICER_H
