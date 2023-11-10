#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "FaceLandmarkDetector.h"

extern "C" {
    // Function prototypes exported from the dylib
    FaceLandmarkDetector* DlibFaceLandmarkDetector_Init();
    void DlibFaceLandmarkDetector_Dispose(FaceLandmarkDetector*);
    bool DlibFaceLandmarkDetector_LoadShapePredictor(FaceLandmarkDetector*, const wchar_t*);
    void DlibFaceLandmarkDetector_SetImage(FaceLandmarkDetector*, unsigned char*, int, int, int, bool);
    int DlibFaceLandmarkDetector_Detect(FaceLandmarkDetector*, double);
    int DlibFaceLandmarkDetector_DetectLandmark(FaceLandmarkDetector*, double, double, double, double);
}

int main() {
    // Initialize the detector
    FaceLandmarkDetector* detector = DlibFaceLandmarkDetector_Init();

    // Load shape predictor
    if(!DlibFaceLandmarkDetector_LoadShapePredictor(detector, L"sp_human_face_68.dat")) {
        std::cerr << "Error loading shape predictor!" << std::endl;
        return -1;
    }

    // Load image using OpenCV
    cv::Mat image = cv::imread("test.jpg", cv::IMREAD_GRAYSCALE);
    cv::Mat image_rgb = cv::imread("test.jpg");
    if (image.empty()) {
        std::cerr << "Error reading the image!" << std::endl;
        return -1;
    }

    // Set the image to the detector
    DlibFaceLandmarkDetector_SetImage(detector, image.data, image.cols, image.rows, 1, false);

    // Detect faces
    int numFaces = DlibFaceLandmarkDetector_Detect(detector, 1.0);
    std::cout << "Detected " << numFaces << " faces." << std::endl;

    // Detect landmarks for each face (for simplicity, only the first face here)
    if (numFaces > 0) {
        double faceResult[4];
        DlibFaceLandmarkDetector_GetDetectResult(detector, faceResult);
        int numLandmarks = DlibFaceLandmarkDetector_DetectLandmark(detector, faceResult[0], faceResult[1], faceResult[2], faceResult[3]);
        std::cout << "Detected " << numLandmarks << " landmarks for the first face." << std::endl;
    }

    DlibFaceLandmarkDetector_DrawDetectLandmarkResult(detector, image_rgb.data, image_rgb.cols, image_rgb.rows, 3, false, 0, 255, 0, 255);

    cv::imwrite("output_image.jpg", image_rgb);

    // Dispose the detector
    DlibFaceLandmarkDetector_Dispose(detector);

    return 0;
}