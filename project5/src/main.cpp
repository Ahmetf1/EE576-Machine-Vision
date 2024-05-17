// main.cpp
#include <iostream>
#include <ImageFlow.h>
#include <constants.h>

int main() {
    ImageFlow viewer;
    viewer.loadImages(constants::dataPath, constants::dataMaskpath); // Adjust the path as necessary
    std::cout << "Welcome to the Image Flow!, Follow the instructions in the Window !" << std::endl;
    viewer.run();
    return 0;
}