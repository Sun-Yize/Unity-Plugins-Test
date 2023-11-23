using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;


namespace VCFaceDetection {

public class VCFaceDetector
{
    private IntPtr nativeObj;
    const string DLLFILE = "VCPlugin";

    [DllImport(DLLFILE)]
    public static extern IntPtr VCFaceDetector_Init();
    [DllImport(DLLFILE)]
    public static extern void VCFaceDetector_Dispose(IntPtr detector);

    [DllImport(DLLFILE)]
    public static extern bool VCFaceDetector_LoadShapePredictor(IntPtr detector, string shapePredictorFilename);

    [DllImport(DLLFILE)]
    public static extern void VCFaceDetector_SetImage(IntPtr detector, IntPtr byteArray, int texWidth, int texHeight, int bytesPerPixel, bool flip);

    [DllImport(DLLFILE)]
    public static extern int VCFaceDetector_Detect(IntPtr detector, double adjust_threshold);
    [DllImport(DLLFILE)]
    private static extern void VCFaceDetector_GetDetectResult(IntPtr self, double[] result);
    [DllImport(DLLFILE)]
    public static extern int VCFaceDetector_DetectLandmark(IntPtr detector, double left, double top, double width, double height);
    [DllImport(DLLFILE)]
    private static extern void VCFaceDetector_GetDetectLandmarkResult(IntPtr self, double[] result);
    [DllImport(DLLFILE)]
    public static extern void VCFaceDetector_DrawDetectLandmarkResult(IntPtr detector, IntPtr byteArray, int texWidth, int texHeight, int bytesPerPixel, bool flip, int r, int g, int b, int a, int thickness);
    [DllImport(DLLFILE)]
    public static extern void VCFaceDetector_DrawDetectResult(IntPtr detector, IntPtr byteArray, int texWidth, int texHeight, int bytesPerPixel, bool flip, int r, int g, int b, int a, int thickness);

    public VCFaceDetector(string shapePredictorFilePath)
    {
        nativeObj = VCFaceDetector_Init();
        if (!VCFaceDetector_LoadShapePredictor(nativeObj, shapePredictorFilePath))
        {
            Debug.LogError("Failed to load " + shapePredictorFilePath);
        }
    }

    public void SetImage<T>(T[] array, int width, int height, int bytesPerPixel, bool flip) where T : struct
    {
        GCHandle arrayHandle = GCHandle.Alloc(array, GCHandleType.Pinned);
        VCFaceDetector_SetImage(nativeObj, arrayHandle.AddrOfPinnedObject(), width, height, bytesPerPixel, flip);
        arrayHandle.Free();
    }

    public List<Rect> Detect(double adjust_threshold)
    {
        List<Rect> rects = new List<Rect>();

        int detectCount = VCFaceDetector_Detect(nativeObj, adjust_threshold);
        if (detectCount > 0)
        {
            double[] result = new double[detectCount * 6];
            VCFaceDetector_GetDetectResult(nativeObj, result);

            for (int i = 0; i < detectCount; i++)
            {
                rects.Add(new Rect((float)result[i * 6 + 0], (float)result[i * 6 + 1], (float)result[i * 6 + 2], (float)result[i * 6 + 3]));
            }
        }

        return rects;
    }

    public double[] DetectLandmarkArray(Rect rect)
    {
        int detectCount = VCFaceDetector_DetectLandmark(nativeObj, rect.xMin, rect.yMin, rect.width, rect.height);
        if (detectCount > 0)
        {
            double[] result = new double[detectCount * 2];
            VCFaceDetector_GetDetectLandmarkResult(nativeObj, result);
            return result;
        }
        return new double[0];
    }

    public void DrawDetectLandmarkResult<T>(T[] array, int width, int height, int bytesPerPixel, bool flip, int r, int g, int b, int a, int thickness) where T : struct
    {
        GCHandle arrayHandle = GCHandle.Alloc(array, GCHandleType.Pinned);
        VCFaceDetector_DrawDetectLandmarkResult(nativeObj, arrayHandle.AddrOfPinnedObject(), width, height, bytesPerPixel, flip, r, g, b, a, thickness);
        arrayHandle.Free();
    }

    public void DrawDetectResult<T>(T[] array, int width, int height, int bytesPerPixel, bool flip, int r, int g, int b, int a, int thickness) where T : struct
    {
        GCHandle arrayHandle = GCHandle.Alloc(array, GCHandleType.Pinned);
        VCFaceDetector_DrawDetectResult(nativeObj, arrayHandle.AddrOfPinnedObject(), width, height, bytesPerPixel, flip, r, g, b, a, thickness);
        arrayHandle.Free();
    }
}
}
