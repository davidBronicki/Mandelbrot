#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <math.h>
#include <cmath>
#include <algorithm>

using namespace std;

#define print(input) cout << input << endl

#define PI 3.141592653589793

#include "imageStuff.h"
#include "complex.h"
#include "highPrec.h"
#include "argsHandler.h"
#include "mandelUtils.cpp"

int lowPrecMandelbrotValue(double x, double y,
		int recursionDepth){
	double currentX = 0;
	double currentY = 0;
	for (int i = 0; i < recursionDepth ; i++){
		double temp1 = (currentX * currentX);
		double temp2 = (currentY * currentY);
		if (temp1 + temp2 >= 4){
			return i;
		}
		else{
			currentY = 2 * currentX * currentY + y;
			currentX = temp1 - temp2 + x;
		}
	}
	return recursionDepth;
}

int highPrecMandelbrotValue(highPrec x, highPrec y,
		int recursionDepth){
	highPrec comparator(4.0);
	int targetPrec = max(x.getPrec(), y.getPrec()) + 1;
	targetPrec = max(targetPrec, 3);
	highPrec currentX;
	highPrec currentY;
	currentX.forcePrec(targetPrec);
	currentY.forcePrec(targetPrec);
	for (int i = 0; i < recursionDepth ; i++){
		highPrec temp1(currentX * currentX);
		highPrec temp2(currentY * currentY);
		if (temp1 + temp2 >= comparator){
			return i;
		}
		else{
			currentY = operator*(currentX * currentY, 2) + y;
			currentX = temp1 - temp2 + x;
		}
		currentY.forcePrec(targetPrec);
		currentX.forcePrec(targetPrec);
	}
	return recursionDepth;
}

int perturbativeMandelbrot(
		vector<complex> baseVals, complex dc,
		int recursionDepth){
	complex currentValue;
	for (int i = 0; i < recursionDepth; i++){
		if ((currentValue + baseVals[i]).magnitudeSquared() > 4){
			return i;
		}
		currentValue = baseVals[i] * currentValue * 2.0 +
			currentValue * currentValue + dc;
	}
	return recursionDepth;
}

vector<complex> getPerturbationVals(highPrec x, highPrec y,
		int recursionDepth){
	vector<complex> output;
	highPrec currentX, currentY;
	for (int i = 0; i < recursionDepth; i++){
		output.push_back(complex(
			currentX.toDouble(), currentY.toDouble()));
		highPrec temp(currentX * currentX - currentY * currentY + x);
		currentY = currentX * currentY * 2.0 + y;
		currentX = temp;
	}
	return output;
}

void recursiveFindAnchor(const highPrec& lowX, const highPrec& highX,
		const highPrec& lowY, const highPrec& highY,
		highPrec& x, highPrec& y, double dz, int& currentValue,
		const int& mandelRecusionDepth, int findingRecursionDepth){
	if (findingRecursionDepth == 0 ||
		currentValue >= mandelRecusionDepth) return;
	double angle = randomFloat(2 * PI);
	double mag = randomFloat(dz);
	highPrec newX(x + highPrec(mag * cos(angle)));
	highPrec newY(y + highPrec(mag * sin(angle)));
	newX = clamp(newX, lowX, highX);
	newY = clamp(newY, lowY, highY);
	int temp = highPrecMandelbrotValue(newX, newY, mandelRecusionDepth);
	if (temp >= currentValue){
		currentValue = temp;
		x = newX;
		y = newY;
	}
	recursiveFindAnchor(lowX, highX, lowY, highY, x, y, dz, currentValue,
		mandelRecusionDepth, findingRecursionDepth - 1);
}

tuple<highPrec, highPrec, int> findAnchor(
		highPrec x, highPrec y, double r, double dz,
		const int& mandelRecusionDepth, int findingRecursionDepth){
	highPrec rad(r);
	highPrec lowX(x - rad);
	highPrec highX(x + rad);
	highPrec lowY(y - rad);
	highPrec highY(y + rad);
	int currentValue = highPrecMandelbrotValue(x, y, mandelRecusionDepth);
	recursiveFindAnchor(lowX, highX, lowY, highY, x, y, dz, currentValue,
		mandelRecusionDepth, findingRecursionDepth);
	return make_tuple(x, y, currentValue);
}

