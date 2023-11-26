using System;
using System.Runtime.InteropServices;
using UnityEngine;


namespace VCImageProc {

public class VCImage
{
    const string DLLFILE = "VCPlugin";
    [DllImport(DLLFILE)]
    private static extern void VCImageProc_GetMeanColor(IntPtr image, int imageWidth, int imageHeight, 
                                                    IntPtr mask, int maskWidth, int maskHeight, 
                                                    byte[] outColor);
    [DllImport(DLLFILE)]
    private static extern void VCImageProc_BlurMask(IntPtr image, int width, int height, 
                                    int kernelWidth, int kernelHeight);
    public static Color32 GetOriginalSkinColorFromTexture(Texture2D originImg, Texture2D originMask)
    {
        var image = originImg.GetPixels32();
        var mask = originMask.GetPixels32();

        GCHandle arrayHandle1 = GCHandle.Alloc(image, GCHandleType.Pinned);
        GCHandle arrayHandle2 = GCHandle.Alloc(mask, GCHandleType.Pinned);

        byte[] color = new byte[4];

        VCImageProc_GetMeanColor(arrayHandle1.AddrOfPinnedObject(), originImg.width, originImg.height, 
                             arrayHandle2.AddrOfPinnedObject(), originMask.width, originMask.height, 
                             color);
        arrayHandle1.Free();
        arrayHandle2.Free();
        return new Color32(color[0], color[1], color[2], color[3]);
    }

    public static Texture2D BlurMaskTexture(Texture2D mask, int kernelWidth, int kernelHeight)
    {
        var image = mask.GetPixels32();
        GCHandle arrayHandle = GCHandle.Alloc(image, GCHandleType.Pinned);
        VCImageProc_BlurMask(arrayHandle.AddrOfPinnedObject(), mask.width, mask.height, kernelWidth, kernelHeight);
        arrayHandle.Free();
        Texture2D result = new Texture2D(mask.width, mask.height, TextureFormat.RGB24, false);
        result.SetPixels32(image);
        result.Apply();
        return result;
    }
}
}