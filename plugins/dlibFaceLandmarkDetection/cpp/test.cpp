#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/opencv.h>
#include <opencv2/opencv.hpp>

int main() {
    // 初始化dlib的人脸检测器
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();

    // 打开默认摄像头
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Unable to connect to camera" << std::endl;
        return 1;
    }

    // 降低摄像头分辨率
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    cv::Mat temp;

    int counter = 0;
    int saveEveryNFrames = 10;  // 例如，每10帧保存一次

    while (cap.read(temp)) {
        // 将 OpenCV 图像转换为 dlib 图像
        dlib::cv_image<dlib::bgr_pixel> cimg(temp);

        // 检测人脸
        std::vector<dlib::rectangle> faces = detector(cimg);

        // 在图像上标记人脸
        for (auto face : faces) {
            cv::rectangle(temp, cv::Point(face.left(), face.top()), cv::Point(face.right(), face.bottom()), cv::Scalar(0, 0, 255), 2);
        }

        // 每N帧保存一次图像
        if (counter % saveEveryNFrames == 0) {
            cv::imwrite("frame_" + std::to_string(counter) + ".jpg", temp);
        }
        counter++;

        // 这里我们没有添加额外的延迟，因为处理和保存图像本身就需要一些时间。
    }

    return 0;
}
