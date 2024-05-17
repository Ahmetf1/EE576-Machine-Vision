#include <ImageProcessor.h>
#include <iostream>
#include <filesystem>
#include <constants.h>
#include <algorithm>

using namespace cv;
using namespace std;

ImageProcessor::ImageProcessor() : N1(0), N2(0) {}

void ImageProcessor::loadImages(const string& path) {
    files = getFiles(path);
    
    if (!files.empty()) {
        // assuming the first file is the target
        img = imread(files[0]);
        if (img.empty()) {
            cerr << "Failed to load images. Check path in constants.h" << endl;
            return;
        }

        // update image dimensions
        N1 = img.rows;
        N2 = img.cols;

        // resize for display if necessary
        displayImage = img.clone();

        grid = Mat::zeros(N1 * 2, N2 * 2, img.type());
        // copy welcome screen to the grid full screen
        grid = createWelcomeScreen(constants::welcomeMessage, N2 * 2, N1 * 2);
    }
}

void ImageProcessor::displayImages(int num) {
    if (!files.empty()) {
        if (num < 1 || num >= files.size()) {
            cerr << "Invalid image number." << endl;
            return;
        }
        img = imread(files[num-1]);

        if (img.empty()) {
            cerr << "Failed to load image." << endl;
            return;
        }
        // resize for display if necessary
        displayImage = img.clone();
        fillGrid(grid, displayImage, 0);
        cv::Mat segmented = segmentGreenRegion(displayImage);
        auto [boundaryImage, largestInnerContour] = findBoundaries(segmented);
        cv::Mat rotatedImg = displayImage;
        if (!largestInnerContour.empty()) {
            // create a mask from the largest inner contour
            cv::Mat mask = cv::Mat::zeros(img.size(), CV_8UC1);
            cv::drawContours(mask, std::vector<std::vector<cv::Point>>{largestInnerContour}, -1, cv::Scalar(255), cv::FILLED);

            // apply the mask to the original image
            cv::Mat maskedImage;
            displayImage.copyTo(maskedImage, mask);


            rotatedImg = rotateToHorizontal(maskedImage, largestInnerContour); // Example rotation function
        }
        fillGrid(grid, segmented, 1);
        fillGrid(grid, boundaryImage, 2);
        fillGrid(grid, rotatedImg, 3);

        imshow("Display", grid);
    }
}


void ImageProcessor::fillGrid(Mat& grid, const Mat& img, int position) {
    Mat targetROI = grid(Rect((position % 2) * N2, (position / 2) * N1, img.cols, img.rows));
    img.copyTo(targetROI);
}

vector<string> ImageProcessor::getFiles(const string& path) {
    vector<string> files;
    try {
        if (!filesystem::exists(path)) {
            cerr << "ERROR: Path does not exist. Correct it in constants.h file" << endl;
            return files;
        }
        // Iterate over the folders and add the files to the vector
        for (const auto& entry : filesystem::directory_iterator(path)) {
            for (const auto& file : filesystem::directory_iterator(entry.path())) {
                files.push_back(file.path().string());
            }
        }

        std::sort(files.begin(), files.end());
    } catch (const filesystem::filesystem_error& e) {
        cerr << e.what() << endl;
        return files;
    }
    return files;
}



cv::Mat ImageProcessor::createWelcomeScreen(const std::vector<std::string>& messages, int width, int height) {

    // Create a black image
    cv::Mat welcomeScreen = cv::Mat::zeros(cv::Size(width, height), CV_8UC3);

    // Define the rectangle for the instructions
    cv::Rect instructionRect(50, height / 4, width - 50, height / 2);

    // Draw a black rectangle for the instructions background
    cv::rectangle(welcomeScreen, instructionRect, cv::Scalar(0, 0, 0), cv::FILLED);

    // Set the text properties
    int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    double fontScale = 0.75;
    int thickness = 2;
    cv::Scalar textColor(255, 255, 255); // White color for the text

    // Calculate the position to center the text inside the rectangle
    int baseline = 0;
    cv::Size textSize = cv::getTextSize(messages[0], fontFace, fontScale, thickness, &baseline);
    cv::Point textOrg((welcomeScreen.cols - textSize.width) / 2, 
                      instructionRect.y + (instructionRect.height + textSize.height) / 2);

    // Put the instruction texts in vector onto the welcome screen
    for (int i = 0; i < messages.size(); i++) {
        cv::putText(welcomeScreen, messages[i], textOrg, fontFace, fontScale, textColor, thickness);
        textOrg.y += textSize.height * 2;
    }

    

    return welcomeScreen;
}

