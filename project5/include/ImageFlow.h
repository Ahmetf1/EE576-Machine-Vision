/**
 * @file ImageFlow.h
 * @brief This file defines the ImageFlow class, which is designed to demonstrate optical flow and tracking.
 *
 * The ImageFlow class provides functionalities to load images from a specified directory,
 * display them on a grid, and apply optical flow and tracking on images.
 */

#ifndef IMAGEFLOW_H
#define IMAGEFLOW_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <constants.h>

/**
 * @class ImageFlow
 * @brief A class to load and display motion flow images using OpenCV.
 *
 * ImageFlow supports loading all images from a specified directory and displaying them.
 * It allows users to apply motion flow on images.
 */
class ImageFlow {
public:
    /**
     * @brief Constructor for the ImageFlow class.
     */
    ImageFlow();

    /**
     * @brief Loads images and masks from a specified directory.
     * @param path The path to the directory from which images will be loaded.
     */
    void loadImages(const std::string& path,const std::string& mask_path);

    /**
     * @brief Starts the image Transformer application.
     */
    void run();

private:
    int selectedImage = -1; ///< Index of the currently selected image.
    std::vector<std::string> files; ///< List of image file paths.
    std::vector<std::string> mask_files; ///< List of image masks file paths.

    cv::Mat img; ///< First image.
    cv::Mat img2; ///< Second image.
    cv::Mat mask_img; ///< Mask image.
    cv::Mat mask_img2; ///< Mask image2.
    cv::Mat displayImage; ///< Resized image for display.
    cv::Mat displayImage2; ///< Resized image2 for display.
    cv::Mat grid; ///< Grid to display the images.
    int N1, N2; ///< Dimensions of the image grid.
    std::string path; ///< Path to the directory containing images.

    /**
     * @brief Displays images in a grid format.
     * @param num The number of images to display in the grid.
     */
    void displayImages(int num);

    /**
     * @brief Creates a welcome screen with messages.
     * @param messages The messages to display on the welcome screen.
     * @param width The width of the welcome screen.
     * @param height The height of the welcome screen.
     * @return A cv::Mat object containing the welcome screen.
     */
    cv::Mat createWelcomeScreen(const std::vector<std::string>& messages, int width, int height);

    /**
     * @brief Fills a grid cell with an image.
     * @param grid The grid where the image will be placed.
     * @param img The image to place in the grid.
     * @param position The position in the grid where the image will be placed.
     */
    void fillGrid(cv::Mat &grid, const cv::Mat &img, int position);

    /**
     * @brief Retrieves the list of files from a specified directory.
     * @param path The path to the directory.
     * @return 2 vector of strings containing the image and mask paths respectively.
     */
    std::pair<std::vector<std::string>, std::vector<std::string>> getFiles(const std::string &path, const std::string& mask_path);

    /**
     * @brief Applies mask to the image.
     * @param mask The mask to apply.
     * @param img The image to apply the mask to.
     * @return The masked image.
     */
    cv::Mat applyMask(const cv::Mat& mask, cv::Mat& img);

    /**
     * @brief Extracts the number from a string.
     * @param s The string from which to extract the number.
     * @return The extracted number.
     */
    int extractNumber(const std::string& s);

    /**
     * @brief Applies optical flow to the images. Prints the norm of average flow vectors.
     * @return The image with optical flow applied.
     */
    cv::Mat applyOpticalFlow();

    /**
     * @brief Applies tracking to the images. Prints the norm of average flow vectors.
     * @return The image with tracking applied.
     */
    cv::Mat applyTracking();
};

#endif // IMAGEFLOW_H
