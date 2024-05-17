#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
namespace constants {
    const std::string dataPath = "../Data/tum_freiburg3_sitting_static";
    const std::string dataMaskpath = "../Data/tum_freiburg3_sitting_static/masks";
    const std::vector<std::string> welcomeMessage = {"Welcome to the Optical Flow Interface","Write the image number and press Enter to display the image.","Press 'q' to quit.", "Then press enter" ,"Prepared by: Ahmet Furkan Akinci"};
    const int lineThickness = 4;
    const int minAreaThreshold = 500;
}

#endif // CONSTANTS_H