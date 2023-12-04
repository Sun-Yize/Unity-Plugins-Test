using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using VCFaceDetection;

public class test_opencvsharp : MonoBehaviour
{
    VCFaceDetector det = new VCFaceDetector("/Users/charlie/Documents/vscode_projects/build_test/unity_test/Assets/test_data/sp_human_face_68.dat");
    private WebCamTexture webCamTexture;
    private Texture2D displayTexture;
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
                det.SetImage<Color32>(image, webCamTexture.width, webCamTexture.height, 4, true);
                var faces = det.Detect(-0.5f);
                if (faces.Count > 0)
                {
                    var landmarks = det.DetectLandmarkArray(faces[0]);
                    det.DrawDetectResult(image, webCamTexture.width, webCamTexture.height, 4, true, 255, 255, 255, 255, 2);
                    det.DrawDetectLandmarkResult(image, webCamTexture.width, webCamTexture.height, 4, true, 238, 226, 125, 0, 2);
                    displayTexture.SetPixels32(image);
                    displayTexture.Apply();
                    GetComponent<Renderer>().material.mainTexture = displayTexture;
                }
            }
            yield return null;
        }
    }

    void OnDisable()
    {
        isProcessing = false;
    }
}