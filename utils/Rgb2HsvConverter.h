#pragma once
#include "Structs.h"

class Rgb2HsvConverter {
public:
	Rgb2HsvConverter();
	~Rgb2HsvConverter();

	HSV ConvertToHSV(double red, double green, double blue);
	RGB ConvertToRGB(double hue, double sat, double val);

};