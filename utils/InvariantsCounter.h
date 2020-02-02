#pragma once
#include "Structs.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <math.h>

class InvariantsCounter {
	public:
		InvariantsCounter();
		static Invariants GetInvariants(std::string fileName, uint objectId);
		static Invariants GetInvariants(cv::Mat& _Image, uint objectId);

	private:
		static bool CheckIfPixelIsNonBlackAndBelongsToObject(cv::Mat_<cv::Vec3b>& _Image, int i, int j, uint objectId);
		static double CountmNM(cv::Mat_<cv::Vec3b>& Image, int p, int q, uint objectId);
};
