#include <ImageFlow.h>
#include <iostream>
#include <filesystem>
#include <constants.h>
#include <algorithm>

using namespace cv;
using namespace std;

// Constructor initializes N1 and N2 to 0
ImageFlow::ImageFlow() : N1(0), N2(0) {}

// Load images from the specified paths
void ImageFlow::loadImages(const string& path, const string& mask_path) {
    // Get all image and mask file paths
    std::pair<std::vector<string>, std::vector<string>> all_files = getFiles(path, mask_path);
    files = all_files.first;
    mask_files = all_files.second;
    
    if (!files.empty()) {
        // Load the first image
        img = imread(files[0]);
        if (img.empty()) {
            cerr << "Failed to load images. Check path in constants.h" << endl;
            return;
        }

        // Set image dimensions
        N1 = img.rows;
        N2 = img.cols;

        // Clone the image for display
        displayImage = img.clone();
        
        // Initialize a blank grid
        grid = Mat::zeros(N1 * 2, N2 * 2, img.type());
        
        // Create and assign a welcome screen to the grid
        grid = createWelcomeScreen(constants::welcomeMessage, N2 * 2, N1 * 2);
    }
}

// Display selected images and processed results
void ImageFlow::displayImages(int num) {
    if (!files.empty()) {
        // Check if the image number is valid
        if (num < 1 || num >= files.size()) {
            cerr << "Invalid image number." << endl;
            return;
        }

        // Load images and masks
        img = imread(files[num-1]);
        img2 = imread(files[num]);
        mask_img = imread(mask_files[num-1], IMREAD_GRAYSCALE);
        mask_img2 = imread(mask_files[num], IMREAD_GRAYSCALE);

        // Apply mask to the first image
        cv::Mat img_masked = applyMask(mask_img, img);

        if (img.empty() || img2.empty()) {
            cerr << "Failed to load image." << endl;
            return;
        }

        // Clone images for display
        displayImage = img.clone();
        displayImage2 = img2.clone();
        
        // Fill the grid with images and processed results
        fillGrid(grid, displayImage, 0);
        fillGrid(grid, displayImage2, 1);
        fillGrid(grid, applyOpticalFlow(), 2);
        fillGrid(grid, applyTracking(), 3);  

        // Show the grid
        imshow("Display", grid);
    }
}

// Fill the grid with images at specified positions
void ImageFlow::fillGrid(Mat& grid, const Mat& img, int position) {
    // Calculate the target region of interest (ROI) in the grid
    Mat targetROI = grid(Rect((position % 2) * N2, (position / 2) * N1, img.cols, img.rows));
    // Copy the image to the target ROI
    img.copyTo(targetROI);
}

// Get file paths for images and masks
std::pair<std::vector<string>, std::vector<string>> ImageFlow::getFiles(const string& path, const string& mask_path) {
    vector<string> files;
    vector<string> mask_files;
    try {
        // Check if the path exists
        if (!filesystem::exists(path)) {
            cerr << "ERROR: Path does not exist. Correct it in constants.h file" << endl;
            return std::make_pair(files, mask_files);
        }
        // Iterate through the directory and add file paths to the list
        for (const auto& entry : filesystem::directory_iterator(path)) {
            if (entry.is_regular_file())
                files.push_back(entry.path().string());
        }

        // Sort files based on extracted numbers
        sort(files.begin(), files.end(), [this](const string& a, const string& b) {
            return extractNumber(a) < extractNumber(b);
        });

        // Generate corresponding mask file paths
        for (const auto& entry : files) {
            std::string file_mask_path = mask_path + "/" + entry.substr(entry.find_last_of("/\\") + 1);
            mask_files.push_back(file_mask_path);
        }

    } catch (const filesystem::filesystem_error& e) {
        cerr << e.what() << endl;
        return std::make_pair(files, mask_files);
    }
    return std::make_pair(files, mask_files);
}

// Apply mask to an image
cv::Mat ImageFlow::applyMask(const cv::Mat &mask, cv::Mat &img) {
    // Clone the original image
    cv::Mat new_img = img.clone();
    // Check if the mask and image have the same size
    if (mask.size() != img.size()) {
        cerr << "Mask and image sizes do not match." << endl;
        return new_img;
    }

    // Apply the mask to the image
    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
            // Set pixels to black where the mask is zero
            if (mask.at<uchar>(i, j) == 0) {
                new_img.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
            }
        }
    }
    return new_img;
}

// Extract number from file name
int ImageFlow::extractNumber(const std::string &s) {
    size_t start = s.find_last_of("/\\");
    size_t end = s.find_last_of(".");
    return stoi(s.substr(start + 1, end - start - 1));
}

