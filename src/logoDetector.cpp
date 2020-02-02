#include "logoDetector.h"
#include "..//utils/Rgb2HsvConverter.h"
#include "..//utils/HsvColorsBoundaries.h"
#include <functional>
#include <iostream>
#include <stack>
#include <queue>
#include "../utils/InvariantsCounter.h"
#include "../utils/InvariantsBoundaries.h"

LogoDetector::LogoDetector() {}
LogoDetector::~LogoDetector() {}


bool LogoDetector::DetectObject(std::string fileName) {

	this->InitializeImageDataTable(fileName);

	cv::imshow(fileName + " - input", this->imageRGB);

	this->RunPreProcessImage();
	this->CreateImageHSV();
	this->CheckRGB();
	this->FindConsistentAreas();
	//this->CheckRGB();

	this->ResearchOrangeAreas(this->imageOrangeHSV);
	this->ResearchRedAreas(this->imageRedHSV);

	this->MatchRedRectsWithOrangeRects();
	this->CreateMaskImageForFinalResult();
	this->DrawFinalResult();

	cv::imshow(fileName + " - output", this->imageRGB);
	cv::imwrite(fileName + "_output", this->imageRGB);

	

	return false;
}

void LogoDetector::InitializeImageDataTable(std::string fileName) {

	this->fileName = fileName;
	this->imageRGB = cv::imread(fileName);
	this->imageHSV = cv::imread(fileName);
	this->columnsNumber = imageRGB.cols;
	this->rowsNumber = imageRGB.rows;

	this->pixelsToObjectsTable = new int* [this->rowsNumber];

	for (int i = 0; i < this->rowsNumber; i++) {
		this->pixelsToObjectsTable[i] = new int[this->columnsNumber];
	}

	for (int i = 0; i < 50; i++) {
		this->biggestRedColorAreasIds[i] = 0;
		this->biggestOrangeColorAreasIds[i] = 0;
		this->biggestWhiteColorAreasIds[i] = 0;
	}

	this->areasList.clear();
	this->validRedAreasIds.clear();
	this->validOrangeAreasIds.clear();
	this->validWhiteAreasIds.clear();
	
}

void LogoDetector::RunPreProcessImage() {
	this->ApplyMedianFilter(2);
	//cv::imshow("medianfilter", this->imageRGB);
}

void LogoDetector::LogoDetector::CreateImageHSV() {

	Rgb2HsvConverter converter = Rgb2HsvConverter();
	cv::Mat_<cv::Vec3b> _imageHSV = this->imageRGB.clone();


	for (int i = 0; i < this->rowsNumber; i++) {
		for (int j = 0; j < this->columnsNumber; j++) {
			double a , b, c;
			a = (double)_imageHSV(i, j)[2];
			b = (double)_imageHSV(i, j)[1];
			c = (double)_imageHSV(i, j)[0];
			HSV hsv = converter.ConvertToHSV(((double)_imageHSV(i,j)[2])/255.0, ((double)_imageHSV(i, j)[1]) / 255.0, ((double)_imageHSV(i, j)[0]) / 255.0);
			_imageHSV(i, j)[0] = (int)hsv.h;
			_imageHSV(i, j)[1] = (int)(hsv.s * 100.0);
			_imageHSV(i, j)[2] = (int)(hsv.v * 100.0);
		}
	}

	this->imageHSV = _imageHSV;

}

void LogoDetector::LogoDetector::CheckRGB() {

	Rgb2HsvConverter converter = Rgb2HsvConverter();
	cv::Mat_<cv::Vec3b> _imageHSV = this->imageHSV.clone();



	for (int i = 0; i < this->rowsNumber; i++) {
		for (int j = 0; j < this->columnsNumber; j++) {
			RGB rgb = converter.ConvertToRGB(((double)_imageHSV(i, j)[0]), ((double)_imageHSV(i, j)[1]) / 100, ((double)_imageHSV(i, j)[2]) / 100.0);
			_imageHSV(i, j)[0] = (int)(rgb.b * 255.0);
			_imageHSV(i, j)[1] = (int)(rgb.g * 255.0);
			_imageHSV(i, j)[2] = (int)(rgb.r * 255.0);
		}
	}

	cv::Mat temp = _imageHSV;

}

void LogoDetector::FindConsistentAreas() {

	//cv::Mat orangeAreas = cv::imread(this->fileName);
	this->imageRedHSV = this->imageHSV.clone();
	this->imageOrangeHSV = this->imageHSV.clone();
	this->imageWhiteHSV = this->imageHSV.clone();


	this->FindAreasByColor(Red);
	this->FindAreasByColor(Orange);
	this->FindAreasByColor(White);

	this->SearchBiggestAreasWithColor(Red);
	this->SearchBiggestAreasWithColor(Orange);
	this->SearchBiggestAreasWithColor(White);
	
};

