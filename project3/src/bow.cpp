#include <iostream>
#include <ImageProcessor.h>
#include <BagOfWords.h>
#include <constants.h>

int main() {
    ImageProcessor processor;
    processor.processAllImages(constants::dataPath, constants::outputPath); // Adjust the path as necessary
    BagOfWords bow;
    cv::Mat similarity_matrix = bow.run(constants::outputPath); // Adjust the path as necessary
    
    // Comment out the following line if you don't want to visualize the similarity matrix
    bow.visualizeSimilarityMatrix(similarity_matrix);
    return 0;
}