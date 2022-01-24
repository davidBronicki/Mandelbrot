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

inline double randomFloat(){
	return (double)rand() / RAND_MAX;
}

inline double randomFloat(double maxVal){
	return maxVal * randomFloat();
}

inline double randomFloat(double minVal, double maxVal){
	return minVal + randomFloat(maxVal - minVal);
}

template<class T>
T clamp(T value, T minVal, T maxVal){
	if (value < minVal){
		return minVal;
	}
	else if (value > maxVal){
		return maxVal;
	}
	else{
		return value;
	}
}

template<class T, class S>
S map(T value, T fromLow, T fromHigh, S toLow, S toHigh){
	return (S)((value - fromLow) * 
		(toHigh - toLow) / (fromHigh - fromLow) +
		toLow);
}

int colorMap(double norm){

	vector<vector<double>> splitVals
	{
		{
			0.0 / 3,
			0.0, 0.0, 0.0
		},
		{
			1.8 / 3,
			0.0, 0.0, 255.0
		},
		{
			2.8 / 3,
			255.0, 0.0, 0.0
		},
		{
			3.0 / 3,
			255.0, 255.0, 255.0
		}
	};
	norm = pow(norm, 1.3);

	// vector<vector<double>> splitVals
	// {
	// 	{
	// 		0.0 / 3,
	// 		0.0, 0.0, 0.0
	// 	},
	// 	{
	// 		1.0 / 3,
	// 		180.0, 0.0, 0.0
	// 	},
	// 	{
	// 		1.7 / 3,
	// 		255.0, 80.0, 30.0
	// 	},
	// 	{
	// 		2.40 / 3,
	// 		100.0, 255.0, 50.0
	// 	},
	// 	{
	// 		2.67 / 3,
	// 		70.0, 170.0, 150.0
	// 	},
	// 	{
	// 		2.8 / 3,
	// 		40.0, 80.0, 255.0
	// 	},
	// 	{
	// 		3.0 / 3,
	// 		240.0, 255.0, 240.0
	// 	}
	// };

	// vector<vector<double>> splitVals
	// {
	// 	{
	// 		0.0 / 3,
	// 		0.0, 0.0, 0.0
	// 	},
	// 	{
	// 		1.0 / 3,
	// 		180.0, 0.0, 0.0
	// 	},
	// 	{
	// 		1.7 / 3,
	// 		255.0, 80.0, 30.0
	// 	},
	// 	{
	// 		2.40 / 3,
	// 		100.0, 255.0, 50.0
	// 	},
	// 	{
	// 		2.7 / 3,
	// 		70.0, 170.0, 150.0
	// 	},
	// 	{
	// 		2.85 / 3,
	// 		40.0, 80.0, 255.0
	// 	},
	// 	{
	// 		2.95 / 3,
	// 		220.0, 80.0, 100.0
	// 	},
	// 	{
	// 		3.0 / 3,
	// 		240.0, 255.0, 240.0
	// 	}
	// };
	// norm = pow(norm, 1.5);
	for (int i = 1; i < splitVals.size(); i++)
	{
		if (norm < splitVals[i][0] || i == splitVals.size() - 1)
		{
			return buildColor(
				map(norm, splitVals[i - 1][0], splitVals[i][0],
					splitVals[i - 1][1], splitVals[i][1]),
				map(norm, splitVals[i - 1][0], splitVals[i][0],
					splitVals[i - 1][2], splitVals[i][2]),
				map(norm, splitVals[i - 1][0], splitVals[i][0],
					splitVals[i - 1][3], splitVals[i][3]));
		}
	}
	return 0;
}

void colorMap(vector<int>& colors, int recursionDepth){
	int maxVal = 0;
	for (int i = 0; i < colors.size(); i++){
		if (colors[i] >= recursionDepth){
			colors[i] = -1;
		}
		maxVal = max(maxVal, colors[i]);
	}
	vector<int> bins(maxVal + 1);
	for (int value : colors){
		if (value != -1){
			bins[value]++;
		}
	}
	int total = 0;
	for (int i = 0; i < bins.size(); i++){
		total += bins[i];
		bins[i] = total;
	}
	vector<double> normedBins(bins.size());
	for (int i = 0; i < bins.size(); i++){
		normedBins[i] = (double)(bins[i]) / total;
	}
	for (int i = 0; i < colors.size(); i++){
		colors[i] = colorMap(normedBins[colors[i]]);
	}
}

