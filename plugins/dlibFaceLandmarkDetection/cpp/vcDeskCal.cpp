#include "vcplugin_total.h"

std::vector<cv::Point2f> _ProcessPoints(float* array, int length) {
    std::vector<cv::Point2f> points;
    points.reserve(length / 2);

    for (int i = 0; i < length; i += 2) {
        points.emplace_back(array[i], array[i + 1]);
    }
    return points;
}

DeskCalibrator* VCDeskCalibrate_Init(const char* filename) {
    DeskCalibrator* calibrator = new DeskCalibrator();
    std::string str(filename);
    calibrator->filePath =str;
    try {
        cv::FileStorage fs(str, cv::FileStorage::READ);
        fs["warpMatrix"] >> calibrator->warpMatrix;
        fs.release();
        return calibrator;
    } catch  (const std::exception& e) {
        std::cerr << "Error loading Desk Calibrator: " << e.what() << std::endl;
        return nullptr;
    }
}

void VCDeskCalibrate_CalWarpMatrix(DeskCalibrator* calibrator, float* srcPointsArray, float* desPointsArray) {
    std::vector<cv::Point2f> srcPoints;
    std::vector<cv::Point2f> desPoints;
    for (int i = 0; i < 8; i += 2) {
        srcPoints.push_back(cv::Point2f(srcPointsArray[i], srcPointsArray[i + 1]));
        desPoints.push_back(cv::Point2f(desPointsArray[i], desPointsArray[i + 1]));
    }
    if (srcPoints.size() == 4 && desPoints.size() == 4) {
        calibrator->warpMatrix = cv::getPerspectiveTransform(srcPoints, desPoints);
        cv::FileStorage fs(calibrator->filePath, cv::FileStorage::WRITE);
        fs << "warpMatrix" << calibrator->warpMatrix;
        fs.release();
    }
};

void VCDeskCalibrate_WarpPerspective(DeskCalibrator* calibrator, unsigned char* byteArray, int width, int height, unsigned char* outArray, int outWidth, int outHeight) {
    if (calibrator->frame.empty() || calibrator->frame.cols != width || calibrator->frame.rows != height) {
        calibrator->frame.create(height, width, CV_8UC4);
    }
    if (calibrator->result.empty() || calibrator->result.cols != outWidth || calibrator->result.rows != outHeight) {
        calibrator->result.create(outHeight, outWidth, CV_8UC4);
    }
    memcpy(calibrator->frame.data, byteArray, width * height * 4);
    cv::warpPerspective(calibrator->frame, calibrator->result, calibrator->warpMatrix, cv::Size(outWidth, outHeight));
    memcpy(outArray, calibrator->result.data, outWidth * outHeight * 4);
}

