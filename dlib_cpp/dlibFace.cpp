#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/opencv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

struct FaceLandmarkDetector {
    dlib::frontal_face_detector detector;
    dlib::shape_predictor sp;
    dlib::array2d<unsigned char> img;
    std::vector<dlib::rectangle> faces;
    std::vector<dlib::full_object_detection> shapes;
};

extern "C" {

FaceLandmarkDetector* DlibFaceLandmarkDetector_Init() {
    FaceLandmarkDetector* detector = new FaceLandmarkDetector();
    detector->detector = dlib::get_frontal_face_detector(); // Default face detector
    return detector;
}

void DlibFaceLandmarkDetector_Dispose(FaceLandmarkDetector* detector) {
    delete detector;
}

bool DlibFaceLandmarkDetector_LoadObjectDetector(FaceLandmarkDetector* detector, const wchar_t* objectDetectorFilename) {
    std::wstring ws(objectDetectorFilename);
    std::string str(ws.begin(), ws.end());

    try {
        dlib::deserialize(str) >> detector->detector;
        return true;
    } catch (...) {
        return false;
    }
}

bool DlibFaceLandmarkDetector_LoadShapePredictor(FaceLandmarkDetector* detector, const wchar_t* shapePredictorFilename) {
    std::wstring ws(shapePredictorFilename);
    std::string str(ws.begin(), ws.end());

    try {
        dlib::deserialize(str) >> detector->sp;
        return true;
    } catch (...) {
        return false;
    }
}

void DlibFaceLandmarkDetector_SetImage(FaceLandmarkDetector* detector, unsigned char* byteArray, int texWidth, int texHeight, int bytesPerPixel, bool flip) {
    cv::Mat image(texHeight, texWidth, CV_8UC1, byteArray, texWidth * bytesPerPixel);
    dlib::assign_image(detector->img, dlib::cv_image<unsigned char>(image));
    if (flip) {
        dlib::flip_image_left_right(detector->img, detector->img);
    }
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
    cv::Mat img_rgb(texHeight, texWidth, CV_8UC3, byteArray);
    dlib::array2d<dlib::rgb_pixel> dlib_img;
    dlib::assign_image(dlib_img, dlib::cv_image<dlib::rgb_pixel>(img_rgb));
    
    for (const auto& face : detector->faces) {
        dlib::draw_rectangle(dlib_img, face, dlib::rgb_pixel(r, g, b), thickness);
    }
    
    if (flip) {
        dlib::flip_image_left_right(dlib_img, dlib_img);
    }
}

void DlibFaceLandmarkDetector_DrawDetectLandmarkResult(FaceLandmarkDetector* detector, unsigned char* byteArray, int texWidth, int texHeight, int bytesPerPixel, bool flip, int r, int g, int b, int a) {
    cv::Mat img_rgb(texHeight, texWidth, CV_8UC3, byteArray);
    dlib::array2d<dlib::rgb_pixel> dlib_img;
    dlib::assign_image(dlib_img, dlib::cv_image<dlib::rgb_pixel>(img_rgb));

    for (const auto& shape : detector->shapes) {
        for (unsigned long i = 0; i < shape.num_parts(); i++) {
            cv::circle(img_rgb, cv::Point(shape.part(i).x(), shape.part(i).y()), 2, cv::Scalar(b, g, r), -1);
        }
    }

    // If you need to convert the modified cv::Mat back to dlib::array2d, do so here.
    dlib::assign_image(dlib_img, dlib::cv_image<dlib::rgb_pixel>(img_rgb));
}

}
