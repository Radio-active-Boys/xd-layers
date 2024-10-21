#ifndef ROBOGENSLICER_H
#define ROBOGENSLICER_H

#include <string>

class RobogenSlicer {
public:
    // Constructor
    RobogenSlicer(const std::string& filename);

    // Method to perform slicing
    void Rslicing();

private:
    std::string filename_;
};

#endif // ROBOGENSLICER_H
