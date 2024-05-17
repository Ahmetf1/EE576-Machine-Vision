#include <ImageTransformer.h>
#include <iostream>
#include <filesystem>
#include <constants.h>
#include <algorithm>

using namespace cv;
using namespace std;

ImageTransformer::ImageTransformer() : N1(0), N2(0) {}

void ImageTransformer::loadImages(const string& path) {
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

        // Prepare the grid
        grid = Mat::zeros(N1 * 2, N2 * 2, img.type());
        // Copy welcome screen to the grid full screen
        // resize grid size
        grid = createWelcomeScreen(constants::welcomeMessage, N2 * 2, N1 * 2);
    }
}

void ImageTransformer::displayImages(int num) {
    if (!files.empty()) {
        if (num < 1 || num >= files.size()) {
            cerr << "Invalid image number." << endl;
            return;
        }
        img = imread(files[num-1]);
        img2 = imread(files[num]);

        if (img.empty() || img2.empty()) {
            cerr << "Failed to load image." << endl;
            return;
        }
        // Resize for display if necessary
        displayImage = img.clone();
        displayImage2 = img2.clone();
        // Check if points are available
        if (points.size() == points2.size() && points.size() > 3) {
            matchedImage = findHomographyMap(points, points2, img, img2);
            matchedImageCustom = computeHomography(points, points2, img, img2);
            fillGrid(grid, matchedImageCustom, 0);
            fillGrid(grid, matchedImage, 2);
        } else {
            cv::Mat blackImage = Mat::zeros(N1, N2, img.type());
            fillGrid(grid, displayImage, 0);
            fillGrid(grid, displayImage2, 1);
            fillGrid(grid, blackImage, 2);
            fillGrid(grid, blackImage, 3);
        }    
        points.clear();
        points2.clear();
        firstImageTurn = true;

        imshow("Display", grid);
    }
}

cv::Mat ImageTransformer::cropRectangle(const cv::Mat& img, const cv::Point& center, int width, int height) {
    Rect roi(max(center.x - width / 2, 0), max(center.y - height / 2, 0), width, height);
    // Adjust ROI to be within the image boundaries
    roi = roi & Rect(0, 0, img.cols, img.rows);
    return img(roi).clone();
}

void ImageTransformer::fillGrid(Mat& grid, const Mat& img, int position) {
    Mat targetROI = grid(Rect((position % 2) * N2, (position / 2) * N1, img.cols, img.rows));
    img.copyTo(targetROI);
}

vector<string> ImageTransformer::getFiles(const string& path) {
    vector<string> files;
    try {
        if (!filesystem::exists(path)) {
            cerr << "ERROR: Path does not exist. Correct it in constants.h file" << endl;
            return files;
        }
        for (const auto& entry : filesystem::directory_iterator(path)) {
            files.push_back(entry.path().string());
        }

        // Sort files using a custom lambda that compares the extracted numbers
        std::sort(files.begin(), files.end(), [this](const string& a, const string& b) {
            return extractNumber(a) < extractNumber(b);
        });
    } catch (const filesystem::filesystem_error& e) {
        cerr << e.what() << endl;
        return files;
    }
    return files;
}

void ImageTransformer::mouseCallback(int event, int x, int y, int flags, void* userdata) {
    // Cast userdata to ImageTransformer pointer
    auto* processor = reinterpret_cast<ImageTransformer*>(userdata);
    if (processor->selectedImage == -1) {
        return;
    }
    if (event == EVENT_LBUTTONDOWN) {
        if (x < processor->N2 && y < processor->N1) { // First image
            if (processor->firstImageTurn) {
                processor->point1 = Point(x, y);
                processor->points.push_back(processor->point1);
                processor->firstImageTurn = false;
                processor->PointColor = processor->generateRandomColor();
                circle(processor->grid, processor->point1, constants::circleRadius, processor->PointColor, constants::circleRadius);
            } 
        } else if (x >= processor->N2 && y < processor->N1) { // Second image
            if (!processor->firstImageTurn) {
                processor->point2 = Point(x - processor->N2, y);
                processor->points2.push_back(processor->point2);
                processor->firstImageTurn = true;
                cv::Point2f circleDraw = processor->point2;
                circleDraw.x += processor->N2; 
                circle(processor->grid, circleDraw, constants::circleRadius, processor->PointColor, constants::circleRadius);
            }
        }
        imshow("Display", processor->grid);
    }
}

