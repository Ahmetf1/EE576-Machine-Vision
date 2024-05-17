// main.cpp
#include <iostream>
#include <ImageTransformer.h>
#include <constants.h>

int main() {
    ImageTransformer viewer;
    viewer.loadImages(constants::dataPath); // Adjust the path as necessary
    std::cout << "Welcome to the Homogrophic Transformer!, Follow the instructions in the Window !" << std::endl;
    viewer.run();
    return 0;
}