vector<char> mandelbrot(
		highPrec middleX, highPrec middleY, double radius,
		int sampleWidth, int recursionDepth, bool exact){
	vector<int> colors(sampleWidth * sampleWidth);
	if ((middleX * middleX + middleY * middleY) *
		highPrec(sampleWidth * sampleWidth / (radius * radius))
			> highPrec(1e30)){
		if (exact){
			print("high precision used");
			highPrec r(radius);
			for (int i = 0; i < sampleWidth; i++){
				print("calculating row " << i);
				highPrec x(middleX - r
					+ highPrec(radius * 2 * i / sampleWidth));
				for (int j = 0; j < sampleWidth; j++){
					highPrec y(middleY - r
						+ highPrec(radius * 2 * j / sampleWidth));
					colors[sampleWidth * j + i] = highPrecMandelbrotValue(
						x, y, recursionDepth);
				}
			}
		}
		else{
			print("perturbative precision used");
			auto anchor = findAnchor(middleX, middleY, radius, radius / 10,
				recursionDepth, max(recursionDepth,
					sampleWidth * sampleWidth / 1000));
			recursionDepth = get<2>(anchor);
			print("recursionDepth " << recursionDepth);
			auto anchorX = get<0>(anchor);
			auto anchorY = get<1>(anchor);
			double offsetX = (anchorX - middleX).toDouble();
			double offsetY = (anchorY - middleY).toDouble();
			complex offset(offsetX, offsetY);
			vector<complex> baseValues = getPerturbationVals(
				anchorX, anchorY, recursionDepth);
			for (int i = 0; i < sampleWidth; i++){
				print("calculating row " << i);
				for (int j = 0; j < sampleWidth; j++){
					double x = -radius + radius * i * 2 / sampleWidth;
					double y = -radius + radius * j * 2 / sampleWidth;
					complex dc(x, y);
					colors[sampleWidth * j + i] = perturbativeMandelbrot(
						baseValues, dc - offset, recursionDepth);
				}
			}
			print("perturbative precision used");
			print("recursionDepth " << recursionDepth);
		}
	}
	else{
		// highPrec r(radius);
		print("low precision used");
		double lowX = middleX.toDouble();
		double lowY = middleY.toDouble();
		for (int i = 0; i < sampleWidth; i++){
			print("calculating row " << i);
			double x = lowX - radius +
				2 * radius * i / sampleWidth;
			// print(x);
			for (int j = 0; j < sampleWidth; j++){
				double y = lowY - radius +
					2 * radius * j / sampleWidth;
				colors[sampleWidth * j + i] = lowPrecMandelbrotValue(
					x, y, recursionDepth);
			}
		}
	}
	colorMap(colors, recursionDepth * 0.99);
	return buildBitMap(colors, sampleWidth, sampleWidth);
}

void help(){
	print("\n");
	print("builds the mandelbrot set and exports to bitmap file");
	print("");
	print("all flags are prepended by a \"-\"");
	print("flagname/fn, count & type, description");
	print("");
	print("name/n, n strings, spec file name");
	print("xPos/x, n floats, spec x position");
	print("yPos/y, n floats, spec y position");
	print("radius/r, 1 float, spec radius to sample");
	print("width/w, 1 int, spec image width in pixels");
	print("recursion/rd/d, 1 int, spec recursion depth");
	print("");
	print("grid/g, nill, throw to show basic grid");
	print("exact/e, nill, throw to use exact calculation");
	print("\n");
}

int main(int argc, char** argv){

	arguments::argumentsHandler handler;
	handler.addFlag({"xPos", "x"}, vector<double>{0.0}, -1);
	handler.addFlag({"yPos", "y"}, vector<double>{0.0}, -1);
	handler.addFlag({"name", "n"}, vector<string>{"default"}, -1);
	handler.addFlag({"radius", "r"}, vector<double>{2.0}, 1);
	handler.addFlag({"width", "w"}, vector<long long>{300}, 1);
	handler.addFlag({"recursion", "rd", "d"}, vector<long long>{100}, 1);
	handler.addFlag({"grid", "g"}, arguments::dataType::stringValue, 0);
	handler.addFlag({"exact", "e"}, arguments::dataType::stringValue, 0);
	handler.addFlag({"help", "h"}, arguments::dataType::stringValue, -1);
	handler.passArgs(argc, argv);

	if (handler.getFlagValue("h")){
		help();
	}
	else{
		try{
			highPrec x(handler.getFlagValue("x").operator vector<double>());
			highPrec y(handler.getFlagValue("y").operator vector<double>());
			double r = handler.getFlagValue("r");
			string name = handler.getFlagValue("name");
			int imageWidth = (long long)(handler.getFlagValue("width"));
			int recursionDepth = (long long)(handler.getFlagValue("recursion"));
			vector<char> data = mandelbrot(x, y, r,
				imageWidth, recursionDepth, handler.isFlagCalled("exact"));
			if (handler.isFlagCalled("grid")){
				placeVerticalLine(buildColor(127), data,
					imageWidth / 4, imageWidth, imageWidth);
				placeHorizontalLine(buildColor(127), data,
					imageWidth / 4, imageWidth, imageWidth);
				placeVerticalLine(buildColor(127), data,
					imageWidth * 3 / 4, imageWidth, imageWidth);
				placeHorizontalLine(buildColor(127), data,
					imageWidth * 3 / 4, imageWidth, imageWidth);
				placeVerticalLine(buildColor(0), data,
					imageWidth / 2, imageWidth, imageWidth);
				placeHorizontalLine(buildColor(0), data,
					imageWidth / 2, imageWidth, imageWidth);
			}
			ofstream file;
			file.open(name + ".bmp", ios::binary);
			file.write(&(data[0]), data.size());
			file.close();
		}
		catch(arguments::argumentsError e){
			print(e.what());
			help();
			return 0;
		}
		catch(exception e){
			print(e.what());
			return 1;
		}
	}

	return 0;
}
