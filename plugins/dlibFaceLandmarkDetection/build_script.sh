clear

g++ -std=c++14 -shared -o build/lib/libFaceLandmarkDetector.dylib \
cpp/dlibFaceLandmarkDetector.cpp \
-I$(brew --prefix opencv@3)/include -I$(brew --prefix dlib)/include \
-L$(brew --prefix opencv@3)/lib -L$(brew --prefix dlib)/lib \
-ldlib -lopencv_core -framework Accelerate

g++ cpp/implyment.cpp -o face_detector \
-I$(brew --prefix opencv@3)/include -L$(brew --prefix opencv@3)/lib \
-I$(brew --prefix dlib)/include -L$(brew --prefix dlib)/lib \
-I$(pwd)/build/include -L$(pwd)/build/lib \
-lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc \
-lFaceLandmarkDetector \
-std=c++14

./face_detector
