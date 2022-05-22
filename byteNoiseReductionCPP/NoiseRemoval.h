#pragma once
#include "Pixel.h"

enum class DetectionType {
	FAST=0,
};

enum class RemovalType {
	Mean = 0,
};

static class NoiseRemoval
{
private:
	static Pixel** pixelArray;
	static int width;
	static int height;
	static int threshold;

	static void extendArray();
	static void shrinkArray();
	static void changePixel(Pixel, int, int);

	static void detectNoise();
	static void removeNoise();

public:
	static void setData(Pixel**, int, int, int, DetectionType, RemovalType);
	static Pixel** returnArray();

};