cv::Mat ImageTransformer::createWelcomeScreen(const std::vector<std::string>& messages, int width, int height) {

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

    // Put the instruction texts in vector onto the welcome screen
    for (int i = 0; i < messages.size(); i++) {
        cv::putText(welcomeScreen, messages[i], textOrg, fontFace, fontScale, textColor, thickness);
        textOrg.y += textSize.height * 2;
    }

    

    return welcomeScreen;
}

void ImageTransformer::run() {
    namedWindow("Display", WINDOW_AUTOSIZE);
    setMouseCallback("Display", ImageTransformer::mouseCallback, this);
    imshow("Display", grid);

    while (true) {
        int number = -1;
        char key = waitKey(0);
        if (key == 'q') break;
        else if (key == 13 || key == 10) {
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

cv::Mat ImageTransformer::findHomographyMap(const std::vector<cv::Point2f>& pts1, const std::vector<cv::Point2f>& pts2, const cv::Mat& img1, const cv::Mat& img2) {
    if (pts1.empty() || pts2.empty() || pts1.size() != pts2.size()) {
        std::cerr << "Error: Point sets are empty or not of equal size." << std::endl;
        return cv::Mat();
    }
    
    // Compute the homography matrix using RANSAC
    cv::Mat H = cv::findHomography(pts1, pts2, cv::RANSAC);

    // Warp the second image using the homography matrix
    cv::Mat img2Transformed;
    cv::warpPerspective(img2, img2Transformed, H, img2.size());

    // Convert points to keypoints for visualization
    std::vector<cv::KeyPoint> keypoints1, keypoints2Transformed;
    for (const auto& pt : pts1) {
        keypoints1.emplace_back(pt, 1.f);
    }

    std::vector<cv::Point2f> pts2Transformed;
    for (const auto& pt : points2) {
        // Manually apply homography to points2 for keypoint generation (optional, as they may be off the visible area)
        std::vector<cv::Point2f> ptWrapper(1, pt), ptTransformed;
        cv::perspectiveTransform(ptWrapper, ptTransformed, H);
        keypoints2Transformed.emplace_back(ptTransformed[0], 1.f);
        pts2Transformed.push_back(ptTransformed[0]);
    }

    // Calculate the average error between the original and transformed points
    // find pts2transformed from keypoints2Transformed
    double error = calculateAverageError(pts1, pts2Transformed);

    std::cout << "Average error in built-in method: " << error << std::endl;

    // Create dummy matches since every point is assumed to match its counterpart
    std::vector<cv::DMatch> matches;
    for (size_t i = 0; i < pts1.size(); ++i) {
        matches.emplace_back(i, i, 0);
    }

    // Draw matches
    cv::Mat imgMatches;
    cv::drawMatches(img1, keypoints1, img2Transformed, keypoints2Transformed, matches, imgMatches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
    return imgMatches;
}

double ImageTransformer::calculateAverageError(const std::vector<cv::Point2f>& originalPoints, const std::vector<cv::Point2f>& transformedPoints) {
    // Check if the vectors are of the same size
    if (originalPoints.size() != transformedPoints.size() || originalPoints.empty()) {
        std::cerr << "Error: Points vectors are empty or not of the same size." << std::endl;
        return -1.0; // Return an error code
    }

    // Sum the Euclidean distances between original and transformed points
    double totalError = 0.0;
    for (size_t i = 0; i < originalPoints.size(); i++) {
        cv::Point2f diff = originalPoints[i] - transformedPoints[i];
        totalError += cv::sqrt(diff.x*diff.x + diff.y*diff.y);
    }

    // Calculate the average error
    double averageError = totalError / originalPoints.size();
    return averageError;
}

cv::Scalar ImageTransformer::generateRandomColor() {
    // Initialize random seed
    srand(time(0));

    // Generate random BGR values
    // 255 + 12
    int blue = rand() % 256;  // Random blue component [0, 255]
    int green = rand() % 256; // Random green component [0, 255]
    int red = rand() % 256;   // Random red component [0, 255]

    // Return the random color
    return cv::Scalar(blue, green, red);
}

cv::Mat ImageTransformer::computeHomography(const std::vector<cv::Point2f>& pts1, const std::vector<cv::Point2f>& pts2, const cv::Mat& img1, const cv::Mat& img2) {
    if (pts1.size() != pts2.size() || pts1.size() < 4) {
        std::cerr << "Insufficient or unequal number of points." << std::endl;
        return cv::Mat();
    }

    const int N = pts1.size();
    cv::Mat A(2*N, 9, CV_32F);
    for (int i = 0; i < N; ++i) {
        float x = pts1[i].x, y = pts1[i].y;
        float xp = pts2[i].x, yp = pts2[i].y;

        A.at<float>(2*i, 0) = -x;
        A.at<float>(2*i, 1) = -y;
        A.at<float>(2*i, 2) = -1;
        A.at<float>(2*i, 3) = 0;
        A.at<float>(2*i, 4) = 0;
        A.at<float>(2*i, 5) = 0;
        A.at<float>(2*i, 6) = x*xp;
        A.at<float>(2*i, 7) = y*xp;
        A.at<float>(2*i, 8) = xp;

        A.at<float>(2*i+1, 0) = 0;
        A.at<float>(2*i+1, 1) = 0;
        A.at<float>(2*i+1, 2) = 0;
        A.at<float>(2*i+1, 3) = -x;
        A.at<float>(2*i+1, 4) = -y;
        A.at<float>(2*i+1, 5) = -1;
        A.at<float>(2*i+1, 6) = x*yp;
        A.at<float>(2*i+1, 7) = y*yp;
        A.at<float>(2*i+1, 8) = yp;
    }

    cv::Mat w, u, vt;
    cv::SVD::compute(A, w, u, vt, cv::SVD::MODIFY_A | cv::SVD::FULL_UV);

    cv::Mat h = vt.row(8); // The last row of V^T
    cv::Mat H(3, 3, CV_32F);
    for (int i = 0; i < 9; ++i) {
        H.at<float>(i/3, i%3) = h.at<float>(0, i);
    }


    // Warp the second image using the homography matrix
    cv::Mat img2Transformed;
    cv::warpPerspective(img2, img2Transformed, H, img2.size());

    // Convert points to keypoints for visualization
    std::vector<cv::KeyPoint> keypoints1, keypoints2Transformed;
    for (const auto& pt : pts1) {
        keypoints1.emplace_back(pt, 1.f);
    }

    std::vector<cv::Point2f> pts2Transformed;
    for (const auto& pt : points2) {
        // Manually apply homography to points2 for keypoint generation (optional, as they may be off the visible area)
        std::vector<cv::Point2f> ptWrapper(1, pt), ptTransformed;
        cv::perspectiveTransform(ptWrapper, ptTransformed, H);
        keypoints2Transformed.emplace_back(ptTransformed[0], 1.f);
        pts2Transformed.push_back(ptTransformed[0]);
    }

    // Calculate the average error between the original and transformed points
    // find pts2transformed from keypoints2Transformed
    double error = calculateAverageError(pts1, pts2Transformed);
    std::cout << "Average error in custom method: " << error << std::endl;
    std::cout << "Homography matrix:" << std::endl;
    std::cout << H << std::endl;

    // Create dummy matches since every point is assumed to match its counterpart
    std::vector<cv::DMatch> matches;
    for (size_t i = 0; i < pts1.size(); ++i) {
        matches.emplace_back(i, i, 0);
    }

    // Draw matches
    cv::Mat imgMatches;
    cv::drawMatches(img1, keypoints1, img2Transformed, keypoints2Transformed, matches, imgMatches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
    return imgMatches;
}

int ImageTransformer::extractNumber(const string& filename) {
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
