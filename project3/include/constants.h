/*
* @file constants.h
* @brief This file contains the constants used in the Image Processor application.
*/

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <opencv2/opencv.hpp>

namespace constants {
    // Image Processor Related Constants
    const std::string dataPath = "../Data/components";
    const std::string outputPath = "../LearningData";
    constexpr int width = 192;
    constexpr int height = 108;
    const std::vector<std::string> welcomeMessage = {"Welcome to the Image Processor!","Write the image number and press Enter to display the image.", "You can also move between the images by pressing 'a' and 'd' ","Press 'q' to quit.", "Prepared by: Ahmet Furkan Akinci"};
    const std::string windowName = "Image Processor";
    const std::string terminalMessage = "Welcome to the Image Processor!, Follow the instructions in the Window !";
    constexpr int circleRadius = 6;
    constexpr int lineThickness = 4;
    const cv::Scalar SegLowLimit(40, 30, 65);
    const cv::Scalar SegHighLimit(70, 255, 255);
    const cv::Scalar OuterContourColor(255, 0, 0);
    const cv::Scalar InnerContourColor(0, 255, 0);

    // BOW Related Constants
    constexpr int vocabularySize = 100;
}

#endif // CONSTANTS_H