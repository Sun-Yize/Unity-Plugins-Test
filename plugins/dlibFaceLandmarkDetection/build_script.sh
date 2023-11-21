clear
clang++ -std=c++14 -Ofast -march=native -shared -o build/lib/libFaceLandmarkDetector.dylib \
    cpp/dlibFaceLandmarkDetector.cpp \
    -I/Users/charlie/Documents/vscode_projects/build_test/repos/build/dlib-19.24-static/include \
    -I/Users/charlie/Documents/vscode_projects/build_test/plugins/OpenCv/opencv4/ \
    -L/Users/charlie/Documents/vscode_projects/build_test/repos/build/dlib-19.24-static/lib \
    -L/Users/charlie/Documents/vscode_projects/build_test/plugins/OpenCv/opencv4/ \
    -lopencv -ldlib -framework Accelerate

cp build/lib/libFaceLandmarkDetector.dylib /Users/charlie/Documents/vscode_projects/build_test/unity_test/Assets/Plugins/DlibFaceLandmarkDetector/Plugins/macos/libFaceLandmarkDetector.dylib
