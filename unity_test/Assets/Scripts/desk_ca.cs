// using System.Collections.Generic;
// using UnityEngine;
// using UnityEngine.UI;
// using System.IO;
// using System;
// // using Newtonsoft.Json.Linq;
// using System.Collections;

// public class desk_ca : MonoBehaviour
// {
//     public GameObject menu;
//     public GameObject deskCaliPage;
//     public RawImage backgroundImage;
//     public Button confirmButton;
//     public Button transformButton;
//     public Button plusButton;
//     public Button minusButton;
//     public Button cropButton;
//     public Button resetButton;
//     public Button saveButton;
//     public Image rectImage;
//     public RectTransform rectImageTransform;
//     public Text guideText;
//     public Toggle rotateToggle;
//     public Toggle enhanceToggle;
//     public GameObject rotateTog;
//     public Image corrLTPoint;
//     public Image corrLBPoint;
//     public Image corrRTPoint;
//     public Image corrRBPoint;

//     private Texture2D cameraTexture;
//     private bool camAvailable;
//     private Texture2D output;
//     private int w = 640;
//     private int h = 480;
//     private int tempW = 1500;
//     private int tempH = 1125;
//     private int deltaW = 16;
//     private int deltaH = 12;
//     private int bgW = 880;
//     private int bgH = 660;

//     private List<Point[]> approx_hull;
//     private List<Point2f> destination_point;
//     private List<Point2f> src_point;
//     private Mat homography_mat;
//     private Mat inverse_homography_mat;
//     private Mat result;

//     private bool clickConfirm = false; 
//   //  private bool clickTransform = false;
//     private bool calculated = false;
//     private bool canTransform = false;
//     private bool cropped = false;
//     private bool saved = false;
//    // private bool readFromJson = false;
//     private bool rotate = true;
//     private bool enhance = false;

//     private float bgX;
//     private float bgY;
//     private float rectX;
//     private float rectY;

//     private float ratio = 0.8f;
//     private float woffset;
//     private float hoffset;

//     private float cropRectUpX;
//     private float cropRectUpY;
//     private float cropRectW;
//     private float cropRectH;

//     private float scalingFactorX;
//     private float scalingFactorY;
//     private WebCamInterface.WebCamera _webCamera;

//     private Mat tempMat=new Mat();
    
//     //TODO: update to Mediapipe v0.10.0
//     void OnEnable()
//     {
//         WebCamInterface.Initialize();
//         var camResolution = Settings.GetValue<int>("camera_resolution");
//         _webCamera = WebCamInterface.GetCamera(Settings.GetValue<int>("desk_demo_camera"));
//         if (_webCamera == null) return;

//         StartCoroutine(_webCamera.Start(camResolution, delegate (Texture2D texture)
//         {
//             StartCoroutine(_webCamera.Capture());
//             cameraTexture = texture;
//         }));

//         ResetAll();

//         var imageTransform = backgroundImage.GetComponent<RectTransform>();
//         imageTransform.sizeDelta = new Vector2(bgW, bgH);
//       //  imageTransform.sizeDelta = new Vector2(w, h);
//         Vector3[] v = new Vector3[4];
//         //imageTransform.GetLocalCorners(v);
//         scalingFactorX = 1 / imageTransform.lossyScale[0];
//         scalingFactorY = 1 / imageTransform.lossyScale[1];
//         imageTransform.GetWorldCorners(v);
//         bgX = v[1].x;
//         bgY = v[1].y;   //ratio 4:3
//         confirmButton.onClick.AddListener(TaskOnClickConfirm);
//         transformButton.onClick.AddListener(TaskOnClickTransform);
//         cropButton.onClick.AddListener(TaskOnClickCrop);
//         plusButton.onClick.AddListener(ClickEnlarge);
//         minusButton.onClick.AddListener(ClickShrink);
//         resetButton.onClick.AddListener(ResetAll);
//         saveButton.onClick.AddListener(SaveToJSON);
        
//         rotateToggle.onValueChanged.AddListener(RotateToggleChange);
//         enhanceToggle.onValueChanged.AddListener(EnhanceToggleChange);

//         ResetButtons();

//         rotateToggle.isOn = true;
//         enhanceToggle.isOn = false;
//         rectImage.enabled = false;
//         corrLTPoint.enabled = false;
//         corrLBPoint.enabled = false;
//         corrRTPoint.enabled = false;
//         corrRBPoint.enabled = false;
//         //ReadFromJson();
//     }

