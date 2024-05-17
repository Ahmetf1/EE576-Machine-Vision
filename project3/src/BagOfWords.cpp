#include <BagOfWords.h>
#include <constants.h>

#include <vector>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>


using namespace std;

cv::Mat BagOfWords::run(const string& path)
{
    loadImages(path);
    // check if the images are loaded
    if (images.empty()) {
        cerr << "ERROR: No images loaded. Check the path in constants.h file" << endl;
        return cv::Mat();
    }
    vector<cv::Mat> descriptors = getDescriptors(images);
    cv::Mat Vocabulary = buildVocabulary(descriptors);
    vector<cv::Mat> histograms = buildHistograms(descriptors, Vocabulary);
    calculateAverageDescriptors(histograms);
    cv::Mat similarityMatrix = calculateSimilarityMatrix();
    return similarityMatrix;
}

void BagOfWords::visualizeSimilarityMatrix(const cv::Mat& similarityMatrix) {
    double minVal, maxVal;
    cv::minMaxLoc(similarityMatrix, &minVal, &maxVal); // Find min and max values
    cv::Mat normalizedSimMatrix;
    // normalize the similarity matrix to [0, 255]
    similarityMatrix.convertTo(normalizedSimMatrix, CV_8U, 255.0 / (maxVal - minVal), -minVal * 255.0 / (maxVal - minVal));

    // resize the matrix for better visualization
    int scaleFactor = 50; // Adjust this factor based on how large you want the cells to be
    cv::Mat resizedSimMatrix;
    cv::resize(normalizedSimMatrix, resizedSimMatrix, cv::Size(), scaleFactor, scaleFactor, cv::INTER_NEAREST);

    // convert to color for adding colored labels
    cv::Mat colorSimMatrix;
    cv::cvtColor(resizedSimMatrix, colorSimMatrix, cv::COLOR_GRAY2BGR);

    // set font properties
    int fontFace = cv::FONT_HERSHEY_PLAIN;
    double fontScale = 1.5;
    int thickness = 2;

    // Add labels along the y axis
    for (size_t i = 0; i < classLabels.size(); ++i) {
        // Calculate the position for each label
        int posY = (i + 1) * scaleFactor * similarityMatrix.rows / classLabels.size() - scaleFactor / 4;

        // Draw the labels along the side
        cv::putText(colorSimMatrix, indexToLabelMap[i], cv::Point(0, posY), fontFace, fontScale, cv::Scalar(0, 255, 0), thickness);
    }

    // Display the image with labels
    cv::namedWindow("Similarity Matrix with Labels", cv::WINDOW_AUTOSIZE);
    cv::imshow("Similarity Matrix with Labels", colorSimMatrix);
    cv::waitKey(0); // Wait for a key press to close the window
}


void BagOfWords::loadImages(const string &path)
{
    images.clear();
    try {
        if (!filesystem::exists(path)) {
            cerr << "ERROR: Path does not exist. Correct it in constants.h file" << endl;
        }
        // iterate over the folders and add the files to the vector
        for (const auto& entry : filesystem::directory_iterator(path)) {
            for (const auto& file : filesystem::directory_iterator(entry.path())) {
                ImageWithLabel image;
                image.label = entry.path().filename().string();
                image.image = cv::imread(file.path().string());
                images.push_back(image);
            }
        }
    } catch (const filesystem::filesystem_error& e) {
        cerr << e.what() << endl;
    }
}

std::vector<cv::Mat> BagOfWords::getDescriptors(vector<ImageWithLabel> images)
{
    cv::Ptr<cv::SIFT> detector = cv::SIFT::create();
    vector<cv::Mat> descriptors;
    // iterate over the images and get the descriptors
    for (const auto& image : images) {
        // convert the image to grayscale
        cv::Mat gray;
        cv::cvtColor(image.image, gray, cv::COLOR_BGR2GRAY);
        // detect the keypoints
        vector<cv::KeyPoint> keypoints;
        detector->detect(gray, keypoints);
        // compute the descriptors
        cv::Mat descriptor;
        detector->compute(gray, keypoints, descriptor);
        // add the descriptors to the vector
        descriptors.push_back(descriptor);
    }
    return descriptors;
}

cv::Mat BagOfWords::buildVocabulary(const vector<cv::Mat>& descriptors) {
    // concatenate all descriptors into a single cv::Mat object
    cv::Mat allDescriptors;
    for (const auto& descriptor : descriptors) {
        allDescriptors.push_back(descriptor);
    }
    // apply k-means clustering to find the vocabulary
    int vocabularySize = constants::vocabularySize; // Example size of the vocabulary
    cv::Mat labels;
    cv::Mat vocabulary;
    kmeans(allDescriptors, vocabularySize, labels, cv::TermCriteria(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 100, 0.1), 1, cv::KMEANS_PP_CENTERS, vocabulary);
    return vocabulary;
}

vector<cv::Mat> BagOfWords::buildHistograms(const vector<cv::Mat>& descriptors, const cv::Mat& vocabulary) {
    // use a FLANN-based matcher to match descriptors to vocabulary
    cv::FlannBasedMatcher matcher;
    vector<cv::Mat> histograms(descriptors.size());
    for (size_t i = 0; i < descriptors.size(); ++i) {
        vector<cv::DMatch> matches;
        matcher.match(descriptors[i], vocabulary, matches);
        // build histogram for the current set of descriptors
        cv::Mat histogram = cv::Mat::zeros(1, vocabulary.rows, CV_32F);
        for (const auto& match : matches) {
            int idx = match.trainIdx; // Index of the vocabulary word
            histogram.at<float>(idx) += 1.0;
        }
        normalize(histogram, histogram, 1.0, 0.0, cv::NORM_L1); // Normalize the histogram
        histograms[i] = histogram;
    }
    return histograms;
}

void BagOfWords::calculateAverageDescriptors(const vector<cv::Mat>& histograms) {
    map<int, vector<cv::Mat>> classHistograms;
    // organize histograms by class
    for (size_t i = 0; i < images.size(); ++i) {
        int label = getLabelIndex(images[i].label); // Convert label string to int, if necessary
        classHistograms[label].push_back(histograms[i]);
        indexToLabelMap[label] = images[i].label; // This assumes labels are consistent per index
    }
    // calculate average descriptor for each class
    for (const auto& pair : classHistograms) {
        cv::Mat sumHistogram = cv::Mat::zeros(pair.second[0].rows, pair.second[0].cols, pair.second[0].type());
        for (const cv::Mat& histogram : pair.second) {
            sumHistogram += histogram;
        }
        sumHistogram /= pair.second.size();
        averageDescriptors[pair.first] = sumHistogram;
        classLabels.push_back(pair.first);
    }
}

cv::Mat BagOfWords::calculateSimilarityMatrix() {
    int numClasses = averageDescriptors.size();
    cv::Mat similarityMatrix = cv::Mat::zeros(numClasses, numClasses, CV_32F);
    for (int i = 0; i < numClasses; ++i) {
        for (int j = 0; j < numClasses; ++j) {
            if (i == j) {
                continue; // Skip diagonal
            }
            float dissimilarity = norm(averageDescriptors[classLabels[i]] - averageDescriptors[classLabels[j]], cv::NORM_L2);
            similarityMatrix.at<float>(i, j) = dissimilarity;
        }
    }
    return similarityMatrix;
}

int BagOfWords::getLabelIndex(const string &label)
{
    if (classLabelsMap.find(label) == classLabelsMap.end()) {
        classLabelsMap[label] = classLabelsMap.size();
    }
    return classLabelsMap[label];
}
