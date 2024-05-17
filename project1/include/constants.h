#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
namespace constants {
    const std::string dataPath = "../Data/orchard_dataset_image - small";
    constexpr int width = 192;
    constexpr int height = 108;
    const std::vector<std::string> welcomeMessage = {"Welcome to the Image Viewer!","Write the image number and press Enter to display the image.","Press 'q' to quit.","By clicking on the images you can select ROIs","Prepared by: Ahmet Furkan Akinci"};
}

#endif // CONSTANTS_H