/*
* @file BagOfWords.cpp
* @brief This file contains the implementation of the BagOfWords class to train SVM models with histograms generated using the Bag of Words algorithm.
*/

#include <BagOfWords.h>
#include <constants.h>

#include <vector>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/ml.hpp>


using namespace std;

void BagOfWords::run(DataProvider& dataProvider)
{
    useDepth = dataProvider.useDepth;
    std::vector<cv::Mat> histograms;
    histograms = getHistograms(dataProvider.getTrainset(), true);
    trainSVMs(dataProvider.getTrainset(), histograms);
}

void BagOfWords::predict(vector<vector<ImageData>> images)
{
    SVMpredict(images);
}

cv::Mat BagOfWords::readAndMaskImage(const ImageData &imageData)
{
    if (useDepth) {
        cv::Mat image = cv::imread(imageData.path, cv::IMREAD_UNCHANGED);
        cv::normalize(image, image, 0, 255, cv::NORM_MINMAX, CV_8U);
        
        cv::Mat mask = cv::imread(imageData.path.substr(0, imageData.path.find_last_of('_')) + "_maskcrop.png", cv::IMREAD_GRAYSCALE);
        cv::Mat maskedImage;
        cv::bitwise_and(image, image, maskedImage, mask);
        cv::Mat sharpened;
        cv::addWeighted(maskedImage, 1.5, maskedImage, -0.5, 0, sharpened);
        cv::Mat edges;
        cv::Laplacian(sharpened, edges, CV_16S, 3);
        cv::convertScaleAbs(edges, edges);
        return sharpened;
    }
    else {
        cv::Mat image = cv::imread(imageData.path);
        std::string maskPath = imageData.path.substr(0, imageData.path.find_last_of('_')) + "_maskcrop.png";
        cv::Mat mask = cv::imread(maskPath, cv::IMREAD_GRAYSCALE);
        cv::Mat maskedImage;
        cv::bitwise_and(image, image, maskedImage, mask);
        return maskedImage;
    }
}

std::vector<cv::Mat> BagOfWords::getHistograms(vector<vector<ImageData>> images, bool is_train)
{
    std::vector<cv::Mat> descriptorsVec;
    cv::BOWKMeansTrainer bowTrainer(constants::vocabularySize);
    cv::Ptr<cv::SIFT> detector;
    cv::Ptr<cv::AKAZE> detector2;
    if (useDepth) {
        detector2 = cv::AKAZE::create(cv::AKAZE::DESCRIPTOR_MLDB, 0, 3, 0.00000001f);  // Lower threshold
    } else {
        detector = cv::SIFT::create();
    }
    auto extractor = cv::SiftDescriptorExtractor::create();
    for (const auto& imageSet : images) {
        for (const auto& image : imageSet) {
            cv::Mat maskedImage = readAndMaskImage(image);
            vector<cv::KeyPoint>keypoints;
            if (useDepth) {
                detector2->detect(maskedImage, keypoints);
            } else {
                detector->detect(maskedImage, keypoints);
            }
            cv::Mat descriptors;
            extractor->compute(maskedImage, keypoints, descriptors);
            descriptorsVec.push_back(descriptors);
            if (is_train) {
                bowTrainer.add(descriptors);
            }
        }
    }
    if (is_train) {
        cv::Mat vocabulary = bowTrainer.cluster();
        cv::Ptr<cv::DescriptorExtractor > descExtractor = cv::SiftDescriptorExtractor::create();
        cv::Ptr<cv::DescriptorMatcher > descMatcher = cv::BFMatcher::create();
        bowExtractor = new cv::BOWImgDescriptorExtractor(descExtractor, descMatcher);
        bowExtractor->setVocabulary(vocabulary);
    }

    std::vector<cv::Mat> histograms;
    for (auto descriptors : descriptorsVec) {
        cv::Mat response_hist;
        bowExtractor->compute(descriptors, response_hist);
        
        cv::Mat normalizedHist;
        cv::normalize(response_hist, normalizedHist, 0, 1, cv::NORM_MINMAX);
        histograms.push_back(normalizedHist);
    }

    return histograms;
}


void BagOfWords::trainSVMs(const vector<vector<ImageData>>& images, const vector<cv::Mat>& histograms) {
    svms.resize(images.size()); // Ensure one SVM per class
    int histogramIndex = 0;

    for (size_t i = 0; i < images.size(); ++i) {
        vector<int> labels;

        // Create a matrix to hold the histograms for the current category
        cv::Mat categoryHistograms(0, histograms[0].cols, histograms[0].type()); // Initialize an empty matrix with the correct size and type

        // Fill the categoryHistograms matrix with histograms from the current category
        for (size_t j = 0; j < images[i].size(); ++j) {
            categoryHistograms.push_back(histograms[histogramIndex + j]);
            labels.push_back(1); // Positive class
        }

        // Now handle the negative samples for binary classification
        for (size_t k = 0; k < histograms.size(); ++k) {
            if (k < histogramIndex || k >= histogramIndex + images[i].size()) {
                categoryHistograms.push_back(histograms[k]);
                labels.push_back(-1); // Negative class
            }
        }

        histogramIndex += images[i].size();

        // Convert vector of labels to cv::Mat
        cv::Mat labelsMat(labels.size(), 1, CV_32SC1, &labels[0]);

        // Set up and train the SVM for this category
        cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::create();
        svm->setType(cv::ml::SVM::ONE_CLASS);
        svm->setKernel(cv::ml::SVM::RBF);
        svm->setNu(constants::nu);
        svm->setTermCriteria(constants::termCriteria);
        svm->train(categoryHistograms, cv::ml::ROW_SAMPLE, labelsMat); // Train with histogram data
        svms[i] = svm;
    }
}

void BagOfWords::SVMpredict(const vector<vector<ImageData>>& images) {
    std::vector<cv::Mat> histograms = getHistograms(images, false);
    std::vector<int> tp(images.size(), 0);
    std::vector<int> fp(images.size(), 0);
    std::vector<int> tn(images.size(), 0);
    std::vector<int> fn(images.size(), 0);
    int t = 0;
    for (int i = 0; i < images.size(); ++i) {
        for (int j = 0; j < images[i].size(); ++j) {
            for (int k = 0; k < svms.size(); ++k) {
                float response = svms[k]->predict(histograms[t], cv::noArray(), cv::ml::StatModel::RAW_OUTPUT);
                if (response > constants::threshold) {
                    if (k == i) {
                        tp[k]++;
                    } else {
                        fp[k]++;
                    }
                } else {
                    if (k == i) {
                        fn[k]++;
                    } else {
                        tn[k]++;
                    }
                }
            }
            t++;   
        }
    }
    for (int i = 0; i < images.size(); ++i) {
        std::cout << "---" << std::endl;
        std::cout << "Precision for " << images[i][0].label << ": " << (float)tp[i] / (tp[i] + fp[i]) << std::endl;
        std::cout << "Recall for " << images[i][0].label << ": " << (float)tp[i] / (tp[i] + fn[i]) << std::endl;
        std::cout << "Accuracy for " << images[i][0].label << ": " << (float)(tp[i] + tn[i]) / (tp[i] + tn[i] + fp[i] + fn[i]) << std::endl;
    }
}