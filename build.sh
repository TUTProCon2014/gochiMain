g++ -O3 -rdynamic -std=c++1y test_main.cpp -o app `pkg-config --cflags --libs opencv`