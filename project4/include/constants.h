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
    const std::string dataPath = "../Data/rgbd-dataset";
    const std::vector<std::string> labels = {"calculator", "banana", "cap", "keyboard", "toothbrush"};

    // BOW Related Constants
    constexpr int vocabularySize = 10;

    // SVM Related Constants
    constexpr double nu = 0.15;
    constexpr double threshold = 50;
    const cv::TermCriteria termCriteria(cv::TermCriteria::MAX_ITER, 100, 1e-6);
}

#endif // CONSTANTS_H