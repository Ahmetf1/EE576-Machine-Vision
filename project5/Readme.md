# EE 576 Machine Vision Project 5
The HomographicTransformer project is a C++ application designed to load and display images with OpenCV then apply Optical Flow and Tracking to find average flow vectors. 


## Prerequisites

Before you run the HomographicTransformer application, make sure you have the following installed:

- C++ compiler (GCC, Clang, MSVC)
- CMake (for building the project)
- OpenCV library (4.x or higher recommended)


## How to Run

Copy your data inside the Data folder, change constants.h file according to your folder name.

```
cd <project_dir>
mkdir build && cd build
cmake ..
make
./project5
```