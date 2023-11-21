clear
g++ -std=c++11 cpp/test.cpp -o your_program `pkg-config --cflags --libs opencv4` -ldlib -lblas -llapack

# g++ -std=c++14 cpp/test.cpp -o your_program \
#     -I$(brew --prefix opencv@3)/include -L$(brew --prefix opencv@3)/lib \
#     -I$(brew --prefix dlib)/include -L$(brew --prefix dlib)/lib \
#     -lopencv_core -lopencv_highgui -lopencv_imgproc -ldlib

./your_program

