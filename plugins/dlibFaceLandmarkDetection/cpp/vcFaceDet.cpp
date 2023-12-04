#include "vcplugin_total.h"

FaceLandmarkDetector* VCFaceDetector_Init() {
    FaceLandmarkDetector* detector = new FaceLandmarkDetector();
    detector->detector = dlib::get_frontal_face_detector();
    return detector;
}

void VCFaceDetector_Dispose(FaceLandmarkDetector* detector) {
    delete detector;
}

bool VCFaceDetector_LoadObjectDetector(FaceLandmarkDetector* detector, const char* objectDetectorFilename) {
    std::string str(objectDetectorFilename);
    try {
        dlib::deserialize(str) >> detector->detector;
        return true;
    } catch (...) {
        return false;
    }
}

bool VCFaceDetector_LoadShapePredictor(FaceLandmarkDetector* detector, const char* shapePredictorFilename) {
    std::string str(shapePredictorFilename);
    try {
        dlib::deserialize(str) >> detector->sp;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading shape predictor: " << e.what() << std::endl;
        return false;
    }
}

void VCFaceDetector_SetImage(FaceLandmarkDetector* detector, unsigned char* byteArray, int width, int height, int bytesPerPixel, bool flip) {
    detector->faces.clear();
    detector->shapes.clear();
    if (detector->originImage.empty() || detector->originImage.cols != width || detector->originImage.rows != height) {
        detector->originImage.create(height, width, CV_8UC4);
    }
    memcpy(detector->originImage.data, byteArray, width * height * bytesPerPixel);
    cv::cvtColor(detector->originImage, detector->processedImage, cv::COLOR_RGBA2BGR);

    if (flip) {
        cv::flip(detector->processedImage, detector->processedImage, 0);
    }

    cv::resize(detector->processedImage, detector->processedImage, cv::Size(), detector->scale_factor, detector->scale_factor, cv::INTER_LINEAR);
    dlib::assign_image(detector->img, dlib::cv_image<dlib::bgr_pixel>(detector->processedImage));
}

int VCFaceDetector_Detect(FaceLandmarkDetector* detector, double adjust_threshold) {
    std::vector<dlib::rectangle> resized_faces = detector->detector(detector->img, adjust_threshold);
    for (const auto& rect : resized_faces) {
        dlib::rectangle original_rect(
            static_cast<long>(rect.left() / detector->scale_factor),
            static_cast<long>(rect.top() / detector->scale_factor),
            static_cast<long>(rect.right() / detector->scale_factor),
            static_cast<long>(rect.bottom() / detector->scale_factor));
        detector->faces.push_back(original_rect);
    }

    return detector->faces.size();
}

void VCFaceDetector_GetDetectResult(FaceLandmarkDetector* detector, double* result) {
    int i = 0;
    for (const auto& rect : detector->faces) {
        result[i++] = rect.left();
        result[i++] = rect.top();
        result[i++] = rect.width();
        result[i++] = rect.height();
    }
}

int VCFaceDetector_DetectLandmark(FaceLandmarkDetector* detector, double left, double top, double width, double height) {
    dlib::rectangle rect(left * detector->scale_factor, top * detector->scale_factor, 
                         (left + width) * detector->scale_factor, 
                         (top + height) * detector->scale_factor);
    dlib::full_object_detection scaled_shape = detector->sp(detector->img, rect);

    std::vector<dlib::point> original_points;
    for (unsigned long i = 0; i < scaled_shape.num_parts(); ++i) {
        dlib::point pt = scaled_shape.part(i);
        original_points.push_back(dlib::point(pt.x() / detector->scale_factor, pt.y() / detector->scale_factor));
    }
    dlib::full_object_detection original_shape(rect, original_points);
    detector->shapes.push_back(original_shape);

    return original_shape.num_parts();
}

void VCFaceDetector_GetDetectLandmarkResult(FaceLandmarkDetector* detector, double* result) {
    int i = 0;
    for (const auto& shape : detector->shapes) {
        for (unsigned long j = 0; j < shape.num_parts(); j++) {
            result[i++] = shape.part(j).x();
            result[i++] = shape.part(j).y();
        }
    }
}

long VCFaceDetector_ShapePredictorNumParts(FaceLandmarkDetector* detector) {
    if (detector->shapes.size() > 0) {
        return detector->shapes[0].num_parts();
    }
    return 0;
}

