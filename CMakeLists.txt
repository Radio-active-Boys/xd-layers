cmake_minimum_required(VERSION 3.10)

# Set the project name
project(MyRobogenApp)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the source files
set(SOURCES
    src/main.cpp  # Replace with your actual source files
    # src/other_file.cpp  # Add additional source files here if necessary
)

# Include directories for headers
if(WIN32)
    # Include the directory that contains unistd.h
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependences/RobogenSlicer/include/unistd_h)
endif()

# Include directories for RobogenSlicer headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependences/RobogenSlicer/include)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link the RobogenSlicer library after defining the target
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dependences/RobogenSlicer/lib/Debug/RobogenSlicer.lib)

# Specify output directory for the executable
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/bin
)

# Optionally, set the output name (useful if you want a specific name)
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "run"
)

# Add a custom clean target
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Cleaning up build files..."
)