void LogoDetector::ResearchOrangeAreas(cv::Mat& image) {

	for (int i = 0; i < 5; i++) {
		Invariants invariants = InvariantsCounter::GetInvariants(this->imageOrangeHSV, biggestOrangeColorAreasIds[i]);
		if (this->IsObjectOrangeRectangle(invariants)) {
			std::cout << "We got orange rectangle!!!" << std::endl;


			MainObject newOne;
			newOne.id = biggestRedColorAreasIds[i];
			newOne.color = Color::Orange;
			newOne.objectType = ObjectType::OrangeRectangle;
			newOne.hasSunCircle = false;
			newOne.hasSunRing = false;
			newOne.hasPLetter = false;
			newOne.hasOLetter = false;
			newOne.hasLLetter = false;
			newOne.hasSLetter = false;
			newOne.hasALetter = false;
			newOne.hasTLetter = false;
			this->areasList.push_back(newOne);


			this->FindSubobjectsInOrangeRectangle(this->areasList.size() - 1, invariants);
		}
	}
}

void LogoDetector::ResearchRedAreas(cv::Mat& image) {

	for (int i = 0; i < 5; i++) {
		Invariants invariants = InvariantsCounter::GetInvariants(this->imageRedHSV, biggestRedColorAreasIds[i]);
		if (this->IsObjectRedRectangle(invariants)) {
			std::cout << "We got red rectangle!!!" << std::endl;

			MainObject newOne;
			newOne.id = biggestRedColorAreasIds[i];
			newOne.color = Color::Red;
			newOne.objectType = ObjectType::RedRectangle;
			newOne.hasSunCircle = false;
			newOne.hasSunRing = false;
			newOne.hasPLetter = false;
			newOne.hasOLetter = false;
			newOne.hasLLetter = false;
			newOne.hasSLetter = false;
			newOne.hasALetter = false;
			newOne.hasTLetter = false;
			this->areasList.push_back(newOne);

			this->FindSubobjectsInRedRectangle(this->areasList.size() - 1, invariants);

		}
	}
}

void  LogoDetector::MatchRedRectsWithOrangeRects() {
	int areasSize = this->areasList.size();
	for (int i = 0; i < areasSize; i++) {

		if (this->areasList[i].color != Red) continue;

		for (int j = 0; j < areasSize; j++) {
			if (this->areasList[j].color != Orange) continue;
			if ((std::sqrt(std::pow(areasList[i].geometricCenterX - areasList[j].geometricCenterX, 2) +
				std::pow(areasList[i].geometricCenterY - areasList[j].geometricCenterY, 2)))
				< 1.5 * (areasList[i].halfDiagonal + areasList[j].halfDiagonal)) 
			{
				areasList[i].secondMainObjectReferenceId = areasList[j].id;
				areasList[j].secondMainObjectReferenceId = areasList[i].id;
			}
		}
	}
}

void LogoDetector::CreateMaskImageForFinalResult() {

	cv::Mat_<cv::Vec3b> _maskImage = this->imageRGB.clone();

	for (int i = 0; i < imageRGB.rows; i++){
		for (int j = 0; j < imageRGB.cols; j++) {
			_maskImage(i, j)[0] = 0;
			_maskImage(i, j)[1] = 0;
			_maskImage(i, j)[2] = 0;
		}
	}

	this->imageMask = _maskImage;
	int areasSize = this->areasList.size();
	bool isValid = false;

	for (int i = 0; i < areasSize; i++) {
		if (this->areasList[i].secondMainObjectReferenceId != -1) {

			if (this->areasList[i].color == Red) {
				this->validRedAreasIds.insert(this->areasList[i].id);
				isValid = this->areasList[i].hasPLetter && this->areasList[i].hasOLetter && this->areasList[i].hasLLetter
					&& this->areasList[i].hasSLetter && this->areasList[i].hasALetter && this->areasList[i].hasTLetter;
			}
			else {
				this->validOrangeAreasIds.insert(this->areasList[i].id);
				isValid = this->areasList[i].hasSunCircle && this->areasList[i].hasSunRing;
			}
		}

		if (isValid) {
			this->validWhiteAreasIds.insert(this->areasList[i].subobjectIds.begin(), this->areasList[i].subobjectIds.end());
		}
	
	}

	std::set<int>::iterator it;
	for (it = this->validRedAreasIds.begin(); it != this->validRedAreasIds.end(); ++it)
	{
		this->AddSpecificAreaToMask(Red, *it);
	}

	for (it = this->validOrangeAreasIds.begin(); it != this->validOrangeAreasIds.end(); ++it)
	{
		this->AddSpecificAreaToMask(Orange, *it);
	}

	for (it = this->validWhiteAreasIds.begin(); it != this->validWhiteAreasIds.end(); ++it)
	{
		this->AddSpecificAreaToMask(White, *it);
	}
	
}

