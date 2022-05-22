#pragma once
#include <Windows.h>
#include <string>
class Pixel
{
private:
	byte R = 0;
	byte G = 0;
	byte B = 0;
public:
	Pixel(){}
	Pixel(byte R, byte G, byte B) {
		this->R = R;
		this->G = G;
		this->B = B;
	};

	int getR() { return this->R; };
	int getG() { return this->G; };
	int getB() { return this->B; };

	void setR(byte R) { this->R = R; };
	void setG(byte G) { this->G = G; };
	void setB(byte B) { this->B = B; };
};


struct Coordinates {
	int x, y;
};