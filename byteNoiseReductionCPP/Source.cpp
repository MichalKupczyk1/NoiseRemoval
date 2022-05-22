#include "NoiseRemoval.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include <random>

#pragma warning(disable:4996) //fopen
//some bitmaps have additional bytes in each row just to make sure that each row can be divided by 4
void countStep(int& step,int width)
{
	if (width % 4 != 0)
		step = (short)(4 - (width % 4));
	else
		step = 0;
}

Pixel* saveToPixelArray(byte* byteArray, int step, int byteAmount,int width) {
	int pixelAmount = ((byteAmount - 57) / 3) + step;
	Pixel* pixelArray = new Pixel[pixelAmount];
	int z = 0;

	for (int i = 0; i < byteAmount - 57;)
	{
		if (step != 0 && (i + 3) % (width) == 0)
		{
			i += step;
			continue;
		}
		pixelArray[z] = Pixel(byteArray[i + 54], byteArray[i + 55], byteArray[i + 56]);
		i += 3;
		z++;
	}

	return pixelArray;
}

Pixel** saveTo2DArray(Pixel* pixelArray, int width, int height) {
	Pixel** twoDimentionalArray = new Pixel*[height];

	int count = 0;
	for (int i = 0; i < height; i++)
		twoDimentionalArray[i] = new Pixel[width];

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			twoDimentionalArray[i][j] = pixelArray[count++];

	return twoDimentionalArray;
}

Pixel* ConvertFrom2DArray(Pixel** pixelArray, int width, int height) {
	Pixel* temp = new Pixel[width * height];
	int z = 0;

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			temp[z++] = pixelArray[i][j];
	return temp;
}


void calculateWidthAndHeight(byte* byteData, int& width, int& height) {
	width = (long)((int)byteData[0] + (256 * (int)byteData[1]) + ((pow(256, 2) * (int)byteData[2])) + ((pow(256, 3) * (int)byteData[3])));
	height = (long)((int)byteData[4] + (256 * (int)byteData[5]) + ((pow(256, 2) * (int)byteData[6])) + ((pow(256, 3) * (int)byteData[7])));
}

void shuffleArray(Coordinates* coordArray, int length) {
	//using <random> generator since default rand() would create a terrible output noise
	std::random_device rd;   
	std::mt19937 gen(rd());  
	std::uniform_int_distribution<> dist(0, length);

	for (int i = 0; i < length; i++)
		std::swap(coordArray[i], coordArray[dist(gen)]);
}

void addNoise(Pixel** pixelArray, int width, int height, double noiseLevel) {
	
	int length = width * height;
	Coordinates* coordArray = new Coordinates[length];
	int z = 0;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			coordArray[z].x = i;
			coordArray[z++].y = j;
		}
	}
	shuffleArray(coordArray,length);

	for (int i = 0; i < length * noiseLevel; i++) {
		pixelArray[coordArray[i].x][coordArray[i].y] = Pixel(rand() % 255, rand() % 255, rand() % 255);
	}
}

void pixelToByteArray(byte* byteArray, Pixel* pixelArray, int width, int amount, int step) {
	int a = 0;
	for (int i = 54; i < amount - 3;)
	{
		byteArray[i] = pixelArray[a].getR();
		byteArray[i + 1] = pixelArray[a].getG();
		byteArray[i + 2] = pixelArray[a].getB();
		i += 3;
		a++;
		if ((i + 3) % width == 0)
		{
			i += step;
			continue;
		}
	}
}

//////////////////////////////////////////////////////////////////////

int NoiseRemoval::height = 0;
int NoiseRemoval::width = 0;
int NoiseRemoval::threshold = 0;
Pixel** NoiseRemoval::pixelArray = nullptr;

void main() {
	//opening a file
	FILE* bitmap = fopen("test.bmp", "rb");
	
	//8 bytes representing width and height of specific bitmap
	byte byteData[8];
	//starting point is the 18th byte
	fseek(bitmap, 18, NULL);
	//reading next 8 bytes, byte-by-byte
	fread(byteData,1,8,bitmap);

	//calculating dimensions, step and the amount of bytes
	int width = 0, height = 0, step = 0;
	calculateWidthAndHeight(byteData, width, height);
	countStep(step, width);
	int byteAmount = 57 + (width *height* 3) + step;
	
	//reading all bytes from a bitmap
	byte* byteArray = new byte[byteAmount];
	fseek(bitmap, 0, NULL);
	fread(byteArray, 1, byteAmount, bitmap);
	fclose(bitmap);

	//creating an array of pixels, 1D and 2D
	int pixelCount = width * height;
	Pixel* pixelArray=saveToPixelArray(byteArray, step, byteAmount, width);	
	Pixel** twoDimentionalArray = saveTo2DArray(pixelArray, width, height);

	//20% of the pixels will be corrupted 
	addNoise(twoDimentionalArray, width, height, 0.2);

	int threshold = 60;

	NoiseRemoval::setData(twoDimentionalArray, width, height, threshold,DetectionType::FAST,RemovalType::Mean);

	twoDimentionalArray = NoiseRemoval::returnArray();

	pixelArray = ConvertFrom2DArray(twoDimentionalArray, width, height);
	pixelToByteArray(byteArray, pixelArray, width, byteAmount, step);

	//writing all bytes to newly created bitmap
	std::ofstream result("result.bmp", std::ios_base::binary);
	result.write((const char*)&byteArray[0], byteAmount);
	result.close();

	for (int i = 0; i < height; i++)
		delete[]twoDimentionalArray[i];
	twoDimentionalArray = nullptr;

	delete[]pixelArray;
	pixelArray = nullptr;


	//additional couts showing calculated values
	std::cout << "Byte amount: " << byteAmount << " Pixel count: " << pixelCount << std::endl;
	std::cout << "Width: " << width << " Height: " << height <<std::endl;
	std::cout << "Step: " << step << std::endl;
}