long VCFaceDetector_ShapePredictorNumFeatures(FaceLandmarkDetector* detector) {
    return 68;
}

bool VCFaceDetector_IsAllPartsInRect(FaceLandmarkDetector* detector) {
    for (const auto& shape : detector->shapes) {
        for (unsigned long i = 0; i < shape.num_parts(); i++) {
            if (!(shape.part(i).x() >= 0 && shape.part(i).y() >= 0 && shape.part(i).x() < detector->img.nc() && shape.part(i).y() < detector->img.nr())) {
                return false;
            }
        }
    }
    return true;
}

void VCFaceDetector_DrawDetectResult(FaceLandmarkDetector* detector, unsigned char* byteArray, int texWidth, int texHeight, int bytesPerPixel, bool flip, int r, int g, int b, int a, int thickness) {
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

void VCFaceDetector_DrawDetectLandmarkResult(FaceLandmarkDetector* detector, unsigned char* byteArray, int texWidth, int texHeight, int bytesPerPixel, bool flip, int r, int g, int b, int a, int thickness) {
    cv::Mat image(texHeight, texWidth, CV_8UC4, byteArray);
    if (flip) {
        cv::flip(image, image, 0);
    }

    // We'll define a lambda function to draw lines between points
    auto drawLine = [&](const dlib::point& pt1, const dlib::point& pt2, const cv::Scalar& color) {
        // Convert from 'point' (or whatever type is returned by shape.part(j)) to 'cv::Point'
        cv::Point p1(static_cast<int>(pt1.x()), static_cast<int>(pt1.y()));
        cv::Point p2(static_cast<int>(pt2.x()), static_cast<int>(pt2.y()));
        cv::line(image, p1, p2, color, thickness);
    };


    for (const auto& shape : detector->shapes) {
        // Draw lines for jawline
        for (unsigned long j = 0; j <= 16; j++) {
            if (j < 16) drawLine(shape.part(j), shape.part(j + 1), cv::Scalar(b, g, r, a));
        }

        // Draw lines for left eyebrow
        for (unsigned long j = 17; j < 21; j++) {
            drawLine(shape.part(j), shape.part(j + 1), cv::Scalar(b, g, r, a));
        }

        // Draw lines for right eyebrow
        for (unsigned long j = 22; j < 26; j++) {
            drawLine(shape.part(j), shape.part(j + 1), cv::Scalar(b, g, r, a));
        }

        // Draw lines for nose bridge
        for (unsigned long j = 27; j < 30; j++) {
            drawLine(shape.part(j), shape.part(j + 1), cv::Scalar(b, g, r, a));
        }

        // Draw lines for lower nose
        for (unsigned long j = 30; j < 35; j++) {
            if (j < 35) drawLine(shape.part(j), shape.part(j + 1), cv::Scalar(b, g, r, a));
        }
        // Close the loop for the lower nose
        drawLine(shape.part(30), shape.part(35), cv::Scalar(b, g, r, a));

        // Draw lines for left eye
        for (unsigned long j = 36; j < 41; j++) {
            drawLine(shape.part(j), shape.part(j + 1), cv::Scalar(99, 135, 227, a));
        }
        // Close the loop for the left eye
        drawLine(shape.part(36), shape.part(41), cv::Scalar(99, 135, 227, a));

        // Draw lines for right eye
        for (unsigned long j = 42; j < 47; j++) {
            drawLine(shape.part(j), shape.part(j + 1), cv::Scalar(203, 129, 180, a));
        }
        // Close the loop for the right eye
        drawLine(shape.part(42), shape.part(47), cv::Scalar(203, 129, 180, a));

        // Draw lines for outer lip
        for (unsigned long j = 48; j < 59; j++) {
            drawLine(shape.part(j), shape.part(j + 1), cv::Scalar(b, g, r, a));
        }
        // Close the loop for the outer lip
        drawLine(shape.part(48), shape.part(59), cv::Scalar(b, g, r, a));

        // Draw lines for inner lip
        for (unsigned long j = 60; j < 67; j++) {
            drawLine(shape.part(j), shape.part(j + 1), cv::Scalar(b, g, r, a));
        }
        // Close the loop for the inner lip
        drawLine(shape.part(60), shape.part(67), cv::Scalar(b, g, r, a));
    }

    if (flip) {
        cv::flip(image, image, 0);
    }
    memcpy(byteArray, image.data, texWidth * texHeight * bytesPerPixel);
}
