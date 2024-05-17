/**
 * @file ImageTransformer.h
 * @brief This file defines the ImageTransformer class, which is designed to load, display, and manipulate images using OpenCV.
 *
 * The ImageTransformer class provides functionalities to load images from a specified directory,
 * display them on a grid, select an image to view in detail, and apply Homographic Transformations on images.
 */

#ifndef IMAGETRANSFORMER_H
#define IMAGETRANSFORMER_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <constants.h>

/**
 * @class ImageTransformer
 * @brief A class to load and display images using OpenCV.
 *
 * ImageTransformer supports loading all images from a specified directory and displaying them.
 * It allows users to apply Homographic Transformations on images.
 */
class ImageTransformer {
public:
    /**
     * @brief Constructor for the ImageTransformer class.
     */
    ImageTransformer();

    /**
     * @brief Loads images from a specified directory.
     * @param path The path to the directory from which images will be loaded.
     */
    void loadImages(const std::string& path);

    /**
     * @brief Starts the image Transformer application.
     */
    void run();

private:
    static constexpr int width = constants::width; ///< Width of the display window.
    static constexpr int height = constants::height; ///< Height of the display window.
    int selectedImage = -1; ///< Index of the currently selected image.
    std::vector<std::string> files; ///< List of image file paths.

    cv::Mat img; ///< First image.
    cv::Mat img2; ///< Second image.
    cv::Mat displayImage; ///< Resized image for display.
    cv::Mat displayImage2; ///< Resized image2 for display.
    cv::Mat matchedImage; ///< Resized matched image for display.
    cv::Mat matchedImageCustom; ///< Resized matched image for display.
    cv::Mat grid; ///< Grid to display the images.
    int N1, N2; ///< Dimensions of the image grid.
    cv::Point2f point1, point2; ///< Points for defining a Region of Interest (ROI).
    std::vector<cv::Point2f> points; ///< Points in the first image.
    std::vector<cv::Point2f> points2; ///< Points in the second image.
    cv::Scalar PointColor; ///< Color for drawing shapes.
    bool firstImageTurn = true; ///< Flag to indicate the turn for the first image.
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


    int extractNumber(const std::string& filename);

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

    /**
     * @brief Finds the homography map between two sets of points in two images.
     * @param pts1 The points in the first image.
     * @param pts2 The points in the second image.
     * @param img1 The first image.
     * @param img2 The second image.
     * @return A cv::Mat object containing the homography map.
     */
    cv::Mat findHomographyMap(const std::vector<cv::Point2f>& pts1, const std::vector<cv::Point2f>& pts2, const cv::Mat& img1, const cv::Mat& img2);
    
    /**
     * @brief Calculates the average error between two sets of points.
     * @param originalPoints The original points.
     * @param transformedPoints The transformed points.
     * @return The average error between the two sets of points.
     */
    double calculateAverageError(const std::vector<cv::Point2f>& originalPoints, const std::vector<cv::Point2f>& transformedPoints);

    /**
     * @brief Generates a random color.
     * @return A cv::Scalar object representing the random color.
     */
    cv::Scalar generateRandomColor();

    /**
     * @brief Computes the homography between two sets of points in two images.
     * @param pts1 The points in the first image.
     * @param pts2 The points in the second image.
     * @param img1 The first image.
     * @param img2 The second image.
     * @return A cv::Mat object containing the homography matrix.
     */
    cv::Mat computeHomography(const std::vector<cv::Point2f>& pts1, const std::vector<cv::Point2f>& pts2, const cv::Mat& img1, const cv::Mat& img2);
};

#endif // IMAGETRANSFORMER_H
