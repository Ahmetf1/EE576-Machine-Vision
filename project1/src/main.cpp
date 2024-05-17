// main.cpp
#include <ImageViewer.h>
#include <constants.h>

int main() {
    ImageViewer viewer;
    viewer.loadImages(constants::dataPath); // Adjust the path as necessary
    viewer.run();
    return 0;
}