// Apply optical flow to the images
cv::Mat ImageFlow::applyOpticalFlow() {
    Mat grayImg1, grayImg2;
    // Convert images to grayscale
    cvtColor(img, grayImg1, COLOR_BGR2GRAY);
    cvtColor(img2, grayImg2, COLOR_BGR2GRAY);

    // Compute optical flow between the images
    Mat flow;
    calcOpticalFlowFarneback(grayImg1, grayImg2, flow, 0.5, 3, 15, 3, 5, 1.2, 0);

    // Apply mask to the first image
    Mat output = applyMask(mask_img, img);

    // Load the corresponding mask image
    mask_img = imread(mask_files[selectedImage - 1], IMREAD_GRAYSCALE);
    if (mask_img.empty()) {
        cerr << "Failed to load mask." << endl;
        return output;
    }

    // Find all distinct objects in the mask
    Mat labeledMask, stats, centroids;
    int numLabels = connectedComponentsWithStats(mask_img, labeledMask, stats, centroids, 8, CV_32S);

    // Define a minimum area threshold to filter out small regions
    int minAreaThreshold = constants::minAreaThreshold;

    int k = 0;
    for (int label = 1; label < numLabels; ++label) {
        int area = stats.at<int>(label, CC_STAT_AREA);
        if (area < minAreaThreshold) {
            continue; // Skip small regions
        }
        k++;
        Point2f averageFlow(0.0f, 0.0f);
        int count = 0;

        // Calculate the average flow within the labeled region
        for (int y = 0; y < flow.rows; ++y) {
            for (int x = 0; x < flow.cols; ++x) {
                if (labeledMask.at<int>(y, x) == label) { // Only consider pixels belonging to the current label
                    Vec2f flow_at_point = flow.at<Vec2f>(y, x);
                    averageFlow.x += flow_at_point[0];
                    averageFlow.y += flow_at_point[1];
                    ++count;
                }
            }
        }

        if (count > 0) {
            averageFlow.x /= count;
            averageFlow.y /= count;
        }

        // Draw the average flow vector on the output image
        Point center(centroids.at<double>(label, 0), centroids.at<double>(label, 1));
        arrowedLine(output, center, Point(center.x + static_cast<int>(averageFlow.x), center.y + static_cast<int>(averageFlow.y)), Scalar(0, 0, 255), 2, 8, 0, 0.4);
        std::cout << "Optical Flow: Average flow for object " << k << ": " << norm(averageFlow) << std::endl;
    }

    return output;
}

// Apply tracking to the images
cv::Mat ImageFlow::applyTracking() {
    Mat grayImg1, grayImg2;
    // Convert images to grayscale
    cvtColor(img, grayImg1, COLOR_BGR2GRAY);
    cvtColor(img2, grayImg2, COLOR_BGR2GRAY);

    // Apply mask to the first image
    Mat output = applyMask(mask_img, img);

    // Find all distinct objects in the mask
    Mat labeledMask, stats, centroids;
    int numLabels = connectedComponentsWithStats(mask_img, labeledMask, stats, centroids, 8, CV_32S);

    // Define a minimum area threshold to filter out small regions
    int minAreaThreshold = 500;

    int k = 0;
    for (int label = 1; label < numLabels; ++label) {
        int area = stats.at<int>(label, CC_STAT_AREA);
        if (area < minAreaThreshold) {
            continue; // Skip small regions
        }

        k++;
        // Collect points to track within the labeled region
        std::vector<Point2f> pointsToTrack;
        for (int y = 0; y < labeledMask.rows; ++y) {
            for (int x = 0; x < labeledMask.cols; ++x) {
                if (labeledMask.at<int>(y, x) == label) {
                    pointsToTrack.push_back(Point2f(x, y));
                }
            }
        }

        // Track the points using optical flow
        std::vector<Point2f> trackedPoints;
        std::vector<uchar> status;
        std::vector<float> err;
        calcOpticalFlowPyrLK(grayImg1, grayImg2, pointsToTrack, trackedPoints, status, err);

        Point2f averageFlow(0.0f, 0.0f);
        int count = 0;

        // Calculate the average flow within the tracked points
        for (size_t i = 0; i < pointsToTrack.size(); ++i) {
            if (status[i]) {
                Point2f flow = trackedPoints[i] - pointsToTrack[i];
                averageFlow += flow;
                ++count;
            }
        }

        if (count > 0) {
            averageFlow.x /= count;
            averageFlow.y /= count;
        }

        // Draw the average flow vector on the output image
        Point center(centroids.at<double>(label, 0), centroids.at<double>(label, 1));
        arrowedLine(output, center, Point(center.x + static_cast<int>(averageFlow.x), center.y + static_cast<int>(averageFlow.y)), Scalar(0, 0, 255), 2, 8, 0, 0.4);
        // Print the norm of the average flow vector
        std::cout << "Tracking: Average flow for object " << k << ": " << norm(averageFlow) << std::endl;
    }

    return output;
}

// Create a welcome screen
cv::Mat ImageFlow::createWelcomeScreen(const std::vector<std::string>& messages, int width, int height) {
    // Create a black image
    cv::Mat welcomeScreen = cv::Mat::zeros(cv::Size(width, height), CV_8UC3);

    // Define the rectangle for the instructions
    cv::Rect instructionRect(50, height / 4, width - 100, height / 2);

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

    // Put the instruction texts from vector onto the welcome screen
    for (int i = 0; i < messages.size(); i++) {
        cv::putText(welcomeScreen, messages[i], textOrg, fontFace, fontScale, textColor, thickness);
        textOrg.y += textSize.height * 2;
    }

    return welcomeScreen;
}

// Main function to run the image flow display
void ImageFlow::run() {
    // Create a window for display
    namedWindow("Display", WINDOW_AUTOSIZE);
    imshow("Display", grid);

    while (true) {
        int number = -1;
        char key = waitKey(0); // Wait for a key press
        if (key == 'q') break; // Quit on 'q' key press
        else if (key == 13 || key == 10) {
            // Display images on Enter key press
            displayImages(selectedImage);
        }
        else if (key >= '0' && key <= '9') {
            // Process digit key presses to construct a number
            string numberStr = "";
            numberStr += key;

            // Continue to capture digits until Enter is pressed
            while(true) {
                key = waitKey(0);
                if (key == 13 || key == 10) { // If Enter is pressed, break
                    number = atoi(numberStr.c_str());
                    selectedImage = number;
                    displayImages(number);
                    std::cout << "Current image: " << number << "\n" << std::endl;
                    break;
                }
                else if (key >= '0' && key <= '9') {
                    numberStr += key; // Append the digit to the number string
                }
            }
        }
    }
}