void placeHorizontalLine(Color color, vector<char>& data,
		int y, int width){
	if (y >= width || y < 0) return;
	if (width * width * 4 + 54 != data.size()){
		throw exception();
	}
	int* dataBase = (int*)(void*)&(data[54]);
	for (int x = 0; x < width; x++){
		dataBase[x + width * y] = color.paintedOn(Color::buildFromInt(
			dataBase[x + width * y]));
	}
}

void placeHorizontalLine(double magnitude, vector<char>& data,
	int width, double yPos)
{
	magnitude *= width;
	magnitude /= 100;
	yPos *= width;
	yPos /= 2;
	yPos += width / 2;
	if (magnitude < 1)
	{
		placeHorizontalLine(Color(0, magnitude), data,
			(int)(yPos + 0.5), width);
	}
	else
	{
		for (int i = 0; i < magnitude; i++)
		{
			placeHorizontalLine(Color(0), data,
				(int)(yPos + i - magnitude / 2 + .5), width);
		}
		// int maxVal = (int)magnitude;
		// if ((int)magnitude != magnitude) maxVal++;
		// for (int i = 0; i < maxVal; i++)
		// {
		// 	double tempY = yPos + i - magnitude / 2;
		// 	if (i != 0 && i != maxVal - 1)
		// 	{
		// 		placeHorizontalLine(Color(0), data,
		// 			(int)(tempY + 0.5), width);
		// 	}
		// 	else
		// 	{
		// 		placeHorizontalLine(Color(0, (magnitude - (int)magnitude) / 2),
		// 			data, (int)(tempY + 0.5), width);
		// 	}
		// }
	}
}

void placeVerticalLine(Color color, vector<char>& data,
		int x, int width){
	if (x >= width || x < 0) return;
	if (width * width * 4 + 54 != data.size()){
		throw exception();
	}
	int* dataBase = (int*)(void*)&(data[54]);
	for (int y = 0; y < width; y++){
		dataBase[x + width * y] = color.paintedOn(Color::buildFromInt(
			dataBase[x + width * y]));
	}
}

void placeVerticalLine(double magnitude, vector<char>& data,
	int width, double xPos)
{
	magnitude *= width;
	magnitude /= 100;
	xPos *= width;
	xPos /= 2;
	xPos += width / 2;
	if (magnitude < 1)
	{
		placeVerticalLine(Color(0, magnitude), data,
			(int)(xPos + 0.5), width);
	}
	else
	{
		for (int i = 0; i < magnitude; i++)
		{
			placeVerticalLine(Color(0), data,
				(int)(xPos + i - magnitude / 2 + .5), width);
		}
		// int maxVal = (int)magnitude;
		// if ((int)magnitude != magnitude) maxVal++;
		// for (int i = 0; i < maxVal; i++)
		// {
		// 	double tempY = xPos + i - magnitude / 2;
		// 	if (i != 0 && i != maxVal - 1)
		// 	{
		// 		placeVerticalLine(Color(0), data,
		// 			(int)(tempY + 0.5), width);
		// 	}
		// 	else
		// 	{
		// 		placeVerticalLine(Color(0,
		// 			(magnitude - (int)magnitude) / 2),
		// 			data, (int)(tempY + 0.5), width);
		// 	}
		// }
	}
}

// void halfPointPlacer(vector<char>& data, int width, double r, 
// 	double xPos, double yPos, double separation);
void fullPointPlacer(vector<char>& data, int width, double r, 
	double xPos, double yPos, double separation)
{
	double magnitude = min(separation / r, 1.0);
	if (magnitude < 0.05) return;
	placeHorizontalLine(magnitude, data, width, yPos / r);
	placeVerticalLine(magnitude, data, width, xPos / r);
	for (int i = -4; i < 6; i++)
	{
		fullPointPlacer(data, width, r,
			xPos + i * separation / 10,
			yPos + i * separation / 10,
			separation / 10);
	}
}

// void halfPointPlacer(vector<char>& data, int width, double r, 
// 	double xPos, double yPos, double separation)
// {
// 	double magnitude = separation / r;
// 	if (magnitude < 0.05) return;
// 	placeHorizontalLine(magnitude, data, yPos / r, width);
// 	placeVerticalLine(magnitude, data, xPos / r, width);
// }

