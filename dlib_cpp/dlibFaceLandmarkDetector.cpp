#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/opencv.h>

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
    } catch (const std::exception& e) {
        std::cerr << "Error loading shape predictor: " << e.what() << std::endl;
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

void SetPixel(dlib::array2d<dlib::rgb_pixel>& image, long x, long y, dlib::rgb_pixel color) {
    if (x >= 0 && x < image.nc() && y >= 0 && y < image.nr()) {
        image[y][x] = color;
    }
}

void DrawRectangle(dlib::array2d<dlib::rgb_pixel>& image, dlib::rectangle rect, dlib::rgb_pixel color, int thickness) {
    long left = rect.left();
    long top = rect.top();
    long right = rect.right();
    long bottom = rect.bottom();

    // Horizontal lines
    for (long x = left; x <= right; x++) {
        for (int t = -thickness / 2; t <= thickness / 2; t++) {
            SetPixel(image, x, top + t, color);    // Top line
            SetPixel(image, x, bottom + t, color); // Bottom line
        }
    }
    
    // Vertical lines
    for (long y = top; y <= bottom; y++) {
        for (int t = -thickness / 2; t <= thickness / 2; t++) {
            SetPixel(image, left + t, y, color);    // Left line
            SetPixel(image, right + t, y, color);   // Right line
        }
    }
}

void DrawCircle(dlib::array2d<dlib::rgb_pixel>& image, dlib::point center, int radius, dlib::rgb_pixel color) {
    long x0 = center.x();
    long y0 = center.y();
    int r2 = radius * radius;

    for (long y = y0 - radius; y <= y0 + radius; y++) {
        for (long x = x0 - radius; x <= x0 + radius; x++) {
            if ((x - x0) * (x - x0) + (y - y0) * (y - y0) <= r2) {
                SetPixel(image, x, y, color);
            }
        }
    }
}

void DrawPoints(dlib::array2d<dlib::rgb_pixel>& image, const dlib::full_object_detection& landmarks, int start, int end, dlib::rgb_pixel color) {
    for (int i = start; i < end; ++i) {
        dlib::point p1(landmarks.part(i).x(), landmarks.part(i).y());
        dlib::point p2(landmarks.part(i+1).x(), landmarks.part(i+1).y());
        dlib::draw_line(image, p1, p2, color);
    }
}

void DlibFaceLandmarkDetector_DrawDetectResult(FaceLandmarkDetector* detector, unsigned char* byteArray, int texWidth, int texHeight, int bytesPerPixel, bool flip, int r, int g, int b, int a, int thickness) {
    dlib::array2d<dlib::rgb_pixel> dlib_img(texHeight, texWidth);
    memcpy(&dlib_img[0][0], byteArray, texWidth * texHeight * sizeof(dlib::rgb_pixel));

    dlib::rgb_pixel color = dlib::rgb_pixel(r, g, b);

    for (const auto& face : detector->faces) {
        DrawRectangle(dlib_img, face, color, thickness);
    }
    
    if (flip) {
        dlib::flip_image_left_right(dlib_img, dlib_img);
    }

    // Convert back to byteArray if necessary.
    memcpy(byteArray, &dlib_img[0][0], texWidth * texHeight * sizeof(dlib::rgb_pixel));
}

void DlibFaceLandmarkDetector_DrawDetectLandmarkResult(FaceLandmarkDetector* detector, unsigned char* byteArray, int texWidth, int texHeight, int bytesPerPixel, bool flip, int r, int g, int b, int a) {
    dlib::array2d<dlib::rgb_pixel> dlib_img(texHeight, texWidth);
    memcpy(&dlib_img[0][0], byteArray, texWidth * texHeight * sizeof(dlib::rgb_pixel));

    dlib::rgb_pixel color = dlib::rgb_pixel(r, g, b);

    for (const auto& shape : detector->shapes) {
        // Draw the facial landmarks with lines instead of circles
        DrawPoints(dlib_img, shape, 0, 16, color);       // Jaw line
        DrawPoints(dlib_img, shape, 17, 21, color);      // Left eyebrow
        DrawPoints(dlib_img, shape, 22, 26, color);      // Right eyebrow
        DrawPoints(dlib_img, shape, 27, 30, color);      // Nose bridge
        DrawPoints(dlib_img, shape, 30, 35, color);      // Lower nose
        DrawPoints(dlib_img, shape, 36, 41, color);      // Left eye
        DrawPoints(dlib_img, shape, 42, 47, color);      // Right eye
        DrawPoints(dlib_img, shape, 48, 59, color);      // Outer lip
        DrawPoints(dlib_img, shape, 60, 67, color);      // Inner lip
    }

    if (flip) {
        dlib::flip_image_left_right(dlib_img, dlib_img);
    }

    // Convert back to byteArray if necessary.
    memcpy(byteArray, &dlib_img[0][0], texWidth * texHeight * sizeof(dlib::rgb_pixel));
}
}
