#pragma once
#include "Pixel.h"
#include <algorithm>

enum class RemovalType {
	Mean=0,
	Sum=1,
};

enum class DetectionType {
	FAST = 0,
	FAPG = 1,
};

static class NoiseRemoval
{
private:
	static RemovalType removalType;
	static DetectionType detectionType;

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
	static short findImpulsiveness(short*, int);
	static Pixel calculateMean(Pixel*, bool*, int);


	//detecting noise
	static void FAPG(int);
	static void FAST(int);

	//removing noise
	static void sumRemoval(int);
	static void meanRemoval(int);

	static void calculateDistance(Pixel*, short*);

	static short findMax(short*,int);
	static short findMin(short*, int);

	static void extendArray();
	static void shrinkArray();
	static void createNoiseArray();
	static void changePixel(Pixel, int, int);
	static void detectNoise(int);

public:
	static void setData(Pixel**, int, int, int,DetectionType,RemovalType);
	static Pixel** returnArray();
};

