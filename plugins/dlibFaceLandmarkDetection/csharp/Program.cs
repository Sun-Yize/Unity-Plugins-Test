using System;
using System.Runtime.InteropServices;
using OpenCVSharp;


class Program
{
    // 导入你的动态链接库中的函数
    const string dlibPath = "./build/lib/libFaceLandmarkDetector.dylib";

    [DllImport(dlibPath)]
    public static extern IntPtr DlibFaceLandmarkDetector_Init();
    [DllImport(dlibPath)]
    public static extern void DlibFaceLandmarkDetector_Dispose(IntPtr detector);

    [DllImport(dlibPath, CharSet = CharSet.Unicode)]
    public static extern bool DlibFaceLandmarkDetector_LoadShapePredictor(IntPtr detector, [MarshalAs(UnmanagedType.LPWStr)] string shapePredictorFilename);

    [DllImport(dlibPath)]
    public static extern void DlibFaceLandmarkDetector_SetImage(IntPtr detector, IntPtr byteArray, int texWidth, int texHeight, int bytesPerPixel, bool flip);

    [DllImport(dlibPath)]
    public static extern int DlibFaceLandmarkDetector_Detect(IntPtr detector, double adjust_threshold);

    [DllImport(dlibPath)]
    public static extern void DlibFaceLandmarkDetector_DrawDetectLandmarkResult(IntPtr detector, IntPtr byteArray, int texWidth, int texHeight, int bytesPerPixel, bool flip, int r, int g, int b, int a);

    unsafe static void Main(string[] args)
    {
        // 初始化检测器
        IntPtr detector = DlibFaceLandmarkDetector_Init();

        // 加载形状预测器
        bool isLoaded = DlibFaceLandmarkDetector_LoadShapePredictor(detector, "sspp__hhuummaann__ffaaccee__6688..ddaatt");

        // bool isLoaded = DlibFaceLandmarkDetector_LoadShapePredictor(detector, "shape_predictor_68_face_landmarks.dat");
        if (!isLoaded)
        {
            Console.WriteLine("Error loading shape predictor");
            return;
        }

        // 加载图像
        Mat image = Cv2.ImRead("test.jpg", ImreadModes.Color);

        byte[] imgData = image.ToBytes(".png");
        IntPtr imgDataPtr = Marshal.AllocHGlobal(imgData.Length);
        Marshal.Copy(imgData, 0, imgDataPtr, imgData.Length);

        // 设置检测器的图像
        DlibFaceLandmarkDetector_SetImage(detector, imgDataPtr, image.Width, image.Height, 3, false);

        // 检测
        int numFaces = DlibFaceLandmarkDetector_Detect(detector, 0);
        Console.WriteLine($"Detected {numFaces} faces");

        byte[] outputImgData = new byte[image.Width * image.Height * image.Channels()];
        IntPtr outputImgDataPtr = Marshal.AllocHGlobal(outputImgData.Length);

        // 绘制检测结果
        DlibFaceLandmarkDetector_DrawDetectLandmarkResult(detector, outputImgDataPtr, image.Width, image.Height, 3, false, 255, 0, 0, 255);

        Marshal.Copy(outputImgDataPtr, outputImgData, 0, outputImgData.Length);
        Mat outputImage = Mat.FromImageData(outputImgData, ImreadModes.Color);
        Cv2.ImWrite("output_image.jpg", outputImage);

        // 释放资源
        DlibFaceLandmarkDetector_Dispose(detector);
        Marshal.FreeHGlobal(imgDataPtr);
        Marshal.FreeHGlobal(outputImgDataPtr);
    }
}
