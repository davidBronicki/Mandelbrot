g++ -std=c++17 -c highPrec.cpp -o highPrec.o
g++ -std=c++17 -c mandelbrot.cpp -o mandelbrot.o
g++ -std=c++17 -c argsHandler.cpp -o argsHandler.o
g++ -std=c++17 highPrec.o mandelbrot.o argsHandler.o -o mandelbrot.exe
