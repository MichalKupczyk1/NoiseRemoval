#include "NoiseRemoval.h"
#include <iostream>
Pixel** NoiseRemoval::returnArray() {
	shrinkArray();
	return pixelArray;
}

void NoiseRemoval::setData(Pixel** array, int w, int h, int t, DetectionType detectionType, RemovalType removalType) {
	pixelArray = array;
	width = w;
	height = h;
	threshold = t;

	extendArray();
}

void NoiseRemoval::shrinkArray() {
	Pixel** temp = new Pixel * [height];

	for (int i = 0; i < height; i++)
		temp[i] = new Pixel[width];

	for (int i = 1; i < height + 1; i++)
		for (int j = 1; j < width + 1; j++)
			temp[i - 1][j - 1] = pixelArray[i][j];

	for (int i = 0; i < height; i++)
		delete[]pixelArray[i];
	pixelArray = nullptr;
	pixelArray = temp;
}

void NoiseRemoval::extendArray()
{
	Pixel** temp = new Pixel*[height + 2];
	for (int i = 0; i < height + 2;i++)
		temp[i] = new Pixel[width + 2];
	
	std::cout << width<<" "<<height << std::endl;
	for (int i = 0; i < height; i++)
	{
		temp[i][0] = pixelArray[i][0];
		temp[i][width + 1] = pixelArray[i][width - 1];
	}

	for (int i = 0; i < width; i++)
	{
		temp[0][i] = pixelArray[0][i];
		temp[height+1][i] = pixelArray[height-1][i];
	}

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			temp[i + 1][j + 1] = pixelArray[i][j];
	
	for(int i=0;i<height;i++)
		delete[]pixelArray[i];
	pixelArray = nullptr;
	pixelArray = temp;
}

void NoiseRemoval::detectNoise()
{





}

void NoiseRemoval::removeNoise()
{
}
