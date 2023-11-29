#if defined(_WIN32) || defined(_WIN64)
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __attribute__((visibility("default")))
#endif

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

extern "C" {
    DLL_EXPORT DeskCalibrator* VCDeskCalibrate_Init(const char* filename);
    DLL_EXPORT void VCDeskCalibrate_CalWarpMatrix(DeskCalibrator* calibrator, float* srcPointsArray, float* desPointsArray);
    DLL_EXPORT void VCDeskCalibrate_WarpPerspective(DeskCalibrator* calibrator, unsigned char* byteArray, int width, int height, unsigned char* outArray, int outWidth, int outHeight);
    DLL_EXPORT int VCDeskCalibrate_FindContours(DeskCalibrator* calibrator, unsigned char* byteArray, int width, int height);
    DLL_EXPORT bool VCDeskCalibrate_DrawContours(DeskCalibrator* calibrator, unsigned char* byteArray, int width, int height);
    DLL_EXPORT int VCDeskCalibrate_GetHullContour(DeskCalibrator* calibrator, float* outPoints);
    DLL_EXPORT void VCDeskCalibrate_EnhanceImage(DeskCalibrator* calibrator, unsigned char* byteArray, int width, int height);
    DLL_EXPORT FaceLandmarkDetector* VCFaceDetector_Init();
    DLL_EXPORT void VCFaceDetector_Dispose(FaceLandmarkDetector* detector);
    DLL_EXPORT bool VCFaceDetector_LoadObjectDetector(FaceLandmarkDetector* detector, const char* objectDetectorFilename);
    DLL_EXPORT bool VCFaceDetector_LoadShapePredictor(FaceLandmarkDetector* detector, const char* shapePredictorFilename);
    DLL_EXPORT void VCFaceDetector_SetImage(FaceLandmarkDetector* detector, unsigned char* byteArray, int width, int height, int bytesPerPixel, bool flip);
    DLL_EXPORT int VCFaceDetector_Detect(FaceLandmarkDetector* detector, double adjust_threshold);
    DLL_EXPORT void VCFaceDetector_GetDetectResult(FaceLandmarkDetector* detector, double* result);
    DLL_EXPORT int VCFaceDetector_DetectLandmark(FaceLandmarkDetector* detector, double left, double top, double width, double height);
    DLL_EXPORT void VCFaceDetector_GetDetectLandmarkResult(FaceLandmarkDetector* detector, double* result);
    DLL_EXPORT long VCFaceDetector_ShapePredictorNumParts(FaceLandmarkDetector* detector);
    DLL_EXPORT long VCFaceDetector_ShapePredictorNumFeatures(FaceLandmarkDetector* detector);
    DLL_EXPORT bool VCFaceDetector_IsAllPartsInRect(FaceLandmarkDetector* detector);
    DLL_EXPORT void VCFaceDetector_DrawDetectResult(FaceLandmarkDetector* detector, unsigned char* byteArray, int texWidth, int texHeight, int bytesPerPixel, bool flip, int r, int g, int b, int a, int thickness);
    DLL_EXPORT void VCFaceDetector_DrawDetectLandmarkResult(FaceLandmarkDetector* detector, unsigned char* byteArray, int texWidth, int texHeight, int bytesPerPixel, bool flip, int r, int g, int b, int a, int thickness);
    DLL_EXPORT void VCImageProc_GetMeanColor(unsigned char* image, int imageWidth, int imageHeight, unsigned char* mask, int maskWidth, int maskHeight, unsigned char* outColor);
    DLL_EXPORT void VCImageProc_BlurMask(unsigned char* byteArray, int width, int height, int kernelWidth, int kernelHeight);
}
