# Mandelbrot

This is a basic program to explore the mandelbrot set by exporting to a bmp file.

## Getting started

Some notes for setting up the program.

### Prereqs

Pretty simple. Just have a c++ compiler, I use clang. It was built and tested on linux but should work on windows.

### Initial build

Built with cmake. On linux, run the following to get set up:

```
mkdir path/to/build/
cd path/to/build
cmake path/to/source
make
```

After this you will have an executable in the build directory called mandelbrot_search.

### Using the program

Running the command "mandelbrot.exe -h" will display a very basic help string. It will show all flags and arguments for these flags. Typically, you want -x and -y to specify the center point of where you want to look in the mandelbrot set, and -r for what kind of radius you want to look at. If you need more than double precision for your x and y, you can list more number, for example "-x 0.23834298512345 0.4352394e-14" specifies an x value past double precision.

You'll also want -w for how many pixels wide you want the image, and -r or -rd for the recursion depth. For a starter, use -rd 50 or -rd 100. As you zoom in, you'll need to boost this number.

Other commands are: -g (adds 3 x grid lines and 3 y grid lines to help you spot where you want to go) and -e (for exact measure, only important if you are going beyond double precision).

Never use -e. After double precision, a perturbative process is used. Technically it is not exact, but it is orders of magnitude faster, and the difference is not noticable.