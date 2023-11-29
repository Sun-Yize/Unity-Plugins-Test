#include "vcplugin_total.h"

void VCImageProc_GetMeanColor(unsigned char* image, int imageWidth, int imageHeight, 
                          unsigned char* mask, int maskWidth, int maskHeight, 
                          unsigned char* outColor) {
    cv::Mat imageMat(imageHeight, imageWidth, CV_8UC4, image);
    cv::Mat maskMat(maskHeight, maskWidth, CV_8UC4, mask);

    cv::cvtColor(maskMat, maskMat, cv::COLOR_RGBA2GRAY);
    cv::Scalar color = cv::mean(imageMat, maskMat);

    outColor[0] = static_cast<unsigned char>(color[2]); // R
    outColor[1] = static_cast<unsigned char>(color[1]); // G
    outColor[2] = static_cast<unsigned char>(color[0]); // B
    outColor[3] = 255; // Alpha
}

void VCImageProc_BlurMask(unsigned char* byteArray, int width, int height, int kernelWidth, int kernelHeight) {
    cv::Mat image(height, width, CV_8UC4, byteArray);
    cv::Mat blurredImage;
    cv::GaussianBlur(image, blurredImage, cv::Size(kernelWidth, kernelHeight), 0);
    memcpy(byteArray, blurredImage.data, width * height * 4);
}
