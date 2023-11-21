// FaceLandmarkDetector.h
#ifndef FACELANDMARKDETECTOR_H
#define FACELANDMARKDETECTOR_H

#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/opencv.h>
#include <vector>

struct FaceLandmarkDetector;

extern "C" {

FaceLandmarkDetector* DlibFaceLandmarkDetector_Init();

void DlibFaceLandmarkDetector_Dispose(FaceLandmarkDetector* detector);

bool DlibFaceLandmarkDetector_LoadObjectDetector(FaceLandmarkDetector* detector, const char* objectDetectorFilename);

bool DlibFaceLandmarkDetector_LoadShapePredictor(FaceLandmarkDetector* detector, const char* shapePredictorFilename);

void DlibFaceLandmarkDetector_SetImage(FaceLandmarkDetector* detector, unsigned char* byteArray, int texWidth, int texHeight, int bytesPerPixel, bool flip);

int DlibFaceLandmarkDetector_Detect(FaceLandmarkDetector* detector, double adjust_threshold);

void DlibFaceLandmarkDetector_GetDetectResult(FaceLandmarkDetector* detector, double* result);

int DlibFaceLandmarkDetector_DetectLandmark(FaceLandmarkDetector* detector, double left, double top, double width, double height);

void DlibFaceLandmarkDetector_GetDetectLandmarkResult(FaceLandmarkDetector* detector, double* result);

long DlibFaceLandmarkDetector_ShapePredictorNumParts(FaceLandmarkDetector* detector);

long DlibFaceLandmarkDetector_ShapePredictorNumFeatures(FaceLandmarkDetector* detector);

bool DlibFaceLandmarkDetector_IsAllPartsInRect(FaceLandmarkDetector* detector);

void DlibFaceLandmarkDetector_DrawDetectResult(FaceLandmarkDetector* detector, unsigned char* byteArray, int texWidth, int texHeight, int bytesPerPixel, bool flip, int r, int g, int b, int a, int thickness);

void DlibFaceLandmarkDetector_DrawDetectLandmarkResult(FaceLandmarkDetector* detector, unsigned char* byteArray, int texWidth, int texHeight, int bytesPerPixel, bool flip, int r, int g, int b, int a);

}

struct FaceLandmarkDetector {
    dlib::frontal_face_detector detector;
    dlib::shape_predictor sp;
    dlib::array2d<unsigned char> img;
    std::vector<dlib::rectangle> faces;
    std::vector<dlib::full_object_detection> shapes;
};

#endif // FACELANDMARKDETECTOR_H