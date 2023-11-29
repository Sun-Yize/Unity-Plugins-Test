import cv2
import numpy as np

# 读取图像
image = cv2.imread('test_data/test.jpg')  # 替换为您的图像路径

srcPoints = np.array([[0, 0], [image.shape[1] - 1, 0], [image.shape[1] - 1, image.shape[0] - 1], [0, image.shape[0] - 1]], dtype=np.float32)

# 目标点是源点在输出图像中的新位置
dstPoints = np.array([[0, 0], [300, 0], [300, 300], [0, 300]], dtype=np.float32)  # 例如，创建一个300x300像素的新视角

# 计算透视变换矩阵
matrix = cv2.getPerspectiveTransform(srcPoints, dstPoints)
matrix[0][0] = 1
matrix[1][1] = 1
print(matrix)
# 应用透视变换
warpedImage = cv2.warpPerspective(image, matrix, (300, 300))

# 显示原始图像和变换后的图像
cv2.imshow('Original Image', image)
cv2.imshow('Warped Image', warpedImage)
cv2.waitKey(0)
cv2.destroyAllWindows()