void ImageProcessor::run() {
    namedWindow(constants::windowName, WINDOW_AUTOSIZE);
    imshow(constants::windowName, grid);

    while (true) {
        int number = -1;
        char key = waitKey(0);
        if (key == 'q') break;
        else if (key == 13 || key == 10) {
            displayImages(selectedImage);
        } 
        // Move with a and d keys
        else if (key == 'a') {
            selectedImage -= 1;
            if (selectedImage < 0) selectedImage = 0;
            displayImages(selectedImage);
        } else if (key == 'd') {
            selectedImage += 1;
            if (selectedImage >= files.size() -1 ) selectedImage = files.size() - 2;
            displayImages(selectedImage);
        }
        else if (key >= '0' && key <= '9') {
            // If the key is a digit, we process it to construct a number
            string numberStr = "";
            numberStr += key; // Add the first digit

            // Continue to capture digits until Enter is pressed
            while(true) {
                key = waitKey(0);
                if (key == 13 || key == 10) { // If Enter is pressed, break
                    number = atoi(numberStr.c_str());
                    selectedImage = number;
                    displayImages(number);
                    std::cout << "Current image: " << number << std::endl;
                    break;
                }
                else if (key >= '0' && key <= '9') {
                    numberStr += key; // Append the digit to the number string
                }
            }

        }
    }
}

int ImageProcessor::extractNumber(const string& filename) {
    size_t lastSlash = filename.find_last_of("/\\");
    size_t start = lastSlash == string::npos ? 0 : lastSlash + 1;
    size_t dot = filename.find_last_of(".");
    size_t end = dot == string::npos ? filename.length() : dot;
    
    string numberPart = filename.substr(start, end - start);
    // Extract number from the mixed string (containing letters and numbers)
    string numericString;
    for (char c : numberPart) {
        if (isdigit(c)) {
            numericString += c;
        }
    }
    return numericString.empty() ? 0 : stoi(numericString);
}

cv::Mat ImageProcessor::segmentGreenRegion(const cv::Mat& img) {
    cv::Mat hsvImg, greenMask, segmented;

    // Convert the image from BGR to HSV color space
    cv::cvtColor(img, hsvImg, cv::COLOR_BGR2HSV);

    // Define the range of green color in HSV
    cv::inRange(hsvImg, constants::SegLowLimit, constants::SegHighLimit, greenMask); // Adjust these values as needed

    // Optional: Apply morphological operations to clean up the mask
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
    cv::morphologyEx(greenMask, greenMask, cv::MORPH_CLOSE, kernel); // Close operation to fill small holes and gaps in the segmented area
    cv::morphologyEx(greenMask, greenMask, cv::MORPH_OPEN, kernel); // Open operation to remove small noise

    // Find connected components
    cv::Mat labels, stats, centroids;
    int nLabels = cv::connectedComponentsWithStats(greenMask, labels, stats, centroids);

    // Find the largest connected component (excluding the background)
    int largestLabel = 1;
    int largestArea = stats.at<int>(1, cv::CC_STAT_AREA);
    for(int i = 2; i < nLabels; i++) {
        int area = stats.at<int>(i, cv::CC_STAT_AREA);
        if (area > largestArea) {
            largestArea = area;
            largestLabel = i;
        }
    }

    // Create a mask for the largest component
    cv::Mat largestComponentMask = (labels == largestLabel);

    // Apply the mask of the largest component to the original image to get the segmented green region
    img.copyTo(segmented, largestComponentMask);

    return segmented;
}

