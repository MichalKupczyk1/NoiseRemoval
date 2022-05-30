#pragma once
#include <iostream>
#include <fstream>
#include <math.h>
#include <random>
#include "Pixel.h"

void countStep(int& step, int width);

Pixel* saveToPixelArray(byte* byteArray, int step, int byteAmount, int width);

Pixel** saveTo2DArray(Pixel* pixelArray, int width, int height);

Pixel* ConvertFrom2DArray(Pixel** pixelArray, int width, int height);

void calculateWidthAndHeight(byte* byteData, int& width, int& height);

void shuffleArray(Coordinates* coordArray, int length);

bool** addNoise(Pixel** pixelArray, int width, int height, double noiseLevel);

void pixelToByteArray(byte* byteArray, Pixel* pixelArray, int width, int amount, int step);
