#include "NoiseRemoval.h"
#include <iostream>
Pixel** NoiseRemoval::returnArray() {
	createNoiseArray();
	int windowSize = 9;

	detectNoise(windowSize);
	shrinkArray();
	return pixelArray;
}

void NoiseRemoval::setData(Pixel** array, int w, int h, int t,DetectionType detection,RemovalType removal) {
	pixelArray = array;
	width = w;
	height = h;
	threshold = t;
	detectionType = detection;
	removalType = removal;
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
	noiseArray = new bool* [height];
	for (int i = 0; i < height; i++)
		noiseArray[i] = new bool[width];

	Pixel** temp = new Pixel*[height + 2];
	for (int i = 0; i < height + 2;i++)
		temp[i] = new Pixel[width + 2];
	
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

//detection algorithms
void NoiseRemoval::detectNoise(int windowSize)
{
	if(detectionType==DetectionType::FAST)
		FAST(windowSize);
	if (detectionType == DetectionType::FAPG)
		FAPG(windowSize);
}

void NoiseRemoval::FAPG(int windowSize) {
	Pixel* tempPixelArray = new Pixel[windowSize];
	int index = 0;
	double* sum = new double[windowSize];

	for (int i = 0; i < windowSize; i++)
		sum[i] = 0;

	double** differenceArray = new double* [windowSize];
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
			//returns true if pixel is corrupted, substracting one from indexes because bool array wasn't extended
			noiseArray[i-1][j-1] = isCorrupted(sum, windowSize);

			index = 0;
		}
	}
	//releasing memory
	for (int i = 0; i < windowSize; i++) {
		delete[]differenceArray[i];
	}
	delete[]differenceArray;
	delete[]tempPixelArray;
	delete[]sum;

	sumRemoval(windowSize);
}

void NoiseRemoval::FAST(int windowSize) {
	int index = 0;
	Pixel* tempPixelArray = new Pixel[windowSize];
	short* difference = new short[windowSize];

	short** impulsivenessCalculation = new short* [height+2];
	for (int i = 0; i < height + 2; i++)
		impulsivenessCalculation[i] = new short[width + 2] {};

	short** impulsivenessData = new short* [height + 2];
	for (int i = 0; i < height + 2; i++)
		impulsivenessData[i] = new short[width + 2] {};

	for (int i = 1; i < height + 1; i++) {
		for (int j = 1; j < width + 1; j++) {
			for (int k = -1; k < 2; k++)
			{
				for (int l = -1; l < 2; l++)
				{
					tempPixelArray[index++] = pixelArray[i + l][j + k];
				}
			}
			calculateDistance(tempPixelArray, difference);
			impulsivenessData[i][j] = findMin(difference, windowSize);
			impulsivenessCalculation[i][j] = impulsivenessData[i][j];
			index = 0;
		}
	}

	short* minImpulsiveness = new short[windowSize] {};
	short substraction = 0;
	int counter = 0;
	int goodPixelcounter = 0;
	for (int i = 1; i < height; i++) {
		for (int j = 1; j < width; j++) {
			for (int k = -1; k < 2; k++)
			{
				for (int l = -1; l < 2; l++)
				{
					minImpulsiveness[counter++] = impulsivenessData[i+l][j + k];
				}
			}
			substraction = findImpulsiveness(minImpulsiveness, windowSize);
			impulsivenessCalculation[i][j] = (short)(impulsivenessData[i][j] - substraction);
		
			noiseArray[i - 1][j - 1] = impulsivenessCalculation[i][j] < threshold;
			if (noiseArray[i - 1][j - 1])
				goodPixelcounter++;

			counter = 0;
		}
	}

	for (int i = 0; i < height+2; i++) {
		delete[]impulsivenessCalculation[i];
		delete[]impulsivenessData[i];
	}
	delete[]impulsivenessCalculation;
	delete[]impulsivenessData;
	delete[]tempPixelArray;
	delete[]minImpulsiveness;

	meanRemoval(windowSize);
}

//additional methods

short NoiseRemoval::findImpulsiveness(short* array, int amount) {
	short temp = array[0];
	for (int i = 1; i < amount; i++)
	{
		if (i == 4)
			continue;
		if (array[i] < temp)
			temp = array[i];
	}
	return temp;
}

short NoiseRemoval::findMax(short* array, int amount) {
	short min = array[0];

	for (int i = 0; i < amount; i++)
		if (array[i] < min)min = array[i];

	return min;
}

short NoiseRemoval::findMin(short* array, int amount) {
	short firstMin = SHRT_MAX, secondMin = SHRT_MAX;

	for (int i = 0; i < amount; i++)
	{
		if (i == 4)
			continue;
		if (array[i] < firstMin)
		{
			secondMin = firstMin;
			firstMin = array[i];
		}
		if (array[i] < secondMin)
		{
			secondMin = array[i];
		}
	}
	return (short)(firstMin + secondMin);
}

void NoiseRemoval::calculateDistance(Pixel*pixelArray,short*differenceArray) {
	short distances[3];

	for (int i = 0; i < 9; i++)
	{
		if (i == 4)
			continue;
		distances[0] = abs(pixelArray[4].getR() - pixelArray[i].getR());
		distances[1] = abs(pixelArray[4].getG() - pixelArray[i].getG());
		distances[2] = abs(pixelArray[4].getB() - pixelArray[i].getB());

		differenceArray[i] = findMax(distances, 3);
	}
	differenceArray[4] = 0;
}

Pixel NoiseRemoval::calculateMean(Pixel* tempPixelArray, bool* goodPixelArray, int windowSize) {
	int r = 0;
	int g = 0;
	int b = 0;

	int amount = 0;
	for (int i = 0; i < 9; i++)
	{
		if (i == 4)
			continue;
		if (goodPixelArray[i])
		{
			r += tempPixelArray[i].getR();
			g += tempPixelArray[i].getG();
			b += tempPixelArray[i].getB();
			amount++;
		}
	}
	if (amount > 3)
		return Pixel((byte)(b / amount), (byte)(g / amount), (byte)(r / amount));
	else
		return tempPixelArray[4];
}
//removal algorithms

void NoiseRemoval::meanRemoval(int windowSize) {
	int index = 0;
	Pixel*tempPixelArray = new Pixel[windowSize];
	bool* goodPixelArray = new bool[windowSize];
	for (int i = 1; i < height - 1; i++)
	{
		for (int j = 1; j < width - 1; j++)
		{
			if (!noiseArray[i][j]) {
				for (int k = -1; k < 2; k++)
				{
					for (int l = -1; l < 2; l++)
					{
						tempPixelArray[index] = pixelArray[i + l][j + k];
						goodPixelArray[index] = noiseArray[i + l][j + k];
						index++;
					}
				}
				changePixel(calculateMean(tempPixelArray, goodPixelArray, windowSize), i, j);
				index = 0;
			}
		}
	}		
	delete[]tempPixelArray;
	delete[]goodPixelArray;
}

void NoiseRemoval::sumRemoval(int windowSize)
{
	Pixel* tempPixelArray = new Pixel[windowSize];
	int index = 0;
	double* sum = new double[windowSize];

	for (int i = 0; i < windowSize; i++)
		sum[i] = 0;

	double** differenceArray = new double* [windowSize];
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

			index = 0;
		}
	}
	//releasing memory
	for (int i = 0; i < windowSize; i++) {
		delete[]differenceArray[i];
	}
	delete[]differenceArray;
	delete[]tempPixelArray;
	delete[]sum;
}
