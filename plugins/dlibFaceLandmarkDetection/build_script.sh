clear

g++ -std=c++14 -shared -o build/lib/libFaceLandmarkDetector.dylib \
    cpp/dlibFaceLandmarkDetector.cpp \
    -I/Users/charlie/Documents/vscode_projects/build_test/repos/build/dlib-19.24-static/include \
    -I/Users/charlie/Documents/vscode_projects/build_test/repos/build/opencv2.framework/ \
    -L/Users/charlie/Documents/vscode_projects/build_test/repos/build/dlib-19.24-static/lib \
    -L/Users/charlie/Documents/vscode_projects/build_test/repos/build/opencv2.framework/ \
    -lopencv -ldlib -framework Accelerate

g++ cpp/implyment.cpp -o face_detector \
    -I$(brew --prefix opencv@3)/include -L$(brew --prefix opencv@3)/lib \
    -I$(pwd)/build/include -L$(pwd)/build/lib \
    -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc \
    -lFaceLandmarkDetector \
    -std=c++14

./face_detector
