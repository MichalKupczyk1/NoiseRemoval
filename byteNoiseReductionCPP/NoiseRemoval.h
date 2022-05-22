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
	static bool** noiseArray;

	static int width;
	static int height;
	static int threshold;

	static void getDifferenceTable(double**,Pixel*);
	static void calculateSum(double*, double**,int);
	static double calculateDifference(Pixel, Pixel);
	static bool isCorrupted(double*,int);
	static int findIndex(double*, int);

	static void extendArray();
	static void shrinkArray();
	static void createNoiseArray();

	static void changePixel(Pixel, int, int);

	static void detectNoise();
	static void removeNoise();

public:
	static void setData(Pixel**, int, int, int, DetectionType, RemovalType);
	static Pixel** returnArray();

};

