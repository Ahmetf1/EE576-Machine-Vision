/*
    * @file bow.cpp
    * @brief This file contains the main function to run the Bag of Words and SVM algorithm.
*/
#include <iostream>
#include <BagOfWords.h>
#include <constants.h>
#include <DataProvider.h>


int main() {
    // Change the useeDepth parameter change between RGB or D images
    DataProvider dataProvider(constants::dataPath, constants::labels, false);
    std::cout << "nu: " << constants::nu << std::endl;
    std::cout << "threshold: " << constants::threshold << std::endl;
    std::cout << "vocabulary Size: " << constants::vocabularySize << std::endl;
    BagOfWords bow;
    std::cout << "-----------   Running BOW   -----------" << std::endl;
    bow.run(dataProvider); // Adjust the path as necessary
    std::cout << "-----------   Predicting   -----------" << std::endl;
    bow.predict(dataProvider.getTestset()); // Adjust the path as necessary

    return 0;
}