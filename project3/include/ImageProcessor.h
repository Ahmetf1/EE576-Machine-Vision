/**
 * @file ImageProcessor.h
 * @brief This file defines the ImageProcessor class, which is designed to load, display, and manipulate images using OpenCV.
 *
 * The ImageProcessor class provides functionalities to load images from a specified directory,
 * display them on a grid, select an image to view in detail, and apply Homographic Transformations on images.
 */

#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <constants.h>

/**
 * @class ImageProcessor
 * @brief A class to load and display images using OpenCV.
 *
 * ImageProcessor supports loading all images from a specified directory and displaying them.
 * It allows users to apply Homographic Transformations on images.
 */
class ImageProcessor {
public:
    /**
     * @brief Constructor for the ImageProcessor class.
     */
    ImageProcessor();

    /**
     * @brief Loads images from a specified directory.
     * @param path The path to the directory from which images will be loaded.
     */
    void loadImages(const std::string& path);

    /**
     * @brief Starts the image Processor application.
     */
    void run();

    /**
     * @brief Processes all images in a directory.
     * @param DataPath The path to the directory containing images.
     * @param OutputPath The path to the directory where the processed images will be saved.
     */
    void processAllImages(const std::string& DataPath, const std::string& OutputPath);

private:
    static constexpr int width = constants::width; ///< Width of the display window.
    static constexpr int height = constants::height; ///< Height of the display window.
    int selectedImage = -1; ///< Index of the currently selected image.
    std::vector<std::string> files; ///< List of image file paths.

    cv::Mat img; ///< First image.
    cv::Mat displayImage; ///< Resized image for display.
    cv::Mat grid; ///< Grid to display the images.
    int N1, N2; ///< Dimensions of the image grid.
    std::string path; ///< Path to the directory containing images.

    /**
     * @brief Displays images in a grid format.
     * @param num The number of images to display in the grid.
     */
    void displayImages(int num);

    /**
     * @brief Extracts the Green Region from an image.
     * @param img The input image.
     * @return A rgb mask of the green region.
     */
    cv::Mat segmentGreenRegion(const cv::Mat& img);

    /**
     * @brief Finds the boundaries of the green region in a rgb mask.
     * @param mask The binary mask of the green region.
     * @return A pair containing the rotated image and the contour of the green region.
     */
    std::pair<cv::Mat, std::vector<cv::Point>> findBoundaries(const cv::Mat& mask);

    /**
     * @brief Rotates an image to make the inner region horizontal.
     * @param img The input image.
     * @param contour The contour of the green region.
     * @return The rotated image.
     */
    cv::Mat rotateToHorizontal(const cv::Mat& img, const std::vector<cv::Point>& contour);

    /**
     * @brief Creates a welcome screen with messages.
     * @param messages The messages to display on the welcome screen.
     * @param width The width of the welcome screen.
     * @param height The height of the welcome screen.
     * @return A cv::Mat object containing the welcome screen.
     */
    cv::Mat createWelcomeScreen(const std::vector<std::string>& messages, int width, int height);

    /**
     * @brief Extracts the number from a filename.
     * @param filename The name of the file.
     * @return The number extracted from the filename.
     */
    int extractNumber(const std::string& filename);

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
     * @return A vector of strings containing the file paths.
     */
    std::vector<std::string> getFiles(const std::string &path);
};

#endif // IMAGEPROCESSOR_H
