cd ..
rm -r build
mkdir build
cd build
cmake ..
cmake --build .
ls
cd ../src/Debug
ls
.\run Leg.stl