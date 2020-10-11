/* Start Header -------------------------------------------------------
Copyright (C) 2011 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name		: Vector2D.cpp
Purpose			: Implements 2D Vector Math
Language		: C++/MS VS 2010 
Platform		: Microsoft Visual Studio 2010/i7 2630QM, 8GB DDR3 1600MHz, 3GB NVIDIA GT 555M/Windows 7 Home Premium
Project			: CS529_SantoshShedbalkar_Project2_2
Author			: Santosh Shedbalkar/santosh.s/60001111
Creation date	: 09/29/2011
- End Header --------------------------------------------------------*/

#include "Vector2D.h"

// ---------------------------------------------------------------------------

void Vector2DZero(Vector2D &pResult)
{
	pResult.x = 0.0;
	pResult.y = 0.0;
}

// ---------------------------------------------------------------------------

void Vector2DSet(Vector2D &pResult, float x, float y)
{
	pResult.x = x;
	pResult.y = y;
}

// ---------------------------------------------------------------------------

void Vector2DNeg(Vector2D &pResult, Vector2D &pVec0)
{
	pResult.x = -pVec0.x;
	pResult.y = -pVec0.y;
}

// ---------------------------------------------------------------------------

void Vector2DAdd(Vector2D &pResult, Vector2D &pVec0, Vector2D &pVec1)
{
	pResult.x = pVec0.x + pVec1.x;
	pResult.y = pVec0.y + pVec1.y;
}

// ---------------------------------------------------------------------------

void Vector2DSub(Vector2D &pResult, Vector2D &pVec0, Vector2D &pVec1)
{
	pResult.x = pVec0.x - pVec1.x;
	pResult.y = pVec0.y - pVec1.y;
}

// ---------------------------------------------------------------------------

void Vector2DNormalize(Vector2D &pResult, Vector2D &pVec0)
{
	float d = Vector2DLength(pVec0);
	pResult.x = (d < EPSILON) ? 0.0f : pVec0.x / d;
	pResult.y = (d < EPSILON) ? 0.0f : pVec0.y / d;
}

// ---------------------------------------------------------------------------

void Vector2DScale(Vector2D &pResult, Vector2D &pVec0, float c)
{
	pResult.x = pVec0.x * c;
	pResult.y = pVec0.y * c;
}

// ---------------------------------------------------------------------------

void Vector2DScaleAdd(Vector2D &pResult, Vector2D &pVec0, Vector2D &pVec1, float c)
{
	pResult.x = (pVec0.x * c) + pVec1.x;
	pResult.y = (pVec0.y * c) + pVec1.y;
}

// ---------------------------------------------------------------------------

void Vector2DScaleSub(Vector2D &pResult, Vector2D &pVec0, Vector2D &pVec1, float c)
{
	pResult.x = (pVec0.x * c) - pVec1.x;
	pResult.y = (pVec0.y * c) - pVec1.y;
}

// ---------------------------------------------------------------------------

float Vector2DLength(Vector2D &pVec0)
{
	return sqrt(Vector2DDotProduct(pVec0, pVec0));
}

// ---------------------------------------------------------------------------

float Vector2DSquareLength(Vector2D &pVec0)
{
	return (float) Vector2DDotProduct(pVec0, pVec0);
}

// ---------------------------------------------------------------------------

float Vector2DDistance(Vector2D &pVec0, Vector2D &pVec1)
{
	float dx = pVec1.x - pVec0.x;
	float dy = pVec1.y - pVec0.y;
	return (float) sqrt(dx * dx + dy * dy);
}

// ---------------------------------------------------------------------------

float Vector2DSquareDistance(Vector2D &pVec0, Vector2D &pVec1)
{
	float dx = pVec1.x - pVec0.x;
	float dy = pVec1.y - pVec0.y;
	return (float) (dx * dx + dy * dy);
}

// ---------------------------------------------------------------------------

float Vector2DDotProduct(Vector2D &pVec0, Vector2D &pVec1)
{
	return (float) (pVec0.x * pVec1.x + pVec0.y * pVec1.y);
}


// ---------------------------------------------------------------------------

void Vector2DFromAngle(Vector2D &pResult, float angle)
{
	pResult.x = cosf(angle);
	pResult.y = sinf(angle);
}

// ---------------------------------------------------------------------------
