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

	// vector<vector<double>> splitVals
	// {
	// 	{
	// 		0.0 / 3,
	// 		0.0, 0.0, 0.0
	// 	},
	// 	{
	// 		1.8 / 3,
	// 		0.0, 0.0, 255.0
	// 	},
	// 	{
	// 		2.8 / 3,
	// 		255.0, 0.0, 0.0
	// 	},
	// 	{
	// 		3.0 / 3,
	// 		255.0, 255.0, 255.0
	// 	}
	// };
	// norm = pow(norm, 1.3);

	vector<vector<double>> splitVals
	{
		{
			0.0 / 3,
			0.0, 0.0, 0.0
		},
		{
			1.0 / 3,
			180.0, 0.0, 0.0
		},
		{
			1.7 / 3,
			255.0, 80.0, 30.0
		},
		{
			2.40 / 3,
			100.0, 255.0, 50.0
		},
		{
			2.67 / 3,
			70.0, 170.0, 150.0
		},
		{
			2.8 / 3,
			40.0, 80.0, 255.0
		},
		{
			3.0 / 3,
			240.0, 255.0, 240.0
		}
	};
	norm = pow(norm, 1.5);
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

void placeHorizontalLine(int color, vector<char>& data,
		int y, int width, int height){
	if (width * height * 4 + 54 != data.size()){
		throw exception();
	}
	for (int x = 0; x < width; x++){
		*(int*)(void*)(&(data[(x + width * y) * 4 + 54])) = color;
	}
}

void placeVerticalLine(int color, vector<char>& data,
		int x, int width, int height){
	if (width * height * 4 + 54 != data.size()){
		print(width);
		print(height);
		print(data.size());
		print(width * height * 4);
		throw exception();
	}
	for (int y = 0; y < height; y++){
		*(int*)(void*)((&data[(x + width * y) * 4 + 54])) = color;
	}
}
