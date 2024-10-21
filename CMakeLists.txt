cmake_minimum_required(VERSION 3.10)

# Set the project name
project(MyRobogenApp)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories for RobogenSlicer headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependences/RobogenSlicer/include)

# Add the executable
add_executable(${PROJECT_NAME} src/main.cpp)

# Optionally, set the output name (useful if you want a specific name)
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "run"
)

# Link the RobogenSlicer library
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependences/RobogenSlicer/lib/Debug/RobogenSlicer.lib)
