#include <ImageViewer.h>
#include <iostream>
#include <filesystem>
#include <constants.h>

using namespace cv;
using namespace std;

ImageViewer::ImageViewer() : N1(0), N2(0) {}

void ImageViewer::loadImages(const string& path) {
    files = getFiles(path);
    if (!files.empty()) {
        // Assuming the first file is the target
        img = imread(files[0]);
        if (img.empty()) {
            cerr << "Failed to load images. Check path in constants.h" << endl;
            return;
        }

        // Update image dimensions
        N1 = img.rows;
        N2 = img.cols;

        // Resize for display if necessary
        displayImage = img.clone();
        rotate(displayImage, rotatedImg, ROTATE_180);

        // Prepare the grid
        grid = Mat::zeros(N1 * 2, N2 * 2, img.type());
        // Copy welcome screen to the grid full screen
        // resize grid size
        grid = createWelcomeScreen(constants::welcomeMessage, N2 * 2, N1 * 2);
    }
}

void ImageViewer::displayImages(int num) {
    if (!files.empty()) {
        if (num < 1 || num >= files.size()+1) {
            cerr << "Invalid image number." << endl;
            return;
        }
        img = imread(files[num-1]);
        if (img.empty()) {
            cerr << "Failed to load image." << endl;
            return;
        }

        // Resize for display if necessary
        displayImage = img.clone();
        rotate(displayImage, rotatedImg, ROTATE_180);

        fillGrid(grid, displayImage, 0);
        fillGrid(grid, rotatedImg, 1);
        imshow("Display", grid);
    }
}

cv::Mat ImageViewer::cropRectangle(const cv::Mat& img, const cv::Point& center, int width, int height) {
    Rect roi(max(center.x - width / 2, 0), max(center.y - height / 2, 0), width, height);
    // Adjust ROI to be within the image boundaries
    roi = roi & Rect(0, 0, img.cols, img.rows);
    return img(roi).clone();
}

void ImageViewer::fillGrid(Mat& grid, const Mat& img, int position) {
    Mat targetROI = grid(Rect((position % 2) * N2, (position / 2) * N1, img.cols, img.rows));
    img.copyTo(targetROI);
}

vector<string> ImageViewer::getFiles(const string& path) {
    vector<string> files;
    try {
        if (!filesystem::exists(path)) {
            cerr << "ERROR: Path does not exist. Correct it in constants.h file" << endl;
            return files;
        }
        for (const auto& entry : filesystem::directory_iterator(path)) {
            files.push_back(entry.path().string());
        }
    } catch (const filesystem::filesystem_error& e) {
        cerr << e.what() << endl;
        return files;
    }
    return files;
}

void ImageViewer::mouseCallback(int event, int x, int y, int flags, void* userdata) {
    // Cast userdata to ImageViewer pointer
    auto* processor = reinterpret_cast<ImageViewer*>(userdata);
    if (processor->selectedImage == -1) {
        return;
    }
    if (event == EVENT_LBUTTONDOWN) {
        if (x < processor->N2 && y < processor->N1) { // First image
            processor->point1 = Point(x, y);
            Mat cropped = processor->cropRectangle(processor->displayImage, processor->point1, width, height);
            resize(cropped, cropped, Size(processor->N2, processor->N1));
            processor->fillGrid(processor->grid, cropped, 2);
        } else if (x >= processor->N2 && y < processor->N1) { // Second image (rotated)
            Point correctedPoint = Point(x - processor->N2, y);
            Mat cropped = processor->cropRectangle(processor->rotatedImg, correctedPoint, width, height);
            resize(cropped, cropped, Size(processor->N2, processor->N1));
            processor->fillGrid(processor->grid, cropped, 3);
        }
        imshow("Display", processor->grid);
    }
}

cv::Mat ImageViewer::createWelcomeScreen(const std::vector<std::string>& messages, int width, int height) {

    // Create a black image
    cv::Mat welcomeScreen = cv::Mat::zeros(cv::Size(width, height), CV_8UC3);

    // Define the rectangle for the instructions
    cv::Rect instructionRect(50, height / 4, width - 100, height / 2);

    // Draw a black rectangle for the instructions background
    cv::rectangle(welcomeScreen, instructionRect, cv::Scalar(0, 0, 0), cv::FILLED);

    // Set the text properties
    int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    double fontScale = 2;
    int thickness = 4;
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
void ImageViewer::run() {
    namedWindow("Display", WINDOW_AUTOSIZE);
    setMouseCallback("Display", ImageViewer::mouseCallback, this);
    imshow("Display", grid);

    while (true) {
        char key = waitKey(0);
        if (key == 'q') break;
        else if (key >= '0' && key <= '9') {
            // If the key is a digit, we process it to construct a number
            string numberStr = "";
            numberStr += key; // Add the first digit

            // Continue to capture digits until Enter is pressed
            while(true) {
                key = waitKey(0);
                if (key == 13 || key == 10) { // If Enter is pressed, break
                    // Convert the captured string to an integer
                    int number = atoi(numberStr.c_str());
                    selectedImage = number;
                    displayImages(number);
                    std::cout << number << std::endl;
                    break;
                }
                else if (key >= '0' && key <= '9') {
                    numberStr += key; // Append the digit to the number string
                }
            }

        }
    }
}