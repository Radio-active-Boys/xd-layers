cmake_minimum_required(VERSION 3.10)

# Set the project name
project(MyRobogenApp C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Include directories for headers and dependencies
include_directories( 
                  ${CMAKE_SOURCE_DIR}/include
                  ${CMAKE_SOURCE_DIR}/dependencies/RobogenSlicer/include


)
if(WIN32)
    # Include the directory that contains unistd.h
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/RobogenSlicer/include/unistd_h)
endif()

add_subdirectory(${CMAKE_SOURCE_DIR}/dependencies/RobogenSlicer)

# Source files (using GLOB_RECURSE to automatically include all .cpp files)
file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
# Define compile definitions based on build type
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_compile_definitions(DEBUG)
elseif(CMAKE_BUILD_TYPE MATCHES "Release")
    add_compile_definitions(RELEASE)
endif()


# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})


# Link RobogenSlicer library
target_link_libraries(MyRobogenApp PRIVATE RobogenSlicer) 


# Export compile commands for tooling
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set parallel build level
set(CMAKE_BUILD_PARALLEL_LEVEL 8)


