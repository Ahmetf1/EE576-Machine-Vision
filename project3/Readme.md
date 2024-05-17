# EE 576 Machine Vision Project 3
The project is on finding components on the green region and creating a bow descriptors from the extracted components.


## Prerequisites

Before you run the application, make sure you have the following installed:

- C++ compiler (GCC, Clang, MSVC)
- CMake (for building the project)
- OpenCV library (4.x or higher recommended)


## How to Run Viewer

Copy your data inside the Data folder, change constants.h file according to your folder name.
If your files are structured as 
- Data
    - Components
        - L2835
        - L5050
        - ...
You do not need to change the path variables.

```
cd <project_dir>
mkdir build && cd build
cmake ..
make
./viewer
```

## How to Run BOW

Copy your data inside the Data folder, change constants.h file according to your folder name.

```
cd <project_dir>
mkdir build && cd build
cmake ..
make
./bow
```