inline double prepVal(highPrec val)
{
	// print("prior");
	// print(val.toString());
	// print(val.toDouble());
	val.removeInt();
	// print("post");
	// print(val.toString());
	// print(val.toDouble());
	val *= 10;
	double output = val.toDouble();
	if (output < -5) output += 10;
	if (output > 5) output -= 10;
	// print(output);
	return output;
}

void formatGridLines(vector<char>& data, highPrec x, highPrec y,
	int width, double r)
{
	int counter = 1;
	double temp = log10(r);
	int exponent = (int)temp;
	if (exponent != temp && temp < 0){
		exponent--;
	}
	double newX, newY;

	cout.precision(16);

	if (exponent == -1)
	{
		// print("case 0");
		newX = prepVal(x);
		newY = prepVal(y);
	}
	else
	{
		// highPrec tenVal(10.0);
		// print(tenVal.toDouble());
		if (exponent < -1)
		{
			// print("case 1");
			// print(exponent);
			for (int i = -1; i > exponent; i--)
			{
				x *= 10;
				y *= 10;
			}
			newX = prepVal(x);
			newY = prepVal(y);
		}
		else
		{
			// print("case 2");
			for (int i = -1; i < exponent; i++)
			{
				// x /= tenVal;
				// y /= tenVal;
				x /= 10;
				y /= 10;
			}
			newX = prepVal(x);
			newY = prepVal(y);
		}
	}


	// cout.precision(16);
	// x /= highPrec(pow(10, exponent + 1));
	// x.removeInt();
	// x *= 10;
	// double newX = x.toDouble();
	// if (newX < -5) newX += 10;
	// if (newX > 5) newX -= 10;

	// y /= highPrec(pow(10, exponent + 1));
	// y.removeInt();
	// y *= 10;
	// double newY = y.toDouble();
	// if (newY < -5) newY += 10;
	// if (newY > 5) newY -= 10;

	r /= pow(10, exponent);
	fullPointPlacer(data, width, r, -newX - 10, -newY - 10, 10);
	fullPointPlacer(data, width, r, -newX, -newY, 10);
	fullPointPlacer(data, width, r, -newX + 10, -newY + 10, 10);
}


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
	// vector<complex> output;
	// highPrec currentX, currentY;
	// for (int i = 0; i < recursionDepth; i++){
	// 	output.push_back(complex(
	// 		currentX.toDouble(), currentY.toDouble()));
	// 	highPrec temp(currentX * currentX - currentY * currentY + x);
	// 	currentY = currentX * currentY * 2.0 + y;
	// 	currentX = temp;
	// }
	// return output;

	vector<complex> output;
	highPrec currentX, currentY;
	highPrec cutoff(2.0);
	for (int i = 0; i < recursionDepth; i++){
		output.push_back(complex(
			currentX.toDouble(), currentY.toDouble()));
		if (currentX * currentX + currentY * currentY > cutoff) break;
		highPrec temp(currentX * currentX - currentY * currentY + x);
		currentY = currentX * currentY * 2.0 + y;
		currentX = temp;
	}
	return output;
}

// void recursiveFindAnchor(const highPrec& lowX, const highPrec& highX,
// 		const highPrec& lowY, const highPrec& highY,
// 		highPrec& x, highPrec& y, double dz, int& currentValue,
// 		const int& mandelRecusionDepth, int findingRecursionDepth){
// 	if (findingRecursionDepth == 0 ||
// 		currentValue >= mandelRecusionDepth) return;
// 	double angle = randomFloat(2 * PI);
// 	double mag = randomFloat(dz);
// 	highPrec newX(x + highPrec(mag * cos(angle)));
// 	highPrec newY(y + highPrec(mag * sin(angle)));
// 	newX = clamp(newX, lowX, highX);
// 	newY = clamp(newY, lowY, highY);
// 	int temp = highPrecMandelbrotValue(newX, newY, mandelRecusionDepth);
// 	if (temp >= currentValue){
// 		currentValue = temp;
// 		x = newX;
// 		y = newY;
// 	}
// 	recursiveFindAnchor(lowX, highX, lowY, highY, x, y, dz, currentValue,
// 		mandelRecusionDepth, findingRecursionDepth - 1);
// }

