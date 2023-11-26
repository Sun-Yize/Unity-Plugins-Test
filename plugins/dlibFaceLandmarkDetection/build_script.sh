clear
clang++ -std=c++14 -Ofast -march=native -shared -o build/lib/libVCPlugin.dylib \
    cpp/vcFaceDet.cpp \
    cpp/vcImgProc.cpp \
    cpp/vcDeskCal.cpp \
    -I/Users/charlie/Documents/vscode_projects/build_test/repos/build/dlib-19.24-static/include \
    -I/Users/charlie/Documents/vscode_projects/build_test/plugins/OpenCv/opencv4/ \
    -L/Users/charlie/Documents/vscode_projects/build_test/repos/build/dlib-19.24-static/lib \
    -L/Users/charlie/Documents/vscode_projects/build_test/plugins/OpenCv/opencv4/ \
    -lopencv -ldlib -framework Accelerate

cp build/lib/libVCPlugin.dylib /Users/charlie/Documents/vscode_projects/build_test/unity_test/Assets/Plugins/VCPlugins/Plugins/macos/libVCPlugin.dylib
