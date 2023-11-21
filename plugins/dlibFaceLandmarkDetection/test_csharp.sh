# clear 
# g++ cpp/test.cpp -o face_detector \
#     -I$(brew --prefix opencv)/include/opencv4/ -L$(brew --prefix opencv)/lib \
#     -I$(brew --prefix dlib)/include -L$(brew --prefix dlib)/lib \
#     -lopencv_core -lopencv_highgui -lopencv_imgproc -ldlib -framework Accelerate \
#     -std=c++14
# ./face_detector


clear
clang++ -std=c++14 -O3 -march=native -shared -o build/lib/libFaceLandmarkDetector.dylib \
    cpp/dlibFaceLandmarkDetector.cpp \
    -I/Users/charlie/Documents/vscode_projects/build_test/repos/build/dlib-19.24-static/include \
    -I/Users/charlie/Documents/vscode_projects/build_test/plugins/OpenCv/opencv4/ \
    -L/Users/charlie/Documents/vscode_projects/build_test/repos/build/dlib-19.24-static/lib \
    -L/Users/charlie/Documents/vscode_projects/build_test/plugins/OpenCv/opencv4/ \
    -lopencv -ldlib -framework Accelerate

# g++ -std=c++14 -shared -o build/lib/libFaceLandmarkDetector.dylib \
#     cpp/dlibFaceLandmarkDetector.cpp \
#     -I/Users/charlie/Documents/vscode_projects/build_test/repos/build/dlib-19.24-static/include \
#     -I/Users/charlie/Documents/vscode_projects/build_test/repos/build/opencv2.framework/ \
#     -L/Users/charlie/Documents/vscode_projects/build_test/repos/build/dlib-19.24-static/lib \
#     -L/Users/charlie/Documents/vscode_projects/build_test/repos/build/opencv2.framework/ \
#     -lopencv -ldlib -framework Accelerate

clang++ -std=c++14 -O3 -march=native face_detector \
    cpp/implyment.cpp \
    -I$(brew --prefix opencv@3)/include -L$(brew --prefix opencv@3)/lib \
    -I$(pwd)/build/include -L$(pwd)/build/lib \
    -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc \
    -lFaceLandmarkDetector

./face_detector
