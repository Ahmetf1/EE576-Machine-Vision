/**
 * @file BagOfWords.h
 * @brief This file contains the declaration of the BagOfWords class to find average BOW descriptors and similarity matrix.
*/

#ifndef BAGOFWORDS_H
#define BAGOFWORDS_H

#include <opencv2/opencv.hpp>
#include <string>

using namespace std;
struct ImageWithLabel;

class BagOfWords {
public:
    /**
     * @brief Runs the Bag of Words algorithm on the images in the specified directory.
     * @param path The path to the directory containing images.
     * @return The similarity matrix of the images.
     */
    cv::Mat run(const string& path);
    
    /**
     * @brief Visualizes the similarity matrix.
     * @param similarityMatrix The similarity matrix to visualize.
     */
    void visualizeSimilarityMatrix(const cv::Mat& similarityMatrix);


private:
    /**
     * @brief Loads images from the specified directory.
     * @param path The path to the directory containing images.
     */
    void loadImages(const string& path);
    
    /**
     * @brief Extracts descriptors from the images.
     * @param images The images to extract descriptors from.
     * @return The descriptors of the images.
     */
    std::vector<cv::Mat> getDescriptors(vector<ImageWithLabel> images);
    
    /**
     * @brief Builds a vocabulary from the descriptors.
     * @param descriptors The descriptors to build the vocabulary from.
     * @return The vocabulary.
     */
    cv::Mat buildVocabulary(const vector<cv::Mat>& descriptors);
    
    /**
     * @brief Builds histograms from the descriptors.
     * @param descriptors The descriptors to build histograms from.
     * @param vocabulary The vocabulary to use for building histograms.
     * @return The histograms.
     */
    vector<cv::Mat> buildHistograms(const vector<cv::Mat>& descriptors, const cv::Mat& vocabulary);
    
    /**
     * @brief Calculates the average BOW descriptors for each class.
     * @param histograms The histograms to calculate the average BOW descriptors from.
     */
    void calculateAverageDescriptors(const vector<cv::Mat>& histograms);
    
    /**
     * @brief Calculates the similarity matrix of the images.
     * @return The similarity matrix.
     */
    cv::Mat calculateSimilarityMatrix();
    
    /**
     * @brief Gets the index of a label.
     * @param label The label to get the index of.
     * @return The index of the label.
     */
    int getLabelIndex(const string& label);
    
    map<string, int> classLabelsMap; // Maps class labels to their names
    map<int, string> indexToLabelMap; // Maps class indices back to labels
    std::vector<ImageWithLabel> images; // Vector of images with their labels
    map<int, cv::Mat> averageDescriptors; // Maps class labels to their average BOW descriptors
    vector<int> classLabels; // Unique class labels

};

struct ImageWithLabel {
    cv::Mat image; // Image data
    std::string label; // Image label
};

#endif // BAGOFWORDS_H