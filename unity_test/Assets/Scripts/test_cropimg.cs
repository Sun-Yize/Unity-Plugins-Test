using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using VCFaceDetection;
using VCImageProc;
using VCDeskCalibrate;
using System.IO;


public class test_cropimg : MonoBehaviour
{
    private WebCamTexture webCamTexture;
    private Texture2D displayTexture;
    private Texture2D result;
    private bool isProcessing;
    VCCalibrator calibrator = new VCCalibrator("example.json");

    // Start is called before the first frame update
    void Start()
    {
        // byte[] fileData = File.ReadAllBytes("/Users/charlie/Documents/vscode_projects/build_test/unity_test/Assets/test_data/test2.jpg");
        // Texture2D texture = new Texture2D(2, 2);
        // texture.LoadImage(fileData);
        // // var output = calibrator.WarpPerspective(texture, 900, 900);
        // var num = calibrator.FindContours(texture);
        // var res = calibrator.DrawContours(texture);
        // GetComponent<Renderer>().material.mainTexture = texture;

        // int w = texture.width;
        // int h = texture.height;
        //  int src_width = w;
        //  int src_height = h;

        //  // add offset to show the full transformed image (quadrilateral)
        // var woffset = w * 0.8f;
        // var hoffset = h * 0.8f;

        // List<Vector2> destination_point = new List<Vector2>();
        
        // destination_point.Add(new Vector2(woffset, hoffset));
        // destination_point.Add(new Vector2(woffset, hoffset + src_height));
        // destination_point.Add(new Vector2(woffset + src_width, hoffset));
        // destination_point.Add(new Vector2(woffset + src_width, hoffset + src_height));


        // var scr_point = calibrator.GetHullContour();
        // Debug.Log(scr_point.Count);
        // calibrator.CalWarpMatrix(scr_point, destination_point);
        // output = calibrator.WarpPerspective(texture, 900, 900);
        // GetComponent<Renderer>().material.mainTexture = output;


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
                var output = calibrator.WebCamTexToTex2D(webCamTexture);
                // Debug.Log(output.width);
                // Debug.Log(output.height);
                // output = calibrator.EnhanceImage(output);
                // var output = calibrator.WarpPerspective(webCamTexture, 600, 600);
                // 
                var num = calibrator.FindContours(output);
                var res = calibrator.DrawContours(output);
                GetComponent<Renderer>().material.mainTexture = output;
            }
            yield return null;
        }
    }

    void OnDisable()
    {
        isProcessing = false;
    }

    // Update is called once per frame
    void Update()
    {

    }
}
