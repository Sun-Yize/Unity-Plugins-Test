#ifndef VCPLUGIN_H
#define VCPLUGIN_H

#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/opencv.h>
#include <opencv2/opencv.hpp>

struct FaceLandmarkDetector {
    dlib::frontal_face_detector detector;
    dlib::shape_predictor sp;
    dlib::array2d<unsigned char> img;
    std::vector<dlib::rectangle> faces;
    std::vector<dlib::full_object_detection> shapes;
    cv::Mat originImage;
    cv::Mat processedImage;
    double scale_factor=0.5;
};

struct DeskCalibrator {
    cv::Mat frame;
    cv::Mat result;
    cv::Mat warpMatrix;
    std::string filePath;
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Point> hullContour;
};

DeskCalibrator* VCDeskCalibrate_Init(const char* filename);
void VCDeskCalibrate_CalHomographyMatrix(DeskCalibrator* calibrator);
void VCDeskCalibrate_WarpPerspective(DeskCalibrator* calibrator, unsigned char* byteArray, int width, int height, unsigned char* outArray, int outWidth, int outHeight);
int VCDeskCalibrate_FindContours(DeskCalibrator* calibrator, unsigned char* byteArray, int width, int height);
bool VCDeskCalibrate_DrawContours(DeskCalibrator* calibrator, unsigned char* byteArray, int width, int height);
int VCDeskCalibrate_GetHullContour(DeskCalibrator* calibrator, float* outPoints);

FaceLandmarkDetector* VCFaceDetector_Init();
void VCFaceDetector_Dispose(FaceLandmarkDetector* detector);
bool VCFaceDetector_LoadObjectDetector(FaceLandmarkDetector* detector, const char* objectDetectorFilename);
bool VCFaceDetector_LoadShapePredictor(FaceLandmarkDetector* detector, const char* shapePredictorFilename);
void VCFaceDetector_SetImage(FaceLandmarkDetector* detector, unsigned char* byteArray, int width, int height, int bytesPerPixel, bool flip);
int VCFaceDetector_Detect(FaceLandmarkDetector* detector, double adjust_threshold);
void VCFaceDetector_GetDetectResult(FaceLandmarkDetector* detector, double* result);
int VCFaceDetector_DetectLandmark(FaceLandmarkDetector* detector, double left, double top, double width, double height);
void VCFaceDetector_GetDetectLandmarkResult(FaceLandmarkDetector* detector, double* result);
long VCFaceDetector_ShapePredictorNumParts(FaceLandmarkDetector* detector);
long VCFaceDetector_ShapePredictorNumFeatures(FaceLandmarkDetector* detector);
bool VCFaceDetector_IsAllPartsInRect(FaceLandmarkDetector* detector);
void VCFaceDetector_DrawDetectResult(FaceLandmarkDetector* detector, unsigned char* byteArray, int texWidth, int texHeight, int bytesPerPixel, bool flip, int r, int g, int b, int a, int thickness);
void VCFaceDetector_DrawDetectLandmarkResult(FaceLandmarkDetector* detector, unsigned char* byteArray, int texWidth, int texHeight, int bytesPerPixel, bool flip, int r, int g, int b, int a, int thickness);

void VCImageProc_GetMeanColor(unsigned char* image, int imageWidth, int imageHeight, unsigned char* mask, int maskWidth, int maskHeight, unsigned char* outColor);
void VCImageProc_BlurMask(unsigned char* byteArray, int width, int height, int kernelWidth, int kernelHeight);

#endif // VCPLUGIN_H