//     private void Update()
//     {
//         if (_webCamera == null || cameraTexture == null)
//             return;
        
//         Color32[] frame = cameraTexture.GetPixels32();

//         if (enhance);

//         if(saved)
//         {
//             guideText.text = "Successfully saved. Page will be closed in 1 second.";
//             // VCImage.LoadWarp(frame, w, h);
//             output.SetPixels32(frame);
//             output.Apply();
//             backgroundImage.texture = output;
//         }
//         else if (calculated)
//         {
//                         Mat result = new Mat();

//             if (cropped) // show cropped area
//             {
//                 Cv2.WarpPerspective(frame, result, homography_mat, new OpenCvSharp.Size(w, h));
//                 guideText.text = "Click the tick button to save the calibration result.";
//             }
//             else // for users to crop
//             {
//                 Cv2.WarpPerspective(frame, result, homography_mat, new OpenCvSharp.Size(tempW, tempH));
//                 guideText.text = "Select an area you want to display.";
//             }
//             output = OpenCvSharp.Unity.MatToTexture(result, output);
//             backgroundImage.texture = output;
            
//             result.Release();
//         }

//         if (enhance) frame = EnhanceImg(frame);

//         if (saved) //transformed and cropped
//         {
            
//             Mat result = new Mat();
//             Mat _homoMat = ReadFromJson();
//             /*Mat _homoMat = new Mat(3, 3, MatType.CV_32F);
//             if (!readFromJson)
//             {
//                 _homoMat = ReadFromJson();
//                 readFromJson = true;
//                 UnityEngine.Debug.Log(_homoMat.Dump());
//             }*/
//             Cv2.WarpPerspective(frame, result, _homoMat, new OpenCvSharp.Size(w, h));
//             output = OpenCvSharp.Unity.MatToTexture(result, output);
//             backgroundImage.texture = output;
            
//             _homoMat.Release();
//             result.Release();
//         }
//         else if (calculated) // transformed
//         {
//             Mat result = new Mat();

//             if (cropped) // show cropped area
//             {
//                 Cv2.WarpPerspective(frame, result, homography_mat, new OpenCvSharp.Size(w, h));
//                 guideText.text = "Click the tick button to save the calibration result.";
//             }
//             else // for users to crop
//             {
//                 Cv2.WarpPerspective(frame, result, homography_mat, new OpenCvSharp.Size(tempW, tempH));
//                 guideText.text = "Select an area you want to display.";
//             }
//             output = OpenCvSharp.Unity.MatToTexture(result, output);
//             backgroundImage.texture = output;
            
//             result.Release();
//         }
//         else if (clickConfirm)
//         {
//             output = OpenCvSharp.Unity.MatToTexture(tempMat, output);
//             backgroundImage.texture = output;
//         }
//         else // detect paper
//         {
//             // convert gray
//             Mat imgGray = new Mat();
//             Cv2.CvtColor(frame, imgGray, ColorConversionCodes.BGR2GRAY);
//             // 5*5 filter
//             Mat imgGrayBlur = new Mat();
//             Cv2.GaussianBlur(imgGray, imgGrayBlur, new Size(5, 5), 0);

//             // apply threshold
//             //Mat mask = new Mat();
//             //Cv2.Threshold(imgGrayBlur, mask, 200.0, 255.0, ThresholdTypes.Otsu);

//             // canny edge detector
//             Mat cannyEdges = new Mat();
//             Cv2.Canny(imgGrayBlur, cannyEdges, 10, 50);

//             // detect largest contour
//             Point[][] contours;
//             HierarchyIndex[] hierarchy;
//             Cv2.FindContours(cannyEdges, out contours, out hierarchy, RetrievalModes.Tree, ContourApproximationModes.ApproxNone);

//             //find contours with the largest contour area
//             List<Point[]> keyPoints = new List<Point[]>();
//             double maxArea = 0;

//             if (contours.Length > 0)
//             {
//                 foreach (Point[] contour in contours)
//                 {
//                     double area = Cv2.ContourArea(contour);
//                     if (area > maxArea)
//                     {
//                         maxArea = area;
//                         // add contours to key points
//                         keyPoints.Clear();
//                         keyPoints.Add(contour);
//                     }
//                 }

