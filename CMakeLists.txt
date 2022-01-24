cmake_minimum_required(VERSION 3.0)
project(MANDLEBROT_SEARCH CXX)

file(GLOB Main_Code_Pile
	"*.cpp")

add_executable(mandelbrot_search ${Main_Code_Pile})

set_target_properties(mandelbrot_search PROPERTIES CXX_STANDARD 14 CXX_STANDARD_REQUIRED ON)