// tuple<highPrec, highPrec, int> findAnchor(
// 		highPrec x, highPrec y, double r, double dz,
// 		const int& mandelRecusionDepth, int findingRecursionDepth){
// 	highPrec rad(r);
// 	highPrec lowX(x - rad);
// 	highPrec highX(x + rad);
// 	highPrec lowY(y - rad);
// 	highPrec highY(y + rad);
// 	int currentValue = highPrecMandelbrotValue(x, y, mandelRecusionDepth);
// 	recursiveFindAnchor(lowX, highX, lowY, highY, x, y, dz, currentValue,
// 		mandelRecusionDepth, findingRecursionDepth);
// 	return make_tuple(x, y, currentValue);
// }

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
			// print("perturbative precision used");
			// auto anchor = findAnchor(middleX, middleY, radius, radius / 10,
			// 	recursionDepth, max(recursionDepth,
			// 		sampleWidth * sampleWidth / 1000));//expensive
			// int recursionDepth = get<2>(anchor);
			// print("recursionDepth " << recursionDepth);
			// auto anchorX = get<0>(anchor);
			// auto anchorY = get<1>(anchor);
			// double offsetX = (anchorX - middleX).toDouble();
			// double offsetY = (anchorY - middleY).toDouble();
			// complex offset(offsetX, offsetY);
			// vector<complex> baseValues = getPerturbationVals(
			// 	anchorX, anchorY, recursionDepth);
			// for (int i = 0; i < sampleWidth; i++){
			// 	print("calculating row " << i);
			// 	for (int j = 0; j < sampleWidth; j++){
			// 		double x = -radius + radius * i * 2 / sampleWidth;
			// 		double y = -radius + radius * j * 2 / sampleWidth;
			// 		complex dc(x, y);
			// 		colors[sampleWidth * j + i] = perturbativeMandelbrot(
			// 			baseValues, dc - offset, recursionDepth);
			// 	}
			// }
			// print("perturbative precision used");
			// print("recursionDepth " << recursionDepth);

			print("perturbative precision used");
			// auto anchor = findAnchor(middleX, middleY, radius, radius / 10,
			// 	recursionDepth, max(recursionDepth,
			// 		sampleWidth * sampleWidth / 1000));//expensive
			// int recursionDepth = get<2>(anchor);
			// print("recursionDepth " << recursionDepth);
			highPrec anchorX(middleX);
			highPrec anchorY(middleY);
			double offsetX = 0;
			double offsetY = 0;
			complex offset(offsetY, offsetY);
			print("reached");
			vector<complex> baseValues = getPerturbationVals(
				anchorX, anchorY, recursionDepth);
			print("reached");
			for (int i = 0; i < sampleWidth; i++)
			{
				print("calculating row " << i);
				for (int j = 0; j < sampleWidth; j++)
				{
					double x = -radius + radius * i * 2 / sampleWidth;
					double y = -radius + radius * j * 2 / sampleWidth;
					complex dc(x, y);
					int temp = perturbativeMandelbrot(
						baseValues, dc - offset, recursionDepth);
					if (baseValues.size() < recursionDepth && temp == baseValues.size())
					{
						anchorX = middleX + highPrec(x);
						anchorY = middleY + highPrec(y);
						offset = complex(x, y);
						baseValues = getPerturbationVals(anchorX,
							anchorY, recursionDepth);
						temp = baseValues.size();
					}
					colors[sampleWidth * j + i] = temp;
				}
			}
			print("perturbative precision used");
			print("recursionDepth " << baseValues.size());
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
	print("xPos/x, 1 float, spec x position");
	print("yPos/y, 1 float, spec y position");
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
	handler.addFlag({"xPos", "x"}, vector<string>{"0"}, 1);
	handler.addFlag({"yPos", "y"}, vector<string>{"0"}, 1);
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
			string tempString = (string)(handler.getFlagValue("x"));
			highPrec x(tempString, tempString.length() + 5);
			tempString = (string)(handler.getFlagValue("y"));
			highPrec y(tempString, tempString.length() + 5);
			y.negate();
			double r = handler.getFlagValue("r");
			string name = handler.getFlagValue("name");
			int imageWidth = (long long)(handler.getFlagValue("width"));
			int recursionDepth = (long long)(handler.getFlagValue("recursion"));
			vector<char> data = mandelbrot(x, y, r,
				imageWidth, recursionDepth, handler.isFlagCalled("exact"));
			if (handler.isFlagCalled("grid")){
				formatGridLines(data, x, y, imageWidth, r);
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
