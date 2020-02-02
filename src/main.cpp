/*
@@@*********************************************************************************************@@@
@*************************************************************************************************@
***************************************************************************************************
***************************************************************************************************
***************************************************************************************************
***************************************************************************************************
********************************************@*******@**********************************************
********************************************(@*****@@******@***************************************
**************************************@@****@@@***@@@*****@@***************************************
**************************************@@@***@@@**@@@@***@@@******@*********************************
********************************@@/***@@@@*@@@@@@@@@@#@@@@@***@@@**********************************
*********************************@@@@*(@@@@@@&*******@@@@@@@@@@@*******#***************************
**********************************@@@@@@@*****@@@@@@@*****@@@@@*****@@@****************************
****************************@@@@***@@@***#@@@@@@@@@@@@@@@****@@@@@@@@******************************
******************************@@@@@@***@@@@@@@@@@@@@@@@@@@@@**@@@@/********************************
*******************************@@@@**@@@@@@@@@@@@@@@@@@@@@@@@***@@@@@@@@@@*************************
************************@@@@@@@@@@**@@@@@@@@@@@@@@@@@@@@@@@@@@***@@@@@@@***************************
****************************@@@@@(**@@@@@@@@@@@@@@@@@@@@@@@@@@@**@@********************************
*******************************@@**@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*(@@@@@@***************************
*************************@@@@@@@@**@@@@@@@@@@@@@@@@@@@@@@@@@@@@@**@@@@@@@@*************************
***************************@@@@@@**@@@@@@@@@@@@@@@@@@@@@@@@@@@@%**@@/******************************
*******************************@@@**@@@@@@@@@@@@@@@@@@@@@@@@@@@**@@@@@@****************************
***************************@@@@@@@**&@@@@@@@@@@@@@@@@@@@@@@@@@**#@@#@@@@@@@#***********************
************************@@@@@@@@@@@***@@@@@@@@@@@@@@@@@@@@@@@**(@@@@*******************************
********************************@@@@@**@@@@@@@@@@@@@@@@@@@@***@@@@@@@******************************
******************************@@@@@@@@/***@@@@@@@@@@@@@@@***@@@@***@@@@****************************
****************************@@@*****@@@@@******#@@&(******@@@@@@@**********************************
**********************************(@@@@@@@@@@@@****(@@@@@@@@(*@@@@*********************************
*********************************@@@@***@@@@@*@@@@@@@@@@*@@@@****@@********************************
*********************************@******@@@/**@@@@**@@@***@@@**************************************
***************************************@@*****@@@***@@@****@@**************************************
***************************************@******@@*****@*********************************************
**********************************************%*******@********************************************
***************************************************************************************************
***************************************************************************************************
@**************************************************************************************************
@@*************************************************************************************************
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
%%%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%@@@@@@@@@@@%%%%%@@@@@@@@@%%%%%@@%%%%%%%%%%&@@@@@@@@@@%%%%%%@@@@@&%%%@@@@@@@@@@@%%%%%%%%%
%%%%%%%%%%%@@%%%%%%%%@@%%%@@%%%%%%%%@@%%%@@%%%%%%%%%@@%%%%%%%%%%%%%%@@%%%%%@@%%%%%%@@&%%%%%%%%%%%%%
%%%%%%%%%%%@@%%%%%%%%@@%%%@@%%%%%%%%@@%%%@@%%%%%%%%%@@@@@@@@@@%%%%%%@@%%%%%@@%%%%%%@@&%%%%%%%%%%%%%
%%%%%%%%%%%@@%%@@@@@@@%%%%@@%%%%%%%%@@%%%@@%%%%%%%%%%%%%%%%%%%@@%%%@@%%@@@@@@@%%%%%@@%%%%%%%%%%%%%%
%%%%%%%%%%%@@%%%%%%%%%%%%%@@@%%%%%&@@@%%%@@@%%%%%%%%%%%%%%%%&@@@%%%@@%%%%%%%@@%%%%%@@%%%%%%%%%%%%%%
%%%%%%%%%%%@@%%%%%%%%%%%%%%%@@@@@@@%%%%%%@@@@@@@@@%%%%@@@@@@@@@%%%%@@%%%%%%%%@@%%%%@@%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@
*/


#include <iostream>
#include <math.h>
#include "logoDetector.h"
#include <thread>
#include <chrono>


const double pi = 3.14159;

cv::Mat& perform(cv::Mat& I) {
	CV_Assert(I.depth() != sizeof(uchar));
	switch (I.channels()) {
	case 1:
		for (int i = 0; i < I.rows; ++i)
			for (int j = 0; j < I.cols; ++j)
				I.at<uchar>(i, j) = (I.at<uchar>(i, j) / 32) * 32;
		break;
	case 3:
		cv::Mat_<cv::Vec3b> _I = I;
		for (int i = 0; i < I.rows; ++i)
			for (int j = 0; j < I.cols; ++j) {
				_I(i, j)[0] = (_I(i, j)[0] / 32) * 32;
				_I(i, j)[1] = (_I(i, j)[1] / 32) * 32;
				_I(i, j)[2] = (_I(i, j)[2] / 32) * 32;
			}
		I = _I;
		break;
	}
	return I;
}



int main(int, char* []) {
	std::cout << "Start ..." << std::endl;
	
	LogoDetector logoDetector = LogoDetector();

	//example of invalid picture
	//logoDetector.DetectObject("images/test.png");

	logoDetector.DetectObject("images/logo_billboard.png");

	logoDetector.DetectObject("images/logo_skosne.png");
	
	logoDetector.DetectObject("images/logo_background.png");
	
	cv::waitKey(-1);
	return 0;
}