void LogoDetector::AddSpecificAreaToMask(Color color, int objectId) {
	cv::Mat_<cv::Vec3b> _tempImage;
	cv::Mat_<cv::Vec3b> _maskImage = this->imageMask;

	switch (color) {
		case Orange:
		{
			_tempImage = this->imageOrangeHSV;
			break;
		}
		case Red:
		{
			_tempImage = this->imageRedHSV;
			break;
		}
		case White:
		{
			_tempImage = this->imageWhiteHSV;
			break;
		}
	}

	for (int i = 0; i < imageRGB.rows; i++) {
		for (int j = 0; j < imageRGB.cols; j++) {
			if((_tempImage(i, j)[1] == objectId) || (objectId > 255 && (_tempImage(i, j)[1] + 256) == objectId))
				_maskImage(i, j)[0] = 255;
		}
	}

	this->imageMask = _maskImage;

}

void LogoDetector::DrawFinalResult() {

	cv::Mat_<cv::Vec3b> _tempImage = this->imageRGB;
	cv::Mat_<cv::Vec3b> _maskImage = this->imageMask;

	for (int i = 0; i < imageRGB.rows; i++) {
		for (int j = 0; j < imageRGB.cols; j++) {
			if (_maskImage(i, j)[0] != 255) {
				_tempImage(i, j)[0] = 255;
				_tempImage(i, j)[1] = 255;
				_tempImage(i, j)[2] = 255;
			}
		}
	}
	this->imageRGB = _tempImage;
	this->DrawMainObjectsBoundaries();
}

void LogoDetector::FindSubobjectsInOrangeRectangle(uint objectId, Invariants mainObjectInvariants) {
	for (int i = 0; i < 50; i++) {

		Invariants invariants = InvariantsCounter::GetInvariants(this->imageWhiteHSV, this->biggestWhiteColorAreasIds[i]);

		if (invariants.S < 50) continue;

		if (this->AreSubobjectConstraintsFullfilled(invariants, mainObjectInvariants)) {

			if (this->IsObjectSunCircle(invariants)) {
				std::cout << "We got sun circle!!!" << std::endl;
				this->areasList[objectId].hasSunCircle = true;
				this->areasList[objectId].subobjectIds.insert(this->biggestWhiteColorAreasIds[i]);
				//continue;
			}

			if (this->IsObjectSunRing(invariants)) {
				std::cout << "We got sun ring!!!" << std::endl;
				this->areasList[objectId].hasSunRing = true;
				this->areasList[objectId].subobjectIds.insert(this->biggestWhiteColorAreasIds[i]);
				//continue;
			}
		}
	}

	if (!(this->areasList[objectId].hasSunCircle && this->areasList[objectId].hasSunRing)) this->areasList.pop_back();
	else {
		this->areasList[objectId].geometricCenterX = (mainObjectInvariants.minX + mainObjectInvariants.maxX) / 2;
		this->areasList[objectId].geometricCenterY = (mainObjectInvariants.minY + mainObjectInvariants.maxY) / 2;
		this->areasList[objectId].halfDiagonal = std::sqrt(
			std::pow(mainObjectInvariants.maxY - this->areasList[objectId].geometricCenterY, 2) +
			std::pow(mainObjectInvariants.maxX - this->areasList[objectId].geometricCenterX, 2)) / 2;
	}

}

