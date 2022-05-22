#include "NoiseRemoval.h"
#include <iostream>
Pixel** NoiseRemoval::returnArray() {
	detectNoise();
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

void NoiseRemoval::changePixel(Pixel pixel, int x, int y) {
	pixelArray[x][y] = pixel;
}

void NoiseRemoval::createNoiseArray() {
	noiseArray = new bool* [height];

	for (int i = 0; i < height; i++)
		noiseArray[i] = new bool[width];
}

double NoiseRemoval::calculateDifference(Pixel a, Pixel b) {
	double R = a.getR() - b.getR();
	double G = a.getG()- b.getG();
	double B = a.getB() - b.getB();

	return sqrt(R * R + G * G + B * B);
}

void NoiseRemoval::getDifferenceTable(double**difference,Pixel* array) {
	for (int k = 0; k < 9; k++)
	{
		for (int l = k; l < 9; l++)
		{
			difference[k][l] = calculateDifference(array[k], array[l]);
			difference[l][k] = difference[k][l];
		}
	}
}

void NoiseRemoval::calculateSum(double* sumArray, double** difference, int windowSize) {
	int it = 0;
	for (int i = 0; i < windowSize; i++)
		sumArray[i] = 0;

	for (int i = 0; i < windowSize; i++)
	{
		for (int j = 0; j < windowSize; j++)
		{
			sumArray[it] += difference[i][j];
		}
		it++;
	}
}

bool NoiseRemoval::isCorrupted(double* sumArray,int windowSize) {
	int goodPixels = 0;
	
	for (int i = 0; i < windowSize; i++)
	{
		//std::cout << sumArray[i] << std::endl;
		if (i == 4)
			continue;
		if (sumArray[i] < threshold)
			goodPixels++;
	}
	return goodPixels < 3;
}

int NoiseRemoval::findIndex(double* sumArray, int windowSize) {
	int minIndex = 0;
	for (int k = 0; k < 9; k++)
		if (sumArray[k] < sumArray[minIndex]) minIndex = k;
	return minIndex;
}

void NoiseRemoval::detectNoise()
{
	createNoiseArray();
	const int windowSize = 9;
	Pixel tempPixelArray[windowSize];
	int index = 0;
	
	double sum[windowSize];
	for (int i = 0; i < windowSize; i++)
		sum[i] = 0;

	double** differenceArray = new double*[windowSize];
	for (int i = 0; i < windowSize; i++)
		differenceArray[i] = new double[windowSize];

	for (int i = 1; i < height + 1; i++) {
		for (int j = 1; j < width + 1; j++) {
			for (int k = -1; k < 2; k++)
			{
				for (int l = -1; l < 2; l++)
				{
					tempPixelArray[index++] = pixelArray[i + l][j + k];
				}
			}
			//passing differenceArray through reference to save results over there
			getDifferenceTable(differenceArray, tempPixelArray);
			//calculating sum
			calculateSum(sum, differenceArray, windowSize);
			//returns true if pixel is corrupted
			if (isCorrupted(sum, windowSize))
				changePixel(tempPixelArray[findIndex(sum, windowSize)], i, j);
			//temporairly removing noise inside of this loop
			index = 0;
		}
	}

	for (int i = 0; i < windowSize; i++) {
		delete[]differenceArray[i];
	}
	delete[]differenceArray;
}

void NoiseRemoval::removeNoise()
{
}
