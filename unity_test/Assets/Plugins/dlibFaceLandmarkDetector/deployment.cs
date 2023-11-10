using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;


namespace DlibFace {
    public class MainFace {
        [DllImport("FaceLandmarkDetector")]
        public static extern IntPtr DlibFaceLandmarkDetector_Init();
        public static string v1 = "sdasd";
    }
}
