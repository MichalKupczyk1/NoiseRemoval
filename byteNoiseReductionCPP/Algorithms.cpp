#include "Algorithms.h"

void countStep(int& step, int width)
{
	if (width % 4 != 0)
		step = (short)(4 - (width % 4));
	else
		step = 0;
}

Pixel* saveToPixelArray(byte* byteArray, int step, int byteAmount, int width) {
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
	Pixel** twoDimentionalArray = new Pixel * [height];

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

bool** addNoise(Pixel** pixelArray, int width, int height, double noiseLevel) {
	bool** noiseArray = new bool* [height];
	for (int i = 0; i < height; i++)
		noiseArray[i] = new bool[width] {1};

	int length = width * height;
	Coordinates* coordArray = new Coordinates[length];
	int z = 0;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			coordArray[z].x = i;
			coordArray[z].y = j;
			z++;
		}
	}
	shuffleArray(coordArray, length);

	for (int i = 0; i < length * noiseLevel; i++) {
		pixelArray[coordArray[i].x][coordArray[i].y] = Pixel(rand() % 255, rand() % 255, rand() % 255);
		noiseArray[coordArray[i].x][coordArray[i].y] = false;
	}
	for (int i = length * noiseLevel; i < length; i++)
		noiseArray[coordArray[i].x][coordArray[i].y] = true;
	return noiseArray;
}

void pixelToByteArray(byte* byteArray, Pixel* pixelArray, int width, int amount, int step) {
	int a = 0;
	//skipping header info which is always the same as in the original image
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