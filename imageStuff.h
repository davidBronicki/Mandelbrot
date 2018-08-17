#pragma once
#include <vector>
#include <fstream>
#include <iostream>
// #include <math.h>

using namespace std;

#define print(input) cout << input << endl

int buildColor(int r, int g, int b, int a){
	int output = a & 255;
	output <<= 8;
	output += r & 255;
	output <<= 8;
	output += g & 255;
	output <<= 8;
	output += b & 255;
	return output;
}

int buildColor(int r, int g, int b){
	return buildColor(r, g, b, 255);
}

int buildColor(int greyScale){
	return buildColor(greyScale, greyScale, greyScale);
}

void pushInt(vector<char>& data, int value){
	data.resize(data.size() + 4);
	*(int*)(void*)&(data[data.size() - 4]) = value;
}

vector<char> buildBitMap(vector<int> colors, int width, int height){
	vector<char> data;

	//BMP header

	data.push_back('B');
	data.push_back('M');//bitmap

	pushInt(data, 54 + 4 * width * height);//file size (int)
	pushInt(data, 0);//unused
	pushInt(data, 54);//offset from start to data start (int)

	//DIB header

	pushInt(data, 40);//bytes in dib header (int)
	pushInt(data, width);//width in pix (int)
	pushInt(data, height);//height in pix (int)

	data.push_back(1);
	data.push_back(0);//color plane count (2 bytes)

	data.push_back(32);
	data.push_back(0);//bits per pix (2 bytes)

	pushInt(data, 0);//no data compression
	pushInt(data, 4 * width * height);//bitmap data size in bytes (int)
	int temp = 0x130b0000;//pixel resolution 2835
	pushInt(data, temp);//horizontal
	pushInt(data, temp);//vertical
	pushInt(data, 0);//zero colors in pallete
	pushInt(data, 0);//all colors are important

	//bitmap data

	for (int i : colors){
		pushInt(data, i);
	}

	return data;
}