//                 Point[] hull = Cv2.ConvexHull(keyPoints[0]);
//                 Point[] hullContour = Cv2.ApproxPolyDP(hull, Cv2.ArcLength(hull, true) * 0.05, true);

//                 approx_hull = new List<Point[]>();
//                 approx_hull.Add(hullContour);

//                 Point[][] contours_draw = approx_hull.ToArray();
//                 double drawContourArea = Cv2.ContourArea(contours_draw[0]);

//                 if (contours_draw[0].Length == 4 && drawContourArea > 30000)
//                 {
//                     Cv2.DrawContours(frame, contours_draw, -1, new Scalar(85, 190, 255), 4);
//                     canTransform = true;

//                     guideText.text = "Paper detected. Click Transform when you find a good place to fix the camera.";
//                 }
//                 else
//                 {
//                     guideText.text = "Please place a piece of paper properly. If nothing detected, click next to proceed manual correction.";
//                     canTransform = false;
//                 }
//             }
            
//             output = OpenCvSharp.Unity.MatToTexture(frame, output);       
//             backgroundImage.texture = output;
            
//             frame.CopyTo(tempMat);
            
//             imgGray.Release();
//             imgGrayBlur.Release();
//             cannyEdges.Release();
//         }
        
//         output = null;
//         Resources.UnloadUnusedAssets();
//     }

//     private void ResetAll()
//     {
//         guideText.text = "reset";
//         backgroundImage.texture = cameraTexture;
//         clickConfirm = false;
//       //  clickTransform = false;
//         calculated = false;
//         canTransform = false;
//         cropped = false;
//         saved = false;

//         ResetButtons();
        
//         rectImage.enabled = false;
//         corrLTPoint.enabled = false;
//         corrLBPoint.enabled = false;
//         corrRTPoint.enabled = false;
//         corrRBPoint.enabled = false;
        
//         rotate = true;
//         enhance = false;
//         rotateToggle.isOn = true;
//         enhanceToggle.isOn = false;
//         rotateTog.SetActive(true);
//         rotateTog.GetComponent<Toggle>().enabled=true;
//         RecordRectPos();
//     }

//     private void EnableButton(Button btn, bool isDisplay)
//     {
//         btn.enabled = isDisplay; // enable or disable button function
//         btn.image.enabled = isDisplay; // enable or disable button icon
//     }

//     private void ResetButtons()
//     {
//         EnableButton(confirmButton, true);
//         EnableButton(transformButton, false);
//         EnableButton(plusButton, false);
//         EnableButton(minusButton, false);
//         EnableButton(cropButton, false);
//         EnableButton(saveButton, false);
//     }

//     private Mat EnhanceImg(Mat inputImg)
//     {
//         Mat imgYUV = new Mat();
//         Cv2.CvtColor(inputImg, imgYUV, ColorConversionCodes.BGR2YUV);
//         Mat[] channels = new Mat[3];
//         Cv2.Split(imgYUV, out channels);

//         var clahe = Cv2.CreateCLAHE(2.0, new Size(4, 4));
//         Mat imgDest = new Mat();
//         clahe.Apply(channels[0], imgDest);

//         imgDest.CopyTo(channels[0]);
//         Cv2.Merge(channels, imgYUV);
//         Cv2.CvtColor(imgYUV, inputImg, ColorConversionCodes.YUV2BGR);

//         imgYUV.Release();
//         imgDest.Release();
        
//         return inputImg;
//     }

//     private void RecordRectPos()
//     {
//         Vector3[] v = new Vector3[4];
//         rectImageTransform.GetWorldCorners(v);
//         //rectImageTransform.GetLocalCorners(v);
//         rectX = v[1].x;
//         rectY = v[1].y;
//     }

//     public void RotateToggleChange(bool newValue)
//     {
//         rotate = newValue;
//     }

//     public void EnhanceToggleChange(bool newValue)
//     {
//         enhance = newValue;
//     }
    
//     public void TaskOnClickConfirm()
//     {
//         // allow users to select rectangle corners
//         // four colors -> four corners. 
//         // workflow: confirm -> show four corners (according to detection results if any), transform -> transform based on corner position
//         clickConfirm = true;
        
//         guideText.text = "Move four points to correct corner positions.";

//         PlaceCorrPoints();
        
//         EnableButton(confirmButton,false);
//         rotateTog.SetActive(false);
//         rotateTog.GetComponent<Toggle>().enabled=false;
        
