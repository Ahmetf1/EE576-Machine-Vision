#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
namespace constants {
    const std::string dataPath = "../Data/corridor_human2";
    constexpr int width = 192;
    constexpr int height = 108;
    const std::vector<std::string> welcomeMessage = {"Welcome to the Homogrophic Transformer!","Write the image number and press Enter to display the image.","Press 'q' to quit.","By clicking on the images you can select points to use in transformation", "Then press enter", "Custom method result is on top, built-in method result is at the bottom" ,"Prepared by: Ahmet Furkan Akinci"};
    const int circleRadius = 6;
    const int lineThickness = 4;
}

#endif // CONSTANTS_H