void LogoDetector::FindSubobjectsInRedRectangle(uint objectId, Invariants mainObjectInvariants) {
	for (int i = 0; i < 50; i++) {

		if (this->biggestWhiteColorAreasIds[i] > 293 && this->biggestWhiteColorAreasIds[i] < 307) {
			std::cout << "wait" << std::endl;
		}

		Invariants invariants = InvariantsCounter::GetInvariants(this->imageWhiteHSV, this->biggestWhiteColorAreasIds[i]);

		if (invariants.S < 50) continue;

		if (this->AreSubobjectConstraintsFullfilled(invariants, mainObjectInvariants)) {
		
			if (this->IsObjectPLetter(invariants)) {
				std::cout << "We got P letter!!!" << std::endl;
				this->areasList[objectId].hasPLetter = true;
				this->areasList[objectId].subobjectIds.insert(this->biggestWhiteColorAreasIds[i]);
			}

			if (this->IsObjectOLetter(invariants)) {
				std::cout << "We got O letter!!!" << std::endl;
				this->areasList[objectId].hasOLetter = true;
				this->areasList[objectId].subobjectIds.insert(this->biggestWhiteColorAreasIds[i]);
			}

			if (this->IsObjectLLetter(invariants)) {
				std::cout << "We got L letter!!!" << std::endl;
				this->areasList[objectId].hasLLetter = true;
				this->areasList[objectId].subobjectIds.insert(this->biggestWhiteColorAreasIds[i]);
			}

			if (this->IsObjectSLetter(invariants)) {
				std::cout << "We got S letter!!!" << std::endl;
				this->areasList[objectId].hasSLetter = true;
				this->areasList[objectId].subobjectIds.insert(this->biggestWhiteColorAreasIds[i]);
			}

			if (this->IsObjectALetter(invariants)) {
				std::cout << "We got A letter!!!" << std::endl;
				this->areasList[objectId].hasALetter = true;
				this->areasList[objectId].subobjectIds.insert(this->biggestWhiteColorAreasIds[i]);
			}

			if (this->IsObjectTLetter(invariants)) {
				std::cout << "We got T letter!!!" << std::endl;
				this->areasList[objectId].hasTLetter = true;
				this->areasList[objectId].subobjectIds.insert(this->biggestWhiteColorAreasIds[i]);
			}
		}
	}

	if (!(this->areasList[objectId].hasPLetter && this->areasList[objectId].hasOLetter
		&& this->areasList[objectId].hasLLetter && this->areasList[objectId].hasSLetter
		&& this->areasList[objectId].hasALetter && this->areasList[objectId].hasTLetter
		)) this->areasList.pop_back();
	else {
		this->areasList[objectId].geometricCenterX = (mainObjectInvariants.minX + mainObjectInvariants.maxX) / 2;
		this->areasList[objectId].geometricCenterY = (mainObjectInvariants.minY + mainObjectInvariants.maxY) / 2;
		this->areasList[objectId].halfDiagonal = std::sqrt(
			std::pow(mainObjectInvariants.maxY - this->areasList[objectId].geometricCenterY, 2) +
			std::pow(mainObjectInvariants.maxX - this->areasList[objectId].geometricCenterX, 2)) / 2;
	}

}

#pragma optimize( "", off )
bool LogoDetector::AreSubobjectConstraintsFullfilled(Invariants subobjectInvariants, Invariants mainObjectInvariants) {

	if (subobjectInvariants.S > mainObjectInvariants.S || subobjectInvariants.maxX > mainObjectInvariants.maxX 
		|| subobjectInvariants.maxY > mainObjectInvariants.maxY || subobjectInvariants.minX < mainObjectInvariants.minX
		|| subobjectInvariants.minY < mainObjectInvariants.minY)
	{
		return false;
	}
	
	return true;
}
#pragma optimize( "", on )