//         EnableButton(transformButton,true);
//         corrLTPoint.enabled = true;
//         corrLBPoint.enabled = true;
//         corrRTPoint.enabled = true;
//         corrRBPoint.enabled = true;
//     }
    
//     public void TaskOnClickTransform()
//     {
//         //clickTransform = true;
//         EnableButton(transformButton,false);
//         corrLTPoint.enabled = false;
//         corrLBPoint.enabled = false;
//         corrRTPoint.enabled = false;
//         corrRBPoint.enabled = false;

//         CalculateTransformationMatrix();
        
//         EnableButton(plusButton,true);
//         EnableButton(minusButton,true);
//         EnableButton(cropButton,true);
//         rectImage.enabled = true;
        
//         rectImageTransform = rectImage.rectTransform;
//         rectImageTransform.sizeDelta = new Vector2(bgW / 2, bgH / 2);
        
//         RecordRectPos();
//     }

//     public void TaskOnClickCrop()
//     {
//         EnableButton(plusButton,false);
//         EnableButton(minusButton,false);
//         EnableButton(cropButton,false);
//         rectImage.enabled = false;
//         // recalculate transformation matrix
//         RecalculateTransform();
//         cropped = true;

//         EnableButton(saveButton,true);
        
//         RecordRectPos();
//     }

//     public void ClickEnlarge()
//     {
//         int currX = (int)rectImageTransform.sizeDelta.x;
//         int currY = (int)rectImageTransform.sizeDelta.y;
//         rectImageTransform.sizeDelta = new Vector2(currX + deltaW, currY + deltaH);

//         RecordRectPos();
//     }

//     public void ClickShrink()
//     {
//         int currX = (int)rectImageTransform.sizeDelta.x;
//         int currY = (int)rectImageTransform.sizeDelta.y;
//         rectImageTransform.sizeDelta = new Vector2(currX - deltaW, currY - deltaH);

//         RecordRectPos();
//     }

//     private List<Vector2> SortCornerPoints()
//     {
//         // Sort according to position
//         int maxsum = (int)(approx_hull[0][0].x + approx_hull[0][0].y);
//         int minsum = (int)(approx_hull[0][0].x + approx_hull[0][0].y);
//         int leftTopIndex = 0, rightBottomIndex = 0;

//         for (int i = 1; i < 4; i++)
//         {
//             int sum = (int)(approx_hull[0][i].x + approx_hull[0][i].y);
//             if (sum < minsum) { leftTopIndex = i; minsum = sum; }
//             else if (sum > maxsum) { rightBottomIndex = i; maxsum = sum; }
//         }

//         int leftBottomIndex = 0, rightTopIndex = 0, tempX = (int)approx_hull[0][leftTopIndex].x;
//         for (int i = 0; i < 4; i++)
//         {
//             if (i != leftTopIndex && i != rightBottomIndex)
//             {
//                 leftBottomIndex = rightTopIndex = i;
//                 break;
//             }
//         }

//         for (int i = 0; i < 4; i++)
//         {
//             if (i != leftTopIndex && i != rightBottomIndex)
//             {
//                 if (approx_hull[0][i].x > tempX)
//                 {
//                     rightTopIndex = i;
//                 }
//                 else
//                 {
//                     leftBottomIndex = i;
//                 }
//             }
//         }

//         src_point = new List<Vector2>();
//         if (!rotate)
//         {
//             src_point.Add(approx_hull[0][leftTopIndex]);
//             src_point.Add(approx_hull[0][leftBottomIndex]);
//             src_point.Add(approx_hull[0][rightTopIndex]);
//             src_point.Add(approx_hull[0][rightBottomIndex]);
//         }
//         else
//         {
//             src_point.Add(approx_hull[0][rightBottomIndex]);
//             src_point.Add(approx_hull[0][rightTopIndex]);
//             src_point.Add(approx_hull[0][leftBottomIndex]);
//             src_point.Add(approx_hull[0][leftTopIndex]);
//         }

//         return src_point;
//     }

//     private void GetCornerXYZ(Vector2 matCorner, out float rx, out float ry, out float rz)
//     {
//         Vector3[] vec = new Vector3[4];
//         corrLTPoint.rectTransform.GetWorldCorners(vec);
//         rz = vec[0].z;
//         rx = matCorner.x / scalingFactorX / w * bgW;
//         ry = matCorner.y / scalingFactorY / h * bgH;
//     }

