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
