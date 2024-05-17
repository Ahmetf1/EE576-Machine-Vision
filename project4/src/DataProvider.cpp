/**
 * @file DataProvider.cpp
 * @brief This file contains the implementation of the DataProvider class to provide image data.
*/

#include <DataProvider.h>
#include <filesystem>
#include <iostream>
#include <string>


DataProvider::DataProvider(const std::string& path, const std::vector<std::string>& labels, bool useDepth) : path(path), labels(labels), useDepth(useDepth) {
    getImageData();
};


void DataProvider::getImageData(){
    std::filesystem::path lastDirectory;

    
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        // Skip files that are not in the labels list
        if (std::find(labels.begin(), labels.end(), entry.path().filename().string()) == labels.end()) {
            continue;
        }
        std::vector<std::string> subDirectories;
        for (const auto& subDir : std::filesystem::directory_iterator(entry)) { 
            subDirectories.push_back(subDir.path().string());
        }

        std::vector<ImageData> trainImages;
        std::vector<ImageData> testImages;
        for (int i = 0; i < subDirectories.size(); i++) {
            std::vector<std::string> imagePaths;
            for (const auto& file : std::filesystem::directory_iterator(subDirectories[i])) {
                if (!isFileSkipped(file.path().filename().string())) {
                    // Store the image path
                    imagePaths.push_back(file.path().string());
                }
            }

            std::sort(imagePaths.begin(), imagePaths.end());
            if (i == 0) {
                for (const auto& imagePath : imagePaths) {
                    ImageData imageData;
                    imageData.path = imagePath;
                    imageData.label = entry.path().filename().string();
                    testImages.push_back(imageData);
                }
            } else {
                for (const auto& imagePath : imagePaths) {
                    ImageData imageData;
                    imageData.path = imagePath;
                    imageData.label = entry.path().filename().string();
                    trainImages.push_back(imageData);
                }
            }
        }
        trainset.push_back(trainImages);
        testset.push_back(testImages);
    }
}

std::vector<std::vector<ImageData>> DataProvider::getTrainset() {
    if (useDepth) {
        return getDepthTrainset();
    } else {
        return trainset;
    }
}

std::vector<std::vector<ImageData>> DataProvider::getTestset() {
    if (useDepth) {
        return getDepthTestset();
    } else {
        return testset;
    }
}

std::vector<std::vector<ImageData>> DataProvider::getDepthTestset(){
    std::vector<std::vector<ImageData>> depthTestset = testset;
    for (auto& set : depthTestset) {
        for (auto& image : set) {
            std::string depthPath = image.path.substr(0, image.path.find_last_of('_')) + "_depthcrop.png";
            image.path = depthPath;
        }
    }
    return depthTestset;
}

std::vector<std::vector<ImageData>> DataProvider::getDepthTrainset(){
    std::vector<std::vector<ImageData>> depthTrainset = trainset;
    for (auto& set : depthTrainset) {
        for (auto& image : set) {
            std::string depthPath = image.path.substr(0, image.path.find_last_of('_')) + "_depthcrop.png";
            image.path = depthPath;
        }
        
    }
    return depthTrainset;
}


bool DataProvider::isFileSkipped(const std::string& filename) const {
    const std::vector<std::string> skippedExtensions = {".txt", "depthcrop", "maskcrop"};
    for (const auto& ext : skippedExtensions) {
        if (filename.find(ext) != std::string::npos) {
            return true;
        }
    }
    return false;
}