std::pair<cv::Mat, std::vector<cv::Point>> ImageProcessor::findBoundaries(const cv::Mat& mask) {
    cv::Mat singleChannelMask;
    if (mask.channels() == 1 && mask.type() == CV_8UC1) {
        singleChannelMask = mask;
    } else {
        cv::cvtColor(mask, singleChannelMask, cv::COLOR_BGR2GRAY);
    }

    cv::threshold(singleChannelMask, singleChannelMask, 0, 255, cv::THRESH_BINARY);

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(singleChannelMask, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    cv::Mat boundaryImage = cv::Mat::zeros(singleChannelMask.size(), CV_8UC3);

    double maxInnerArea = 0;
    int maxInnerIndex = -1;
    for (int i = 0; i < contours.size(); i++) {
        if (hierarchy[i][3] != -1) { // Checking for inner contours
            double area = cv::contourArea(contours[i]);
            if (area > maxInnerArea) {
                maxInnerArea = area;
                maxInnerIndex = i;
            }
        }
    }

    std::vector<cv::Point> largestInnerContour;
    for (int i = 0; i < contours.size(); i++) {
        if (hierarchy[i][3] == -1) {
            cv::drawContours(boundaryImage, contours, i, constants::OuterContourColor, 2); // Outer contours in blue
        } else if (i == maxInnerIndex) {
            cv::drawContours(boundaryImage, contours, i, constants::InnerContourColor, 2); // Largest inner contour in green
            largestInnerContour = contours[i]; // Save the largest inner contour
        }
    }

    return {boundaryImage, largestInnerContour};
}

cv::Mat ImageProcessor::rotateToHorizontal(const cv::Mat& img, const std::vector<cv::Point>& contour) {
    if (contour.size() < 4) {
        return img;
    }
    cv::RotatedRect rotatedRect = cv::minAreaRect(contour);

    // Step 3: Calculate the angle of rotation
    float angle = rotatedRect.angle;
    if (rotatedRect.size.width < rotatedRect.size.height) {
        angle += 90.0; // Adjust the angle for tall rectangles
    }

    // Step 4: Rotate the image around the center of the rectangle to make it horizontal
    cv::Mat rotationMatrix = cv::getRotationMatrix2D(rotatedRect.center, angle, 1.0);
    cv::Mat rotatedImg;
    cv::warpAffine(img, rotatedImg, rotationMatrix, img.size(), cv::INTER_CUBIC);

    return rotatedImg;
}

#include <filesystem>

void ImageProcessor::processAllImages(const std::string& DataPath, const std::string& OutputPath) {
    for (const auto& classEntry : std::filesystem::directory_iterator(DataPath)) {
        std::string className = classEntry.path().filename().string();
        std::string outputDir = OutputPath + "/" + className;
        std::filesystem::create_directories(outputDir);

        int processedImagesCount = 0; // Count of successfully processed images

        for (const auto& imageEntry : std::filesystem::directory_iterator(classEntry.path())) {
            cv::Mat img = cv::imread(imageEntry.path().string());
            if (img.empty()) {
                std::cerr << "Warning: Could not read image: " << imageEntry.path() << std::endl;
                continue; // Skip processing this image
            }

            // Apply your processing
            cv::Mat segmented = segmentGreenRegion(img); // Placeholder for actual segmentation function
            auto [boundaryImage, largestInnerContour] = findBoundaries(segmented); // Placeholder for actual boundary detection

            if (largestInnerContour.empty()) {
                std::cerr << "Warning: No contour found in image: " << imageEntry.path() << std::endl;
                continue; // Skip processing this image
            }

            cv::Mat mask = cv::Mat::zeros(img.size(), CV_8UC1);
            cv::drawContours(mask, std::vector<std::vector<cv::Point>>{largestInnerContour}, -1, cv::Scalar(255), cv::FILLED);

            cv::Mat maskedImage;
            img.copyTo(maskedImage, mask);

            cv::Mat rotatedImg = rotateToHorizontal(maskedImage, largestInnerContour); // Placeholder for actual rotation function
            
            std::string outputPath = outputDir + "/" + imageEntry.path().filename().string();
            cv::imwrite(outputPath, rotatedImg);

            processedImagesCount++;
        }

        // Delete the folder if less than 2 images were processed
        if (processedImagesCount < 2) {
            std::filesystem::remove_all(outputDir);
            std::cout << "Removed directory due to insufficient processed images: " << outputDir << std::endl;
        }
    }
}
