#pragma once
#include "Enums.h"
#include <set>

typedef struct {
	int id;				// -1 means background, 
	                    //  0 means not-recognize yet object, 
	                    //  1 means object recognized as 'unknown object',
	                    //  2,3,4,5... to positive infinity there are recognized object


	//int minX, minY, maxX, maxY;
	int secondMainObjectReferenceId = -1;
	int geometricCenterX, geometricCenterY;
	double halfDiagonal;
	bool hasSunCircle, hasSunRing;
	bool hasPLetter, hasOLetter, hasLLetter, hasSLetter, hasALetter, hasTLetter;
	std::set<int> subobjectIds;
	Color color;	
	ObjectType objectType;

} MainObject;

typedef struct {
	double r;       // a fraction between 0 and 1
	double g;       // a fraction between 0 and 1
	double b;       // a fraction between 0 and 1
} RGB;

typedef struct {
	double h;       // angle in degrees
	double s;       // a fraction between 0 and 1
	double v;       // a fraction between 0 and 1
} HSV;

typedef struct {
	double M1;
	double M2;
	double M7;
	double malinowskaFactor;
	int S;
	int minX, minY, maxX, maxY;

} Invariants;