int VCDeskCalibrate_FindContours(DeskCalibrator* calibrator, unsigned char* byteArray, int width, int height) {
    if (calibrator->frame.empty() || calibrator->frame.cols != width || calibrator->frame.rows != height) {
        calibrator->frame.create(height, width, CV_8UC4);
    }
    memcpy(calibrator->frame.data, byteArray, width * height * 4);
    cv::Mat imgGray, imgGrayBlur, cannyEdges;
    cv::cvtColor(calibrator->frame, imgGray, cv::COLOR_BGRA2GRAY);
    cv::GaussianBlur(imgGray, imgGrayBlur, cv::Size(5, 5), 0);
    cv::Canny(imgGrayBlur, cannyEdges, 10, 50);
    cv::findContours(cannyEdges, calibrator->contours, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
    return calibrator->contours.size();
}

bool VCDeskCalibrate_DrawContours(DeskCalibrator* calibrator, unsigned char* byteArray, int width, int height) {
    if (calibrator->frame.empty() || calibrator->frame.cols != width || calibrator->frame.rows != height) {
        calibrator->frame.create(height, width, CV_8UC4);
    }
    memcpy(calibrator->frame.data, byteArray, width * height * 4);
    std::vector<std::vector<cv::Point>> contours;

    double maxArea = 0;
    std::vector<cv::Point> keyPoints;

    for (const auto& contour : calibrator->contours) {
        double area = cv::contourArea(contour);
        if (area > maxArea) {
            maxArea = area;
            keyPoints = contour;
        }
    }

    std::vector<cv::Point> hull;
    cv::convexHull(keyPoints, hull);

    cv::approxPolyDP(hull, calibrator->hullContour, cv::arcLength(hull, true) * 0.05, true);

    if (calibrator->hullContour.size() == 4 && cv::contourArea(calibrator->hullContour) > 30000) {
        cv::drawContours(calibrator->frame, std::vector<std::vector<cv::Point>>{calibrator->hullContour}, -1, cv::Scalar(85, 190, 255), 4);
        memcpy(byteArray, calibrator->frame.data, width * height * 4);
        return true;
    } else {
        return false;
    }
}

int VCDeskCalibrate_GetHullContour(DeskCalibrator* calibrator, float* outPoints) {
    int index = 0;
    for (const auto& point : calibrator->hullContour) {
        outPoints[index++] = point.x;
        outPoints[index++] = point.y;
    }
    return calibrator->hullContour.size() * 2;
}

void VCDeskCalibrate_EnhanceImage(DeskCalibrator* calibrator, unsigned char* byteArray, int width, int height) {
    if (calibrator->frame.empty() || calibrator->frame.cols != width || calibrator->frame.rows != height) {
        calibrator->frame.create(height, width, CV_8UC4);
    }
    memcpy(calibrator->frame.data, byteArray, width * height * 4);
    cv::cvtColor(calibrator->frame, calibrator->frame, cv::COLOR_BGR2YUV);

    std::vector<cv::Mat> channels;
    cv::split(calibrator->frame, channels);

    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2.0, cv::Size(4, 4));
    cv::Mat imgDest;
    clahe->apply(channels[0], imgDest);

    imgDest.copyTo(channels[0]);
    cv::merge(channels, calibrator->frame);
    cv::cvtColor(calibrator->frame, calibrator->frame, cv::COLOR_YUV2BGR);

    memcpy(byteArray, calibrator->frame.data, width * height * 4);
}

int main() {
    DeskCalibrator* calibrator = VCDeskCalibrate_Init("example.json");
    if (!calibrator) {
        std::cerr << "Failed to initialize DeskCalibrator." << std::endl;
        return -1;
    }

    cv::Mat testImage = cv::imread("test_data/test1.jpg");
    if (testImage.empty()) {
        std::cerr << "Failed to load test image." << std::endl;
        return -1;
    }

    unsigned char* imageData = testImage.data;
    int width = testImage.cols;
    int height = testImage.rows;
    unsigned char* outArray = new unsigned char[width * height * 4];
    VCDeskCalibrate_WarpPerspective(calibrator, imageData, width, height, outArray, width, height);
    
    int contoursCount = VCDeskCalibrate_FindContours(calibrator, imageData, width, height);
    std::cout << "Contours found: " << contoursCount << std::endl;
    bool drawn = VCDeskCalibrate_DrawContours(calibrator, imageData, width, height);
    std::cout << "Hull points count: " << cv::contourArea(calibrator->hullContour) << std::endl;
    std::cout << "Contours drawn: " << (drawn ? "True" : "False") << std::endl;

    float* hullPoints = new float[calibrator->hullContour.size() * 2];
    int hullPointsCount = VCDeskCalibrate_GetHullContour(calibrator, hullPoints);
    std::cout << "Hull points count: " << hullPointsCount << std::endl;

    VCDeskCalibrate_CalWarpMatrix(calibrator, hullPoints, hullPoints);
    VCDeskCalibrate_EnhanceImage(calibrator, imageData, width, height);

    delete[] outArray;
    delete[] hullPoints;
    delete calibrator;

    return 0;
}