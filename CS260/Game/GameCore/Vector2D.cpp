// ---------------------------------------------------------------------------
// File Name : Vector2D.cpp
// Author		 : Steven George
// Brief     : Implementation for Vector2D
// ---------------------------------------------------------------------------
#include "Vector2D.h"

#define EPSILON 0.0001

// ---------------------------------------------------------------------------

void Vector2DZero(Vector2D &pResult)
{
  pResult.x = 0.0f;
  pResult.y = 0.0f;
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
  float length = Vector2DLength(pVec0);
  
  pResult.x = pVec0.x / length;
  pResult.y = pVec0.y / length;
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
  Vector2DScale(pVec0, pVec0, c);
  Vector2DAdd(pResult, pVec0, pVec1);
}

// ---------------------------------------------------------------------------

void Vector2DScaleSub(Vector2D &pResult, Vector2D &pVec0, Vector2D &pVec1, float c)
{
  Vector2DScale(pResult, pVec0, c);
  Vector2DSub(pResult, pVec0, pVec1);
}

// ---------------------------------------------------------------------------

void Vector2DProject(Vector2D &pResult, Vector2D &pVec0, Vector2D &pVec1)
{
}

// ---------------------------------------------------------------------------

float Vector2DLength(Vector2D &pVec0)
{
  float length = sqrt((pVec0.x * pVec0.x) + (pVec0.y * pVec0.y));
  if(length != 0.0f)
  {
    return length;
  }

  return 1.0f;
}

// ---------------------------------------------------------------------------

float Vector2DSquareLength(Vector2D &pVec0)
{
  float lenSquared = (pVec0.x * pVec0.x) + (pVec0.y * pVec0.y);
  if(lenSquared != 0.0f)
  {
    return lenSquared;
  }
	
  return 1.0f;
}

// ---------------------------------------------------------------------------

float Vector2DDistance(Vector2D &pVec0, Vector2D &pVec1)
{
	return sqrt((pVec0.x - pVec1.x) * (pVec0.x - pVec1.x) + 
              (pVec0.y - pVec1.y) * (pVec0.y - pVec1.y)); 
}

// ---------------------------------------------------------------------------

float Vector2DSquareDistance(Vector2D &pVec0, Vector2D &pVec1)
{
	return ((pVec0.x - pVec1.x) * (pVec0.x - pVec1.x) + 
          (pVec0.y - pVec1.y) * (pVec0.y - pVec1.y)); 
}

// ---------------------------------------------------------------------------

float Vector2DDotProduct(Vector2D &pVec0, Vector2D &pVec1)
{
	return (pVec0.x * pVec1.x) + (pVec0.y * pVec1.y);
}

// ---------------------------------------------------------------------------

void Vector2DFromAngle(Vector2D &pResult, float angle)
{
  pResult.x = cos(angle);
  pResult.y = sin(angle);
}

// ---------------------------------------------------------------------------