//     private void PlaceCorrPoints()
//     {
//         if (canTransform)
//         {
//             List<Vector2> matCornerPointsPos = SortCornerPoints();
//             for (int i = 0; i < matCornerPointsPos.Count; i++)
//             {
//                 float posX, posY, posZ;
//                 GetCornerXYZ(matCornerPointsPos[i], out posX, out posY, out posZ);
//                 Image cornerPoint = GetCornerImage(i);
//                 cornerPoint.transform.position = new Vector3(bgX + posX, bgY - posY, posZ);
//             }
//         }
//         else
//         {
//             SetDefaultCornerPositions();
//         }
//     }

//     private Image GetCornerImage(int index)
//     {
//         // Assuming the corners are ordered as LT, LB, RT, RB
//         switch (index)
//         {
//             case 0: return corrLTPoint;
//             case 1: return corrLBPoint;
//             case 2: return corrRTPoint;
//             case 3: return corrRBPoint;
//             default: throw new ArgumentOutOfRangeException("Invalid corner index");
//         }
//     }

//     private void SetDefaultCornerPositions()
//     {
//         // Implement logic for default positions, or use hard-coded values
//         // Example:
//         corrLTPoint.transform.position = new Vector3(bgX + 100, bgY - 100, 0);
//         corrLBPoint.transform.position = new Vector3(bgX + 100, bgY - 400, 0);
//         corrRTPoint.transform.position = new Vector3(bgX + 700, bgY - 100, 0);
//         corrRBPoint.transform.position = new Vector3(bgX + 700, bgY - 400, 0);
//     }

//     private void GetCornerXY(RectTransform rt, out float posX, out float posY)
//     {
//         Vector3[] vec = new Vector3[4];
//         rt.GetWorldCorners(vec);
//         float rX = (vec[1].x + vec[2].x) / 2;
//         float rY = (vec[0].y + vec[1].y) / 2;
//         posX = (w * Mathf.Abs(bgX - rX) / bgW) * scalingFactorX;
//         posY = (h * Mathf.Abs(bgY - rY) / bgH) * scalingFactorY;
//     }

//     private List<Vector2> GetCorrCornerPos()
//     {
//         List<Vector2> corrCorners = new List<Vector2>();
//         float corrXpos, corrYpos;

//         GetCornerXY(corrLTPoint.rectTransform, out corrXpos, out corrYpos);
//         corrCorners.Add(new Vector2(corrXpos, corrYpos));
//         GetCornerXY(corrLBPoint.rectTransform, out corrXpos, out corrYpos);
//         corrCorners.Add(new Vector2(corrXpos, corrYpos));
//         GetCornerXY(corrRTPoint.rectTransform, out corrXpos, out corrYpos);
//         corrCorners.Add(new Vector2(corrXpos, corrYpos));
//         GetCornerXY(corrRBPoint.rectTransform, out corrXpos, out corrYpos);
//         corrCorners.Add(new Vector2(corrXpos, corrYpos));

//         return corrCorners;
//     }

//     // private void CalculateTransformationMatrix()
//     // {
//     //     // get four rectangles position
//     //     src_point = GetCorrCornerPos();

//     //     int src_width = w;
//     //     int src_height = h;

//     //     // add offset to show the full transformed image (quadrilateral)
//     //     woffset = w * ratio;
//     //     hoffset = h * ratio;

//     //     // Assuming destination_point is a List<Vector2>
//     //     destination_point = new List<Vector2>();
//     //     destination_point.Add(new Vector2(woffset, hoffset));
//     //     destination_point.Add(new Vector2(woffset, hoffset + src_height));
//     //     destination_point.Add(new Vector2(woffset + src_width, hoffset));
//     //     destination_point.Add(new Vector2(woffset + src_width, hoffset + src_height));

//     //     // Call to OpenCV's GetPerspectiveTransform should be adapted if needed
//     //     // homography_mat = Cv2.GetPerspectiveTransform(src_point, destination_point);
//     //     // inverse_homography_mat = Cv2.GetPerspectiveTransform(destination_point, src_point);
//     //     calculated = true;
//     // }

//     // private void RecalculateTransform()
//     // {
//     //     // matrix to transform the original image to cropped transformed image
//     //     RecordRectPos();
//     //     cropRectW = (int)(tempW * rectImageTransform.sizeDelta.x / bgW);
//     //     cropRectH = (int)(tempH * rectImageTransform.sizeDelta.y / bgH);

