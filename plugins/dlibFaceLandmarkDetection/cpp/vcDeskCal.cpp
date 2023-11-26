#include "vcPlugin.h"

std::vector<cv::Point2f> _ProcessPoints(float* array, int length) {
    std::vector<cv::Point2f> points;
    points.reserve(length / 2);

    for (int i = 0; i < length; i += 2) {
        points.emplace_back(array[i], array[i + 1]);
    }
    return points;
}

extern "C" {

DLL_EXPORT DeskCalibrator* VCDeskCalibrate_Init(const char* filename) {
    DeskCalibrator* calibrator = new DeskCalibrator();
    std::string str(filename);
    calibrator->filePath =str;
    try {
        cv::FileStorage fs(str, cv::FileStorage::READ);
        fs["homography_matrix"] >> calibrator->warpMatrix;
        fs.release();
        return calibrator;
    } catch  (const std::exception& e) {
        std::cerr << "Error loading Desk Calibrator: " << e.what() << std::endl;
        return nullptr;
    }
}

DLL_EXPORT void VCDeskCalibrate_CalHomographyMatrix(DeskCalibrator* calibrator) {
    cv::Mat mat = cv::Mat::eye(3, 3, CV_32F);
    cv::FileStorage fs(calibrator->filePath, cv::FileStorage::WRITE);
    fs << "homography_matrix" << mat;
    fs.release();
};

DLL_EXPORT void VCDeskCalibrate_WarpPerspective(DeskCalibrator* calibrator, unsigned char* byteArray, int width, int height, unsigned char* outArray, int outWidth, int outHeight) {
    if (calibrator->frame.empty() || calibrator->frame.cols != width || calibrator->frame.rows != height) {
        calibrator->frame.create(height, width, CV_8UC4);
    }
    memcpy(calibrator->frame.data, byteArray, width * height * 4);
    cv::warpPerspective(calibrator->frame, calibrator->result, calibrator->warpMatrix, cv::Size(outWidth, outHeight));
    memcpy(outArray, calibrator->result.data, outWidth * outHeight * 4);
}

DLL_EXPORT int VCDeskCalibrate_FindContours(DeskCalibrator* calibrator, unsigned char* byteArray, int width, int height) {
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

DLL_EXPORT bool VCDeskCalibrate_DrawContours(DeskCalibrator* calibrator, unsigned char* byteArray, int width, int height) {
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

DLL_EXPORT int VCDeskCalibrate_GetHullContour(DeskCalibrator* calibrator, float* outPoints) {
    int index = 0;
    for (const auto& point : calibrator->hullContour) {
        outPoints[index++] = point.x;
        outPoints[index++] = point.y;
    }
    return calibrator->hullContour.size() * 2;
}

}
