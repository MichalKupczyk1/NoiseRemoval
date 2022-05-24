#include "NoiseRemoval.h"
#include "Algorithms.h"

#pragma warning(disable:4996) //fopen
//some bitmaps have additional bytes in each row just to make sure that each row can be divided by 4


//////////////////////////////////////////////////////////////////////
//initializing static fields of NoiseRemoval class

int NoiseRemoval::height = 0;
int NoiseRemoval::width = 0;
int NoiseRemoval::threshold = 0;
Pixel** NoiseRemoval::pixelArray = nullptr;
bool** NoiseRemoval::noiseArray = nullptr;
RemovalType NoiseRemoval::removalType = (RemovalType)0;
DetectionType NoiseRemoval::detectionType = (DetectionType)0;

//////////////////////////////////////////////////////////////////////
//main program

void main() {

	std::string fileName = "test.bmp";

	//opening a file
	FILE* bitmap = fopen(fileName.c_str(), "rb");
	if (bitmap) {
		std::cout << "Opened bitmap " << fileName << std::endl;

		//8 bytes representing width and height of specific bitmap
		byte byteData[8];
		//starting point is the 18th byte
		fseek(bitmap, 18, NULL);
		//reading next 8 bytes, byte-by-byte
		fread(byteData, 1, 8, bitmap);

		//calculating dimensions, step and the amount of bytes
		int width = 0, height = 0, step = 0;
		calculateWidthAndHeight(byteData, width, height);
		countStep(step, width);
		int byteAmount = 57 + (width * height * 3) + step;

		//reading all bytes from a bitmap
		byte* byteArray = new byte[byteAmount];
		fseek(bitmap, 0, NULL);
		fread(byteArray, 1, byteAmount, bitmap);
		fclose(bitmap);

		//creating an array of pixels, 1D and 2D
		int pixelCount = width * height;
		Pixel* pixelArray = saveToPixelArray(byteArray, step, byteAmount, width);
		Pixel** twoDimentionalArray = saveTo2DArray(pixelArray, width, height);

		//20% of the pixels will be corrupted 
		addNoise(twoDimentionalArray, width, height, 0.3);

		int threshold = 60;

		NoiseRemoval::setData(twoDimentionalArray, width, height, threshold, DetectionType::FAPG, RemovalType::Sum);

		twoDimentionalArray = NoiseRemoval::returnArray();

		pixelArray = ConvertFrom2DArray(twoDimentionalArray, width, height);
		pixelToByteArray(byteArray, pixelArray, width, byteAmount, step);

		//writing all bytes to newly created bitmap
		std::ofstream result("result.bmp", std::ios_base::binary);
		result.write((const char*)&byteArray[0], byteAmount);
		result.close();

		//releasing all pointers
		for (int i = 0; i < height; i++)
			delete[]twoDimentionalArray[i];

		delete[]pixelArray;

		//additional couts showing calculated values
		std::cout << "Byte amount: " << byteAmount << " Pixel count: " << pixelCount << std::endl;
		std::cout << "Width: " << width << " Height: " << height << std::endl;
		std::cout << "Step: " << step << std::endl;
	}
	else {
		fclose(bitmap);
		std::cout << "Failed opening " << fileName << std::endl;
	}
}

