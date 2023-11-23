using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using VCFaceDetection;
using VCImageProc;

public class test_img : MonoBehaviour
{
    private WebCamTexture webCamTexture;
    private Texture2D displayTexture;
    private Texture2D result;
    private bool isProcessing;

    void Start()
    {
        WebCamDevice camDevice = WebCamTexture.devices[0];
        webCamTexture = new WebCamTexture(camDevice.name);
        webCamTexture.Play();

        StartCoroutine(InitializeTexture());
    }

    IEnumerator InitializeTexture()
    {
        yield return new WaitUntil(() => webCamTexture.width > 100);
        displayTexture = new Texture2D(webCamTexture.width, webCamTexture.height, TextureFormat.RGBA32, false);
        StartCoroutine(ProcessWebCamTexture());
    }

    IEnumerator ProcessWebCamTexture()
    {
        isProcessing = true;
        while (isProcessing)
        {
            if (webCamTexture.didUpdateThisFrame && displayTexture != null)
            {
                var image = webCamTexture.GetPixels32();
                displayTexture.SetPixels32(image);
                displayTexture.Apply();
                result = VCImage.BlurMaskTexture(displayTexture, 21, 21);
                Debug.Log(VCImage.GetOriginalSkinColorFromTexture(displayTexture, result));
                GetComponent<Renderer>().material.mainTexture = result;
                // GetComponent<Renderer>().material.mainTexture = displayTexture;
            }
            yield return null;
        }
    }

    void OnDisable()
    {
        isProcessing = false;
    }
}