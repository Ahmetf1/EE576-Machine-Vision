# EE 576 Machine Vision Project 4
The project is on extracting features of images from different classes, creating histograms with Bag of Visual Words, traning a SVM model and inferencing with the trained model. 


## Prerequisites

Before you run the application, make sure you have the following installed:

- C++ compiler (GCC, Clang, MSVC)
- CMake (for building the project)
- OpenCV library (4.x or higher recommended)


## How to Run Viewer

Copy your data inside the Data folder, change constants.h file according to your folder name and chosen labels.
Washington rgbd dataset is used in the project:    https://rgbd-dataset.cs.washington.edu/dataset/

Example configuration for Data
- Data
    - rgbd-dataset
        - apple
            - apple_1
            - apple_2
            - ...
        - ball
            - ball_1
            - ball_2
            - ...
        - ...



## How to Run BOW

Copy your data inside the Data folder, change constants.h file according to your folder name and chosen labels.

```
cd <project_dir>
mkdir build && cd build
cmake ..
make
./bow
```