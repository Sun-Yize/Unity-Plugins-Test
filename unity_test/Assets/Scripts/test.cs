using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using DlibFace;

public class test : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        Debug.Log("test");
        Debug.Log(DlibFace.MainFace.v1);
        Debug.Log(DlibFace.MainFace.DlibFaceLandmarkDetector_Init());
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
