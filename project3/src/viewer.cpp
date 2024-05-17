// main.cpp
#include <iostream>
#include <ImageProcessor.h>
#include <constants.h>

int main() {
    ImageProcessor viewer;
    viewer.loadImages(constants::dataPath); // Adjust the path as necessary
    std::cout << constants::terminalMessage << std::endl;
    viewer.run();
    return 0;
}