/**
 * @file ImageViewer.h
 * @brief This file defines the ImageViewer class, which is designed to load, display, and manipulate images using OpenCV.
 *
 * The ImageViewer class provides functionalities to load images from a specified directory,
 * display them on a grid, select an image to view in detail, and apply basic image processing
 * operations such as cropping and rotating.
 */

#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <constants.h>

/**
 * @class ImageViewer
 * @brief A class to load and display images using OpenCV.
 *
 * ImageViewer supports loading all images from a specified directory and displaying them.
 * It allows users to interact with images, including selecting an image to view it in detail
 * and applying operations like cropping.
 */
class ImageViewer {
public:
    /**
     * @brief Constructor for the ImageViewer class.
     */
    ImageViewer();

    /**
     * @brief Loads images from a specified directory.
     * @param path The path to the directory from which images will be loaded.
     */
    void loadImages(const std::string& path);

    /**
     * @brief Starts the image viewer application.
     */
    void run();

private:
    static constexpr int width = constants::width; ///< Width of the display window.
    static constexpr int height = constants::height; ///< Height of the display window.
    int selectedImage = -1; ///< Index of the currently selected image.
    std::vector<std::string> files; ///< List of image file paths.

    cv::Mat img; ///< Original image.
    cv::Mat rotatedImg; ///< Rotated version of the original image.
    cv::Mat displayImage; ///< Resized image for display.
    cv::Mat grid; ///< Grid to display the images.
    int N1, N2; ///< Dimensions of the image grid.
    cv::Point point1, point2; ///< Points for defining a Region of Interest (ROI).
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
     * @brief Static callback function for mouse events.
     * @param event The type of mouse event.
     * @param x The x-coordinate of the mouse event.
     * @param y The y-coordinate of the mouse event.
     * @param flags Additional flags for the event.
     * @param userdata User data passed to the callback function.
     */
    static void mouseCallback(int event, int x, int y, int flags, void* userdata);

    /**
     * @brief Processes mouse callback events.
     * @param event The type of mouse event.
     * @param x The x-coordinate of the mouse event.
     * @param y The y-coordinate of the mouse event.
     * @param flags Additional flags for the event.
     */
    void processMouseCallback(int event, int x, int y, int flags);

    /**
     * @brief Crops a rectangular region from an image.
     * @param img The source image.
     * @param center The center point of the rectangle.
     * @param width The width of the rectangle.
     * @param height The height of the rectangle.
     * @return A cv::Mat object containing the cropped image.
     */
    cv::Mat cropRectangle(const cv::Mat &img, const cv::Point &center, int width, int height);

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

#endif // IMAGEVIEWER_H
