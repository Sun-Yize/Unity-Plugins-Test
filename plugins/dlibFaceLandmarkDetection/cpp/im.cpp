// main.cpp
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

// 假设你的dylib提供了以下的接口
extern "C" {
    struct FaceLandmarkDetector;
    FaceLandmarkDetector* DlibFaceLandmarkDetector_Init();
    void DlibFaceLandmarkDetector_Dispose(FaceLandmarkDetector* detector);
    bool DlibFaceLandmarkDetector_LoadShapePredictor(FaceLandmarkDetector* detector, const wchar_t* shapePredictorFilename);
    void DlibFaceLandmarkDetector_SetImage(FaceLandmarkDetector* detector, unsigned char* byteArray, int texWidth, int texHeight, int bytesPerPixel, bool flip);
    int DlibFaceLandmarkDetector_Detect(FaceLandmarkDetector* detector, double adjust_threshold);
    int DlibFaceLandmarkDetector_DetectLandmark(FaceLandmarkDetector* detector, double left, double top, double width, double height);
    void DlibFaceLandmarkDetector_DrawDetectLandmarkResult(FaceLandmarkDetector* detector, unsigned char* byteArray, int texWidth, int texHeight, int bytesPerPixel, bool flip, int r, int g, int b, int a);
}

int main() {
    // 使用OpenCV加载图片
    cv::Mat image = cv::imread("test.jpg", cv::IMREAD_COLOR);
    if (image.empty()) {
        std::cerr << "Could not read the image." << std::endl;
        return 1;
    }

    // 转换图片为灰度
    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

    // 初始化检测器
    FaceLandmarkDetector* detector = DlibFaceLandmarkDetector_Init();
    if (!detector) {
        std::cerr << "Failed to initialize the detector." << std::endl;
        return 1;
    }

    // 加载形状预测器模型
    wchar_t shapePredictorFilename[] = L"sp_human_face_68.dat";
    if (!DlibFaceLandmarkDetector_LoadShapePredictor(detector, shapePredictorFilename)) {
        std::cerr << "Failed to load shape predictor." << std::endl;
        return 1;
    }

    // 设置图像数据
    DlibFaceLandmarkDetector_SetImage(detector, gray.data, gray.cols, gray.rows, gray.channels(), false);

    // 进行人脸检测
    int num_faces = DlibFaceLandmarkDetector_Detect(detector, 0);
    std::cout << "Detected faces: " << num_faces << std::endl;

    // 假设我们只处理一张脸进行演示
    if (num_faces > 0) {
        // 这里简化处理，我们假设整张图片只有一张脸
        double left = 0;
        double top = 0;
        double width = gray.cols;
        double height = gray.rows;
        int num_parts = DlibFaceLandmarkDetector_DetectLandmark(detector, left, top, width, height);
        std::cout << "Detected landmarks: " << num_parts << std::endl;
        
        // 将检测结果绘制在彩色图上
        DlibFaceLandmarkDetector_DrawDetectLandmarkResult(detector, image.data, image.cols, image.rows, image.channels(), false, 255, 0, 0, 255);
    }

    // 显示结果
    cv::namedWindow("Face Landmark Detection", cv::WINDOW_AUTOSIZE);
    cv::imshow("Face Landmark Detection", image);
    cv::waitKey(0);

    // 清理资源
    DlibFaceLandmarkDetector_Dispose(detector);
    
    return 0;
}
