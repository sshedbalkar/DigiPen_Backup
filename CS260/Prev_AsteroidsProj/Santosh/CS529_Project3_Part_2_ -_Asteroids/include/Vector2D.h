/* Start Header -------------------------------------------------------
Copyright (C) 2011 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name		: Vector2D.h
Purpose			: Header file for the 2D Vector API
Language		: C++/MS VS 2010 
Platform		: Microsoft Visual Studio 2010/i7 2630QM, 8GB DDR3 1600MHz, 3GB NVIDIA GT 555M/Windows 7 Home Premium
Project			: CS529_SantoshShedbalkar_Project2_2
Author			: Santosh Shedbalkar/santosh.s/60001111
Creation date	: 09/29/2011
- End Header --------------------------------------------------------*/

#ifndef VECTOR2_H
#define VECTOR2_H

#include "math.h"

#ifndef EPSILON
	#define	EPSILON	0.0001f
#endif
#ifndef PI
	#define	PI		3.1415926535897932384626433832795f
#endif


typedef struct Vector2D
{
	float x, y;
}Vector2D;



/*
This function sets the coordinates of the 2D vector (pResult) to 0
*/
void Vector2DZero(Vector2D &pResult);

/*
This function sets the coordinates of the 2D vector (pResult) to x &y
*/
void Vector2DSet(Vector2D &pResult, float x, float y);

/*
In this function, pResult will be set to the opposite of pVec0
*/
void Vector2DNeg(Vector2D &pResult, Vector2D &pVec0);

/*
In this function, pResult will be the sum of pVec0 and pVec1
*/
void Vector2DAdd(Vector2D &pResult, Vector2D &pVec0, Vector2D &pVec1);

/*
In this function, pResult will be the difference between pVec0 &pVec1: pVec0 - pVec1
*/
void Vector2DSub(Vector2D &pResult, Vector2D &pVec0, Vector2D &pVec1);

/*
In this function, pResult will be the unit vector of pVec0
*/
void Vector2DNormalize(Vector2D &pResult, Vector2D &pVec0);

/*
In this function, pResult will be the vector pVec0 scaled by the value c
*/
void Vector2DScale(Vector2D &pResult, Vector2D &pVec0, float c);

/*
In this function, pResult will be the vector pVec0 scaled by c and added to pVec1 
*/
void Vector2DScaleAdd(Vector2D &pResult, Vector2D &pVec0, Vector2D &pVec1, float c);

/*
In this function, pResult will be the vector pVec0 scaled by c and pVec1 will be subtracted from it 
*/
void Vector2DScaleSub(Vector2D &pResult, Vector2D &pVec0, Vector2D &pVec1, float c);

/*
This function returns the length of the vector pVec0
*/
float Vector2DLength(Vector2D &pVec0);

/*
This function return the square of pVec0's length. Avoid the square root
*/
float Vector2DSquareLength(Vector2D &pVec0);

/*
In this function, pVec0 and pVec1 are considered as 2D points.
The distance between these 2 2D points is returned
*/
float Vector2DDistance(Vector2D &pVec0, Vector2D &pVec1);

/*
In this function, pVec0 and pVec1 are considered as 2D points.
The squared distance between these 2 2D points is returned. Avoid the square root
*/
float Vector2DSquareDistance(Vector2D &pVec0, Vector2D &pVec1);

/*
This function returns the dot product between pVec0 and pVec1
*/
float Vector2DDotProduct(Vector2D &pVec0, Vector2D &pVec1);

/*
This function computes the coordinates of the vector repreented by the angle "angle"
*/
void Vector2DFromAngle(Vector2D &pResult, float angle);

#endif