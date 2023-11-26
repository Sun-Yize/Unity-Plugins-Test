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
