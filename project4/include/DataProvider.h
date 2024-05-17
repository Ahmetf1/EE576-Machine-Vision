/**
 * @file DataProvider.h
 * @brief This file contains the declaration of the DataProvider class to provide image data.
*/

#ifndef DATA_PROVIDER_H
#define DATA_PROVIDER_H

#include <string>

struct ImageData;

class DataProvider {
public:
    /**
     * @brief Constructor for the DataProvider class.
     * @param path The path to the directory containing images.
     */
    DataProvider(const std::string& path, const std::vector<std::string>& labels, bool useDepth = false);

    /**
     * @brief Gets the trainset.
     * @return The trainset.
     */
    std::vector<std::vector<ImageData>> getTrainset();
    
    /**
     * @brief Gets the testset.
     * @return The testset.
     */
    std::vector<std::vector<ImageData>> getTestset();

    bool useDepth; ///< Flag to indicate if depth images are used.
private:
    void getImageData(); ///< Gets the image data.
    bool isFileSkipped(const std::string& filename) const; ///< Checks if the file should be skipped.
    std::vector<std::vector<ImageData>> getDepthTestset(); ///< Gets the depth testset.
    std::vector<std::vector<ImageData>> getDepthTrainset(); ///< Gets the depth trainset.
    std::vector<std::vector<ImageData>> trainset; ///< The trainset.
    std::vector<std::vector<ImageData>> testset; ///< The testset.
    std::string path; ///< The path to the directory containing images.
    std::vector<std::string> labels; ///< The labels.
};

/**
 * @brief The ImageData struct that contains the path and label of an image.
*/
struct ImageData {
    std::string path;
    std::string label;
};

#endif // DATA_PROVIDER_H