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
    cv::Mat bgrImage;
};

extern "C" {

FaceLandmarkDetector* DlibFaceLandmarkDetector_Init() {
    FaceLandmarkDetector* detector = new FaceLandmarkDetector();
    detector->detector = dlib::get_frontal_face_detector();
    return detector;
}

void DlibFaceLandmarkDetector_Dispose(FaceLandmarkDetector* detector) {
    delete detector;
}

bool DlibFaceLandmarkDetector_LoadObjectDetector(FaceLandmarkDetector* detector, const char* objectDetectorFilename) {
    std::string str(objectDetectorFilename);
    try {
        dlib::deserialize(str) >> detector->detector;
        return true;
    } catch (...) {
        return false;
    }
}

bool DlibFaceLandmarkDetector_LoadShapePredictor(FaceLandmarkDetector* detector, const char* shapePredictorFilename) {
    std::string str(shapePredictorFilename);
    try {
        dlib::deserialize(str) >> detector->sp;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading shape predictor: " << e.what() << std::endl;
        return false;
    }
}

void DlibFaceLandmarkDetector_SetImage(FaceLandmarkDetector* detector, unsigned char* byteArray, int width, int height, int bytesPerPixel, bool flip) {
    detector->faces.clear();
    detector->shapes.clear();
    if (detector->originImage.empty() || detector->originImage.cols != width || detector->originImage.rows != height) {
        detector->originImage.create(height, width, CV_8UC4);
    }
    memcpy(detector->originImage.data, byteArray, width * height * bytesPerPixel);
    cv::cvtColor(detector->originImage, detector->bgrImage, cv::COLOR_RGBA2BGR);
    if (flip) {
        cv::flip(detector->bgrImage, detector->bgrImage, 0);
    }
    dlib::assign_image(detector->img, dlib::cv_image<dlib::bgr_pixel>(detector->bgrImage));
}

int DlibFaceLandmarkDetector_Detect(FaceLandmarkDetector* detector, double adjust_threshold) {
    detector->faces = detector->detector(detector->img, adjust_threshold);
    return detector->faces.size();
}

void DlibFaceLandmarkDetector_GetDetectResult(FaceLandmarkDetector* detector, double* result) {
    int i = 0;
    for (const auto& rect : detector->faces) {
        result[i++] = rect.left();
        result[i++] = rect.top();
        result[i++] = rect.width();
        result[i++] = rect.height();
    }
}

int DlibFaceLandmarkDetector_DetectLandmark(FaceLandmarkDetector* detector, double left, double top, double width, double height) {
    dlib::rectangle rect(left, top, left + width, top + height);
    dlib::full_object_detection shape = detector->sp(detector->img, rect);
    detector->shapes.push_back(shape);
    return shape.num_parts();
}

void DlibFaceLandmarkDetector_GetDetectLandmarkResult(FaceLandmarkDetector* detector, double* result) {
    int i = 0;
    for (const auto& shape : detector->shapes) {
        for (unsigned long j = 0; j < shape.num_parts(); j++) {
            result[i++] = shape.part(j).x();
            result[i++] = shape.part(j).y();
        }
    }
}

long DlibFaceLandmarkDetector_ShapePredictorNumParts(FaceLandmarkDetector* detector) {
    if (detector->shapes.size() > 0) {
        return detector->shapes[0].num_parts();
    }
    return 0;
}

long DlibFaceLandmarkDetector_ShapePredictorNumFeatures(FaceLandmarkDetector* detector) {
    return 68;
}

bool DlibFaceLandmarkDetector_IsAllPartsInRect(FaceLandmarkDetector* detector) {
    for (const auto& shape : detector->shapes) {
        for (unsigned long i = 0; i < shape.num_parts(); i++) {
            if (!(shape.part(i).x() >= 0 && shape.part(i).y() >= 0 && shape.part(i).x() < detector->img.nc() && shape.part(i).y() < detector->img.nr())) {
                return false;
            }
        }
    }
    return true;
}

void DlibFaceLandmarkDetector_DrawDetectResult(FaceLandmarkDetector* detector, unsigned char* byteArray, int texWidth, int texHeight, int bytesPerPixel, bool flip, int r, int g, int b, int a, int thickness) {
    cv::Mat image(texHeight, texWidth, CV_8UC4, byteArray);
    if (flip) {
        cv::flip(image, image, 0);
    }
    for (const auto& rect : detector->faces) {
        cv::Rect cvRect(rect.left(), rect.top(), rect.width(), rect.height());
        cv::rectangle(image, cvRect, cv::Scalar(b, g, r, a), thickness);
    }
    if (flip) {
        cv::flip(image, image, 0);
    }
    memcpy(byteArray, image.data, texWidth * texHeight * bytesPerPixel);
}

void DlibFaceLandmarkDetector_DrawDetectLandmarkResult(FaceLandmarkDetector* detector, unsigned char* byteArray, int texWidth, int texHeight, int bytesPerPixel, bool flip, int r, int g, int b, int a, int thickness) {
    cv::Mat image(texHeight, texWidth, CV_8UC4, byteArray);
    if (flip) {
        cv::flip(image, image, 0);
    }

    for (const auto& shape : detector->shapes) {
        for (unsigned long j = 0; j < shape.num_parts(); j++) {
            cv::Point p(shape.part(j).x(), shape.part(j).y());
            cv::circle(image, p, thickness, cv::Scalar(b, g, r, a), cv::FILLED);
        }
    }
    if (flip) {
        cv::flip(image, image, 0);
    }
    memcpy(byteArray, image.data, texWidth * texHeight * bytesPerPixel);
}
}
