#pragma once
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include "../utils/Structs.h"

class LogoDetector {

public:

	cv::Mat imageRGB, imageHSV, imageRedHSV, imageOrangeHSV, imageWhiteHSV, imageMask;
	std::string fileName;
	int rowsNumber, columnsNumber;
	uint biggestRedColorAreasIds[50], biggestOrangeColorAreasIds[50], biggestWhiteColorAreasIds[50];
	int** pixelsToObjectsTable;
	std::vector<MainObject> areasList;
	std::set<int> validRedAreasIds, validOrangeAreasIds, validWhiteAreasIds;

	LogoDetector();
	~LogoDetector();

	bool DetectObject(std::string fileName);

private:
	void InitializeImageDataTable(std::string fileName);
	void RunPreProcessImage();
	void FindConsistentAreas();
	void  FindAreasByColor(Color color);
	//void  FindAreasByColor(Color color, int minX, int maxX, int minY, int maxY);

	void CreateImageHSV();
	void ApplyMedianFilter(int dimension);
    cv::Mat RemoveParticles(cv::Mat& image);
	void SearchBiggestAreasWithColor(Color color);
	void ChangePixelToObjectId(uint _old, uint _new, cv::Mat& image, uint* pixelsInObjectsCounter);
	void GetFiveBiggestObjectIdsFromArray(Color color, uint* objectsSizeArray);
	void ResearchOrangeAreas(cv::Mat& image);
	void ResearchRedAreas(cv::Mat& image);
	void MatchRedRectsWithOrangeRects();
	void CreateMaskImageForFinalResult();
	void DrawFinalResult();
	void AddSpecificAreaToMask(Color color, int objectId);

	bool IsObjectOrangeRectangle(Invariants invariants);
	bool IsObjectRedRectangle(Invariants invariants);
	bool IsObjectSunCircle(Invariants invariants);
	bool IsObjectSunRing(Invariants invariants);

	void FindSubobjectsInOrangeRectangle(uint objectId, Invariants mainObjectInvariants);
	void FindSubobjectsInRedRectangle(uint objectId, Invariants mainObjectInvariants);
	bool IsObjectPLetter(Invariants invariants);
	bool IsObjectOLetter(Invariants invariants);
	bool IsObjectLLetter(Invariants invariants);
	bool IsObjectSLetter(Invariants invariants);
	bool IsObjectALetter(Invariants invariants);
	bool IsObjectTLetter(Invariants invariants);

	bool AreSubobjectConstraintsFullfilled(Invariants subobjectInvariants, Invariants mainObjectInvariants);


	bool isPixelOrange(double h, double s, double v);
	bool isPixelRed(double h, double s, double v);
	bool isPixelWhite(double h, double s, double v);
	void CheckRGB();

	void DrawMainObjectsBoundaries();

};


