using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using UnityEngine;


namespace VCDeskCalibrate {

public class VCCalibrator
{
    private IntPtr nativeObj;
    const string DLLFILE = "VCPlugin";
    [DllImport(DLLFILE)]
    private static extern IntPtr VCDeskCalibrate_Init(string matrixFilename);
    [DllImport(DLLFILE)]
    private static extern void VCDeskCalibrate_WarpPerspective(IntPtr calibrator, IntPtr image, int width, int height, 
                                    IntPtr result, int outputWidth, int outputHeight);
    [DllImport(DLLFILE)]
    private static extern int VCDeskCalibrate_FindContours(IntPtr calibrator, IntPtr image, int width, int height);
    [DllImport(DLLFILE)]
    private static extern bool VCDeskCalibrate_DrawContours(IntPtr calibrator, IntPtr image, int width, int height);
    [DllImport(DLLFILE)]
    private static extern int VCDeskCalibrate_GetHullContour(IntPtr calibrator, float[] outPoints);




    public VCCalibrator(string matrixFilename)
    {
        nativeObj = VCDeskCalibrate_Init(matrixFilename);
    }

    public Texture2D WarpPerspective(Texture2D frame, int width, int height)
    {
        Texture2D result = new Texture2D(width, height, TextureFormat.RGB24, false);
        var image = frame.GetPixels32();
        var result_pix = result.GetPixels32();
        GCHandle arrayHandle1 = GCHandle.Alloc(image, GCHandleType.Pinned);
        GCHandle arrayHandle2 = GCHandle.Alloc(result_pix, GCHandleType.Pinned);
        VCDeskCalibrate_WarpPerspective(nativeObj, arrayHandle1.AddrOfPinnedObject(), 
                                                                frame.width, frame.height, 
                                                                arrayHandle2.AddrOfPinnedObject(), width, height);
        arrayHandle1.Free();
        arrayHandle2.Free();
        result.SetPixels32(result_pix);
        result.Apply();
        return result;
    }

    public int FindContours(Texture2D frame)
    {
        var image = frame.GetPixels32();
        GCHandle arrayHandle = GCHandle.Alloc(image, GCHandleType.Pinned);
        int num = VCDeskCalibrate_FindContours(nativeObj, arrayHandle.AddrOfPinnedObject(), frame.width, frame.height);
        arrayHandle.Free();
        return num;
    }

    public bool DrawContours(Texture2D frame)
    {
        var image = frame.GetPixels32();
        GCHandle arrayHandle = GCHandle.Alloc(image, GCHandleType.Pinned);
        bool canTransform = VCDeskCalibrate_DrawContours(nativeObj, arrayHandle.AddrOfPinnedObject(), frame.width, frame.height);
        arrayHandle.Free();
        return canTransform;
    }

    public List<Vector2> GetHullContour()
    {
        float[] points = new float[1024];
        int pointsCount = VCDeskCalibrate_GetHullContour(nativeObj, points);
        List<Vector2> hullContour = new List<Vector2>();
        for (int i = 0; i < pointsCount; i += 2) {
            hullContour.Add(new Vector2(points[i], points[i + 1]));
        }
        return hullContour;
    }

}
}