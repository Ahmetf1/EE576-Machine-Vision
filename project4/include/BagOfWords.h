/**
 * @file BagOfWords.h
 * @brief This file contains the declaration of the BagOfWords class to find average BOW descriptors and similarity matrix.
*/

#ifndef BAGOFWORDS_H
#define BAGOFWORDS_H

#include <opencv2/opencv.hpp>
#include <opencv2/ml.hpp>
#include <string>

#include <DataProvider.h>

using namespace std;
struct ImageWithLabel;

class BagOfWords {
public:
    /**
     * @brief Runs the Bag of Words algorithm on the images in the specified directory.
     * @param path The path to the directory containing images.
     * @return The similarity matrix of the images.
     */
    void run(DataProvider& dataProvider);
    
    /**
     * @brief Predicts the labels of the images in the specified directory.
     * @param path The path to the directory containing images.
     */
    void predict(vector<vector<ImageData>> images);
private:
    /**
     * @brief Reads and masks an image.
     * @param imageData The image data.
     * @return The masked image.
     */
    cv::Mat readAndMaskImage(const ImageData& imageData);
    
    /**
     * @brief Gets the histograms of the images.
     * @param images The images.
     * @param is_train Flag to indicate if the images are for training.
     * @return The histograms.
     */
    std::vector<cv::Mat> getHistograms(vector<vector<ImageData>> images, bool is_train);

    /**
     * @brief Trains the SVM models.
     * @param images The train set.
     * @param histograms The histograms of the training images.
     */
    void trainSVMs(const vector<vector<ImageData>>& images, const vector<cv::Mat>& histograms);
    
    /**
     * @brief Predicts the labels of the images.
     * @param images The test set
     */
    void SVMpredict(const vector<vector<ImageData>>& images);

    
    cv::Ptr<cv::BOWImgDescriptorExtractor> bowExtractor; // BOW descriptor extractor
    map<string, int> classLabelsMap; // Maps class labels to their names
    map<int, cv::Mat> averageDescriptors; // Maps class labels to their average BOW descriptors
    vector<cv::Ptr<cv::ml::SVM>> svms; // List of SVM models for each class
    vector<int> classLabels; // Unique class labels
    bool useDepth;
};

#endif // BAGOFWORDS_H