//     //     cropRectUpX = (tempW * Mathf.Abs(bgX - rectX) / bgW) * scalingFactorX;
//     //     cropRectUpY = (tempH * Mathf.Abs(bgY - rectY) / bgH) * scalingFactorY;

//     //     // Assuming src_point is a List<Vector2>
//     //     src_point = new List<Vector2>();
//     //     var inverseP1 = CalculateInversePoint(cropRectUpX, cropRectUpY);
//     //     src_point.Add(new Vector2(inverseP1.Item1, inverseP1.Item2));
//     //     var inverseP2 = CalculateInversePoint(cropRectUpX, cropRectUpY + cropRectH);
//     //     src_point.Add(new Vector2(inverseP2.Item1, inverseP2.Item2));
//     //     var inverseP3 = CalculateInversePoint(cropRectUpX + cropRectW, cropRectUpY);
//     //     src_point.Add(new Vector2(inverseP3.Item1, inverseP3.Item2));
//     //     var inverseP4 = CalculateInversePoint(cropRectUpX + cropRectW, cropRectUpY + cropRectH);
//     //     src_point.Add(new Vector2(inverseP4.Item1, inverseP4.Item2));

//     //     // Call to OpenCV's GetPerspectiveTransform should be adapted if needed
//     //     // homography_mat = Cv2.GetPerspectiveTransform(src_point, destination_point);
//     // }

//     // private (float, float) CalculateInversePoint(float posX, float posY)
//     // {
//     //     // This method's implementation may depend on the details of your homography matrix
//     //     // You might need to adjust the logic based on how you're handling the transformation matrix
//     //     float inverseX = /* ... */;
//     //     float inverseY = /* ... */;

//     //     return (inverseX, inverseY);
//     // }

//     public void SaveToJSON()
//     {
//         // save the final transformation matrix and user option of "enhance"
//         JObject obj = new JObject();
//         JArray array = new JArray();
//         array.Add(new JObject()
//             {
//                 { "0", homography_mat.GetArray(0,0)[0]},
//                 { "1", homography_mat.GetArray(0,1)[0]},
//                 { "2", homography_mat.GetArray(0,2)[0]},
//                 { "3", homography_mat.GetArray(1,0)[0]},
//                 { "4", homography_mat.GetArray(1,1)[0]},
//                 { "5", homography_mat.GetArray(1,2)[0]},
//                 { "6", homography_mat.GetArray(2,0)[0]},
//                 { "7", homography_mat.GetArray(2,1)[0]},
//                 { "8", homography_mat.GetArray(2,2)[0]},
//             });
//         obj["desk"]= array.ToString();

//         JArray array1 = new JArray();
//         array1.Add(new JObject()
//             {
//                { "enhance",enhance},
//             });
//         obj["enhance"] = array1.ToString();

//         File.WriteAllText(Application.streamingAssetsPath + "/SavedFiles/DeskCalibration.json", obj.ToString());
        
//         saved = true;
//        // readFromJson = false;
//        StartCoroutine(AutoCloseDeskCaliPage());
//     }
    
//     IEnumerator AutoCloseDeskCaliPage()
//     {
//         // auto close after 3 seconds if saving successful
//        yield return new WaitForSeconds(1);
//        deskCaliPage.SetActive(false);
//         menu.SetActive(true);
//     }


//     public Mat ReadFromJson()
//     {
//         var mat_data = JToken.Parse(File.ReadAllText(Application.streamingAssetsPath + "/SavedFiles/DeskCalibration.json"));
//         var mat = JArray.Parse(mat_data["desk"].Value<string>());
//         float[] values = new float[9];
//         Mat matRead = new Mat(3,3,MatType.CV_32F);
//         float[,] matValue = new float[3, 3];
//         foreach (var m in mat)
//         {
//             for (int i = 0; i < 9; i++)
//             {
//                 values[i] = m.Value<float>(i.ToString());
//                 matValue[(int)i / 3, i % 3] = values[i];
//             }
//         }
//         matRead.SetArray(0,0, matValue);
//         return matRead;
//     }

//     private void OnDisable()
//     {
//         if (_webCamera != null)
//             _webCamera.Stop();
//     }

//     private void OnDestroy()
//     {
//         if (_webCamera != null)
//             _webCamera.Stop();
//     }
// }
