#pragma
#include "Rgb2HsvConverter.h"
#include <algorithm>
#include <cmath>  


Rgb2HsvConverter::Rgb2HsvConverter() {};
Rgb2HsvConverter::~Rgb2HsvConverter() {};

HSV Rgb2HsvConverter::ConvertToHSV(double red, double green, double blue) {

	double cmin, cmax, delta;
	HSV output;

	cmin = std::min(std::min(red, green), blue);
	cmax = std::max(std::max(red, green), blue);

	delta = cmax - cmin;

	if (cmax == cmin) {
		output.h = 0;
	}
	else {
		if (cmax == red) {
			output.h = 60.0 * fmod((abs(green - blue)) / delta, 6.0);
		}
		else if (cmax == green) {
			output.h = 60.0 * (abs(blue - red) / delta + 2.0);
		}
		else {
			output.h = 60.0 * (abs(red - blue) / delta + 4.0);
		}
	}

	if (output.h < 0) {
		output.h = 360 + output.h;
	}
	else if (output.h > 360) {
		output.h = - 360 + output.h;
	}

	if (cmax == 0) {
		output.s = 0;
	}
	else {
		output.s = delta / cmax;
	}

	output.v = cmax;
	
	return output;

};

RGB Rgb2HsvConverter::ConvertToRGB(double hue, double sat, double val) {
	double red, grn, blu;
	double i, f, p, q, t;
	RGB result;

	if (val == 0) {
		red = 0;
		grn = 0;
		blu = 0;
	}
	else {
		hue /= 60;
		i = floor(hue);
		f = hue - i;
		p = val * (1 - sat);
		q = val * (1 - (sat * f));
		t = val * (1 - (sat * (1 - f)));
		if (i == 0) { red = val; grn = t; blu = p; }
		else if (i == 1) { red = q; grn = val; blu = p; }
		else if (i == 2) { red = p; grn = val; blu = t; }
		else if (i == 3) { red = p; grn = q; blu = val; }
		else if (i == 4) { red = t; grn = p; blu = val; }
		else if (i == 5) { red = val; grn = p; blu = q; }
	}
	
	result.r = red;
	result.g = grn;
	result.b = blu;
	return result;
}