void LogoDetector::FindAreasByColor(Color color) {

	cv::Mat_<cv::Vec3b> _imageHSV = this->imageHSV.clone();
	std::function<bool(double, double, double)> condition;

	switch (color) {
		case Red: {
			condition = std::bind(&LogoDetector::isPixelRed, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			_imageHSV = this->imageRedHSV;
			break;
		}
		case Orange: {
			condition = std::bind(&LogoDetector::isPixelOrange, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			_imageHSV = this->imageOrangeHSV;
			break;
		}
		case White: {
			condition = std::bind(&LogoDetector::isPixelWhite, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			_imageHSV = this->imageWhiteHSV;
			break;
		}
		default: {
			return;
		}
	}
	for (int i = 0; i < this->rowsNumber; i++) {
		for (int j = 0; j < this->columnsNumber; j++) {

			if (condition(_imageHSV(i, j)[0], _imageHSV(i, j)[1], _imageHSV(i, j)[2])) {
				_imageHSV(i, j)[0] = 255;
				_imageHSV(i, j)[1] = 0;
				_imageHSV(i, j)[2] = 0;
			}
			else {
				_imageHSV(i, j)[0] = 0;
				_imageHSV(i, j)[1] = 0;
				_imageHSV(i, j)[2] = 0;
			}
		}
	}

	_imageHSV = this->RemoveParticles(_imageHSV);
				
	switch (color) {
		case Red: {
			this->imageRedHSV = _imageHSV.clone();
			break;
		}
		case Orange: {
			this->imageOrangeHSV = _imageHSV.clone();
			break;
		}
		case White: {
			this->imageWhiteHSV = _imageHSV.clone();
			break;
		}
		default: {
			return;
		}
	}

}


bool LogoDetector::isPixelOrange(double h, double s, double v) {
	return ((ORANGE_H_MAX >= h && h >= ORANGE_H_MIN) 
		&& (ORANGE_S_MAX >= s && s >= ORANGE_S_MIN) 
		&& (ORANGE_V_MAX >= v && v >= ORANGE_V_MIN));
}

bool LogoDetector::isPixelRed(double h, double s, double v) {
	if ((((int)h >= 0 && (int)h <= 14) || ((int)h >= 342 && (int)h <= 360)) && ((int)s > 49) && ((int)v > 59)) {
		int a = 0;
	}

	if (((RED_H_MAX_1 >= h && h >= RED_H_MIN_1) || (RED_H_MAX_2 >= h && h >= RED_H_MIN_2))
		&& (RED_S_MAX >= s && s >= RED_S_MIN)
		&& (RED_V_MAX >= v && v >= RED_V_MIN))
		return true;
	else
		return false;
}

bool LogoDetector::isPixelWhite(double h, double s, double v) {
	return ((WHITE_H_MAX >= h && h >= WHITE_H_MIN)
		&& (WHITE_S_MAX >= s && s >= WHITE_S_MIN)
		&& (WHITE_V_MAX >= v && v >= WHITE_V_MIN));
}

cv::Mat LogoDetector::RemoveParticles(cv::Mat& image) {

	cv::Mat_<cv::Vec3b> _image = image.clone();
	cv::Mat_<cv::Vec3b> _IResult = image;
	int counter = 0;

	for (int i = 1; i < (_image.rows - 1); i++)
	{

		for (int j = 1; j < (_image.cols - 1); j++){
			counter = 0;

			if (_image(i, j)[0] != 0) {
				if (_image(i - 1, j - 1)[0] != 0) ++counter;
				if (_image(i - 1, j)[0] != 0) ++counter;
				if (_image(i - 1, j - 1)[0] != 0) ++counter;
				if (_image(i + 1, j + 1)[0] != 0) ++counter;
				if (_image(i + 1, j)[0] != 0) ++counter;
				if (_image(i, j + 1)[0] != 0) ++counter;
				if (_image(i - 1, j + 1)[0] != 0) ++counter;
				if (_image(i + 1, j - 1)[0] != 0) ++counter;

				if (counter < 4) {
					_IResult(i ,j)[0] = 0;
					_IResult(i, j)[1] = 0;
					_IResult(i, j)[2] = 0;

				}

			}

		}

	};

	return _IResult;
};

void LogoDetector::ApplyMedianFilter(int dimension) {

	cv::Mat_<cv::Vec3b> _imageRGB = this->imageRGB;
	cv::Mat_<cv::Vec3b> _IResult = this->imageRGB.clone();
	int tableSize = dimension * dimension;
	int* rankTable = new int[tableSize];

	for (int i = dimension / 2; i < (imageRGB.rows - dimension / 2); i++)
	{

		for (int j = dimension / 2; j < (_imageRGB.cols - dimension / 2); j++) {

			int a = 0, b = 0, idx;

			for (; a < dimension; a++) {
				b = 0;
				for (; b < dimension; b++) {
					idx = a * dimension + b;

					// brightness function calculation for each pixel from matrix
					*(rankTable + idx) = _imageRGB(i - dimension / 2 + a, j - dimension / 2 + b)[0]
						+ _imageRGB(i - dimension / 2 + a, j - dimension / 2 + b)[1]
						+ _imageRGB(i - dimension / 2 + a, j - dimension / 2 + b)[2];
				}
			}

			// declaration of two vectors, first one stores pixels in initial order, second one keeps pixels after sorting
			std::vector<int> rankVector, rankVectorSorted;
			rankVector.insert(rankVector.begin(), rankTable, rankTable + tableSize);
			rankVectorSorted.insert(rankVectorSorted.begin(), rankTable, rankTable + tableSize);

			std::sort(rankVectorSorted.begin(), rankVectorSorted.end());

			int medianValue = rankVectorSorted[tableSize - 1]; //rankVectorSorted[40];
			int medianX, medianY;

			// searching pixel with median of bightness function values among pixel from matrix
			for (int c = 0; c < tableSize; c++) {
				if (medianValue == rankVector[c]) {
					medianX = c / dimension;
					medianY = c % dimension;
					break;
				}
			}

			// replacement central pixel's RGB components values with median values
			_IResult(i, j)[0] = _imageRGB(i - dimension / 2 + medianX, j - dimension / 2 + medianY)[0];
			_IResult(i, j)[1] = _imageRGB(i - dimension / 2 + medianX, j - dimension / 2 + medianY)[1];
			_IResult(i, j)[2] = _imageRGB(i - dimension / 2 + medianX, j - dimension / 2 + medianY)[2];

		}
	}
	this->imageRGB = _IResult;

}

void LogoDetector::SearchBiggestAreasWithColor(Color color) {

	cv::Mat_<cv::Vec3b> _imageHSV;

	switch (color)
	{
		case Red: {
			_imageHSV = this->imageRedHSV;
			break;
		}
		case Orange: {
			_imageHSV = this->imageOrangeHSV;
			break;
		}
		case White: {
			_imageHSV = this->imageWhiteHSV;
			break;
		}
	}


	uint separateAreasCounter = 0;
	uint currentAreaNo = 0;
	uint tempObjectId = 0;
	uint* pixelsInObjectsCounter = new uint[600];
	for (int i = 0; i < 600; ++i) {
		pixelsInObjectsCounter[i] = 0;
	}
	std::queue <std::tuple<int, int>> pixelsToCheck;
	uint tempI = 0, tempJ = 0;

	for (int i = 0; i < this->rowsNumber; i++) {
		for (int j = 0; j < this->columnsNumber; j++) {
			if (_imageHSV(i, j)[0] != 0) {
				if (_imageHSV(i, j)[1] == 0) {

                    separateAreasCounter += 1;
					if (separateAreasCounter > 255) {

					}
					
					tempI = i; tempJ = j;
					_imageHSV(i, j)[2] = 1;

				pixel:

					_imageHSV(i, j)[1] = currentAreaNo = separateAreasCounter;

					if (separateAreasCounter > 255) {
						_imageHSV(i, j)[0] = 254;
						tempObjectId = currentAreaNo + 256;
					}
					else tempObjectId = currentAreaNo;
					++pixelsInObjectsCounter[currentAreaNo];

					//check neighbours
					if ((j + 1) < this->columnsNumber && _imageHSV(i, j + 1)[0] == 255 && _imageHSV(i, j + 1)[1] == 0 && _imageHSV(i, j + 1)[2] == 0) {
						pixelsToCheck.push(std::make_tuple(i, j + 1));
						_imageHSV(i, j + 1)[2] = 1;
					}

					if ((i + 1) < this->rowsNumber && (j + 1) < this->columnsNumber && _imageHSV(i + 1, j + 1)[0] == 255 && _imageHSV(i + 1, j + 1)[1] == 0 && _imageHSV(i + 1, j + 1)[2] == 0) {
						pixelsToCheck.push(std::make_tuple(i + 1, j + 1));
						_imageHSV(i + 1, j + 1)[2] = 1;
					}

					if ((i + 1) < this->rowsNumber && _imageHSV(i + 1, j)[0] == 255 && _imageHSV(i + 1, j)[1] == 0 && _imageHSV(i + 1, j)[2] == 0) {
						pixelsToCheck.push(std::make_tuple(i + 1, j));
						_imageHSV(i + 1, j)[2] = 1;
					}

					if ((i + 1) < this->rowsNumber && (j - 1) > -1 && _imageHSV(i + 1, j - 1)[0] == 255 && _imageHSV(i + 1, j - 1)[1] == 0 && _imageHSV(i + 1, j - 1)[2] == 0) {
						pixelsToCheck.push(std::make_tuple(i + 1, j - 1));
						_imageHSV(i + 1, j - 1)[2] = 1;
					}

					if ((j - 1) > -1 && _imageHSV(i, j - 1)[0] == 255 && _imageHSV(i, j - 1)[1] == 0 && _imageHSV(i, j - 1)[2] == 0) {
						pixelsToCheck.push(std::make_tuple(i, j - 1));
						_imageHSV(i, j - 1)[2] = 1;
					}

					if ((i - 1) > -1 && (j - 1) > -1 && _imageHSV(i - 1, j - 1)[0] == 255 && _imageHSV(i - 1, j - 1)[1] == 0 && _imageHSV(i - 1, j - 1)[2] == 0) {
						pixelsToCheck.push(std::make_tuple(i - 1, j - 1));
						_imageHSV(i - 1, j - 1)[2] = 1;
					}

					if ((i - 1) > -1 && _imageHSV(i - 1, j)[0] == 255 && _imageHSV(i - 1, j)[1] == 0 && _imageHSV(i - 1, j)[2] == 0) {
						pixelsToCheck.push(std::make_tuple(i - 1, j));
						_imageHSV(i - 1, j)[2] = 1;
					}

					if ((i - 1) > -1 && (j + 1) < this->columnsNumber && _imageHSV(i - 1, j + 1)[0] == 255 && _imageHSV(i - 1, j + 1)[1] == 0 && _imageHSV(i - 1, j + 1)[2] == 0) {
						pixelsToCheck.push(std::make_tuple(i - 1, j + 1));
						_imageHSV(i - 1, j + 1)[2] = 1;
					}



					if (!pixelsToCheck.empty()) {
						i = std::get<0>(pixelsToCheck.front());
						j = std::get<1>(pixelsToCheck.front());
						pixelsToCheck.pop();
						goto pixel;
					}


					i = tempI;
					j = tempJ;
					
				}
				else currentAreaNo = _imageHSV(i, j)[1];
			}
		}
	}

	std::cout << "no ++ :" << separateAreasCounter << std::endl;

	this->GetFiveBiggestObjectIdsFromArray(color, pixelsInObjectsCounter);

}

void LogoDetector::GetFiveBiggestObjectIdsFromArray(Color color, uint* objectsSizeArray) {
	
	uint* sortedObjectsSizeArray = new uint[600];
	uint fiveBiggestObjectSize[50], fiveBiggestObjectIds[50];

	for (int i = 0; i < 600; ++i) {
		sortedObjectsSizeArray[i] = objectsSizeArray[i];
	}

	std::sort(sortedObjectsSizeArray, sortedObjectsSizeArray + 600, std::greater<int>());

	for (int i = 0; i < 600; ++i) {
		std::cout << sortedObjectsSizeArray[i] << " ";
	}
	std::cout << std::endl;

	for (int i = 0; i < 50; i++) {
		fiveBiggestObjectSize[i] = sortedObjectsSizeArray[i];
		for (int j = 0; j < 600; j++) {
			if (fiveBiggestObjectSize[i] == objectsSizeArray[j]){
				fiveBiggestObjectIds[i] = j;
				std::cout << " [ " << fiveBiggestObjectSize[i] << " -> " << j << " ] ";
				objectsSizeArray[j] = -1;
				break;
			}
        }
  	}

	std::cout << std::endl;
	

	switch (color)
	{
		case Red: {
			for (int i = 0; i < 50; i++) {
				this->biggestRedColorAreasIds[i] = fiveBiggestObjectIds[i];
			}
			break;
		}
		case Orange: {
			for (int i = 0; i < 50; i++) {
				this->biggestOrangeColorAreasIds[i] = fiveBiggestObjectIds[i];
			}
			break;
		}
		case White: {
			for (int i = 0; i < 50; i++) {
				this->biggestWhiteColorAreasIds[i] = fiveBiggestObjectIds[i];
			}
			break;
		}	
	}
	delete[] sortedObjectsSizeArray;

}

void LogoDetector::ChangePixelToObjectId(uint _old, uint _new, cv::Mat& image, uint* pixelsInObjectsCounter) {
	cv::Mat_<cv::Vec3b> _image = image;

	for (int i = 0; i < this->rowsNumber; i++) {
		for (int j = 0; j < this->columnsNumber; j++) {
			if (_image(i, j)[1] == _old) _image(i, j)[1] = _new;
		}
	}

	pixelsInObjectsCounter[_new] += pixelsInObjectsCounter[_old];
	pixelsInObjectsCounter[_old] = 0;
}

bool LogoDetector::IsObjectOrangeRectangle(Invariants invariants) {

	return (OrangeRectM1Upper >= invariants.M1 && invariants.M1 >= OrangeRectM1Lower
		&& OrangeRectM2Upper >= invariants.M2 && invariants.M2 >= OrangeRectM2Lower
		&& OrangeRectM7Upper >= invariants.M7 && invariants.M7 >= OrangeRectM7Lower);
}

bool LogoDetector::IsObjectRedRectangle(Invariants invariants) {

	return (RedRectM1Upper >= invariants.M1 && invariants.M1 >= RedRectM1Lower
		&& RedRectM2Upper >= invariants.M2 && invariants.M2 >= RedRectM2Lower
		&& RedRectM7Upper >= invariants.M7 && invariants.M7 >= RedRectM7Lower);
}

bool LogoDetector::IsObjectSunCircle(Invariants invariants) {

	return (SunCircleM1Upper >= invariants.M1 && invariants.M1 >= SunCircleM1Lower
		&& SunCircleM2Upper >= invariants.M2 && invariants.M2 >= SunCircleM2Lower
		&& SunCircleM7Upper >= invariants.M7 && invariants.M7 >= SunCircleM7Lower);
}

bool LogoDetector::IsObjectSunRing(Invariants invariants) {

	return (SunRingM1Upper >= invariants.M1 && invariants.M1 >= SunRingM1Lower
		&& SunRingM2Upper >= invariants.M2 && invariants.M2 >= SunRingM2Lower
		&& SunRingM7Upper >= invariants.M7 && invariants.M7 >= SunRingM7Lower);
}

bool LogoDetector::IsObjectPLetter(Invariants invariants) {

	return (PLetterM1Upper >= invariants.M1 && invariants.M1 >= PLetterM1Lower
		&& PLetterM2Upper >= invariants.M2 && invariants.M2 >= PLetterM2Lower
		&& PLetterM7Upper >= invariants.M7 && invariants.M7 >= PLetterM7Lower
		&& PLetterMalinowskaFactorUpper >= invariants.malinowskaFactor && invariants.malinowskaFactor >= PLetterMalinowskaFactorLower);
}

bool LogoDetector::IsObjectOLetter(Invariants invariants) {

	return (OLetterM1Upper >= invariants.M1 && invariants.M1 >= OLetterM1Lower
		&& OLetterM2Upper >= invariants.M2 && invariants.M2 >= OLetterM2Lower
		&& OLetterM7Upper >= invariants.M7 && invariants.M7 >= OLetterM7Lower
		&& OLetterMalinowskaFactorUpper >= invariants.malinowskaFactor && invariants.malinowskaFactor >= OLetterMalinowskaFactorLower);
}

bool LogoDetector::IsObjectLLetter(Invariants invariants) {

	return (LLetterM1Upper >= invariants.M1 && invariants.M1 >= LLetterM1Lower
		&& LLetterM2Upper >= invariants.M2 && invariants.M2 >= LLetterM2Lower
		&& LLetterM7Upper >= invariants.M7 && invariants.M7 >= LLetterM7Lower
		&& LLetterMalinowskaFactorUpper >= invariants.malinowskaFactor && invariants.malinowskaFactor >= LLetterMalinowskaFactorLower);
}

bool LogoDetector::IsObjectSLetter(Invariants invariants) {

	return (SLetterM1Upper >= invariants.M1 && invariants.M1 >= SLetterM1Lower
		&& SLetterM2Upper >= invariants.M2 && invariants.M2 >= SLetterM2Lower
		&& SLetterM7Upper >= invariants.M7 && invariants.M7 >= SLetterM7Lower
		&& SLetterMalinowskaFactorUpper >= invariants.malinowskaFactor && invariants.malinowskaFactor >= SLetterMalinowskaFactorLower);
}

bool LogoDetector::IsObjectALetter(Invariants invariants) {

	return (ALetterM1Upper >= invariants.M1 && invariants.M1 >= ALetterM1Lower
		&& ALetterM2Upper >= invariants.M2 && invariants.M2 >= ALetterM2Lower
		&& ALetterM7Upper >= invariants.M7 && invariants.M7 >= ALetterM7Lower
		&& ALetterMalinowskaFactorUpper >= invariants.malinowskaFactor && invariants.malinowskaFactor >= ALetterMalinowskaFactorLower);
}

bool LogoDetector::IsObjectTLetter(Invariants invariants) {

	return (TLetterM1Upper >= invariants.M1 && invariants.M1 >= TLetterM1Lower
		&& TLetterM2Upper >= invariants.M2 && invariants.M2 >= TLetterM2Lower
		&& TLetterM7Upper >= invariants.M7 && invariants.M7 >= TLetterM7Lower
		&& TLetterMalinowskaFactorUpper >= invariants.malinowskaFactor && invariants.malinowskaFactor >= TLetterMalinowskaFactorLower);
}

void LogoDetector::DrawMainObjectsBoundaries() {
	int listSize = this->areasList.size();
	cv::Mat_<cv::Vec3b> _imageRGB = this->imageRGB;

	for (int i = 0; i < listSize; i++) {
		if ((this->areasList[i].color == Red && this->areasList[i].hasPLetter && this->areasList[i].hasOLetter
			&& this->areasList[i].hasLLetter && this->areasList[i].hasSLetter && this->areasList[i].hasALetter
			&& this->areasList[i].hasTLetter) || (this->areasList[i].color == Orange && this->areasList[i].hasSunCircle && this->areasList[i].hasSunRing))
		{
			Invariants invariants; 
			if(this->areasList[i].color == Red) 
				invariants = InvariantsCounter::GetInvariants(this->imageRedHSV, this->areasList[i].id);
			else 
				invariants = InvariantsCounter::GetInvariants(this->imageOrangeHSV, this->areasList[i].id);

			for (int x = invariants.minX; x <= invariants.maxX; x++) {
				_imageRGB(invariants.minY, x)[0] = 0;
				_imageRGB(invariants.minY, x)[1] = 255;
				_imageRGB(invariants.minY, x)[2] = 0;

				_imageRGB(invariants.maxY, x)[0] = 0;
				_imageRGB(invariants.maxY, x)[1] = 255;
				_imageRGB(invariants.maxY, x)[2] = 0;
			}

			for (int y = invariants.minY; y <= invariants.maxY; y++) {
				_imageRGB(y, invariants.minX)[0] = 0;
				_imageRGB(y, invariants.minX)[1] = 255;
				_imageRGB(y, invariants.minX)[2] = 0;

				_imageRGB(y, invariants.maxX)[0] = 0;
				_imageRGB(y, invariants.maxX)[1] = 255;
				_imageRGB(y, invariants.maxX)[2] = 0;
			}
		}
	}

	this->imageRGB = _imageRGB;
}