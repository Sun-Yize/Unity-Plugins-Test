#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "FaceLandmarkDetector.h"

extern "C" {
    // Function prototypes exported from the dylib
    FaceLandmarkDetector* DlibFaceLandmarkDetector_Init();
    void DlibFaceLandmarkDetector_Dispose(FaceLandmarkDetector*);
    bool DlibFaceLandmarkDetector_LoadShapePredictor(FaceLandmarkDetector*, const char*);
    void DlibFaceLandmarkDetector_SetImage(FaceLandmarkDetector*, unsigned char*, int, int, int, bool);
    int DlibFaceLandmarkDetector_Detect(FaceLandmarkDetector*, double);
    int DlibFaceLandmarkDetector_DetectLandmark(FaceLandmarkDetector*, double, double, double, double);
}

int main() {
    // Initialize the detector
    FaceLandmarkDetector* detector = DlibFaceLandmarkDetector_Init();

    // Load shape predictor
    if(!DlibFaceLandmarkDetector_LoadShapePredictor(detector, "test_data/sp_human_face_68.dat")) {
        std::cerr << "Error loading shape predictor!" << std::endl;
        return -1;
    }

    // Load image using OpenCV
    cv::Mat image = cv::imread("test_data/test.jpg", cv::IMREAD_GRAYSCALE);
    cv::Mat image_rgb = cv::imread("test_data/test.jpg");
    if (image.empty()) {
        std::cerr << "Error reading the image!" << std::endl;
        return -1;
    }

    // Set the image to the detector
    DlibFaceLandmarkDetector_SetImage(detector, image.data, image.cols, image.rows, 1, false);

    // Detect faces
    for(int i=0; i<100; i++) {
        int numFaces = DlibFaceLandmarkDetector_Detect(detector, -0.5f);
        std::cout << "Detected " << numFaces << " faces." << std::endl;
    }


    // // Detect landmarks for each face (for simplicity, only the first face here)
    // if (numFaces > 0) {
    //     double faceResult[4];
    //     DlibFaceLandmarkDetector_GetDetectResult(detector, faceResult);
    //     int numLandmarks = DlibFaceLandmarkDetector_DetectLandmark(detector, faceResult[0], faceResult[1], faceResult[2], faceResult[3]);
    //     // std::cout << "Detected " << numLandmarks << " landmarks for the first face." << std::endl;
    // }
    // // DlibFaceLandmarkDetector_DrawDetectLandmarkResult(detector, image_rgb.data, image_rgb.cols, image_rgb.rows, 3, false, 0, 255, 0, 255);
    // DlibFaceLandmarkDetector_DrawDetectResult(detector, image_rgb.data, image_rgb.cols, image_rgb.rows, 3, false, 0, 255, 0, 255, 1);

    // cv::imwrite("test_data/output_image.jpg", image_rgb);

    // Dispose the detector
    DlibFaceLandmarkDetector_Dispose(detector);

    return 0;
}
