// ---------------------------------------------------------------------------
// File Name : Matrix2D.cpp
// Author		 : Steven George
// Brief     : Implementation for Matrix2D
// ---------------------------------------------------------------------------
#include "Matrix2D.h"

// ---------------------------------------------------------------------------
/*
This function sets the matrix Result to the identity matrix
*/
// ---------------------------------------------------------------------------
void Matrix2DIdentity(Matrix2D &Result)
{
  Result.m[0][0] = 1.0f; Result.m[0][1] = 0.0f; Result.m[0][2] = 0.0f;
  Result.m[1][0] = 0.0f; Result.m[1][1] = 1.0f; Result.m[1][2] = 0.0f;
  Result.m[2][0] = 0.0f; Result.m[2][1] = 0.0f; Result.m[2][2] = 1.0f;
}

// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
/*
This functions calculated the transpose matrix of Mtx and saves it in Result
*/
// ---------------------------------------------------------------------------
void Matrix2DTranspose(Matrix2D &Result, Matrix2D &Mtx)
{
  int i = 0;
  int j = 0;

  for(int i = 0; i < 3; i++)
  {
		for(int j = 0; j < 3; j++)
    {
			Result.m[i][j] = Mtx.m[j][i];
    }
  }
}

// ---------------------------------------------------------------------------
/*
This function multiplies Mtx0 with Mtx1 and saves the result in Result
Result = Mtx0*Mtx1
*/
// ---------------------------------------------------------------------------
void Matrix2DConcat(Matrix2D &Result, Matrix2D &Mtx0, Matrix2D &Mtx1)
{
  int i = 0;
  int j = 0;
  int k = 0;

  float val = 0;

  Matrix2D matrix = Result;

  for(i = 0; i < 3; i++)
  {
    for(j = 0; j < 3; j++)
    {
      val = 0;

      for(k = 0; k < 3; k++)
      {
        val += Mtx0.m[i][k] * Mtx1.m[k][j];
      }

      matrix.m[i][j] = val;
    }
  } 

  Result = matrix;
}

// ---------------------------------------------------------------------------
/*
This function creates a translation matrix from x & y and saves it in Result
*/
// ---------------------------------------------------------------------------
void Matrix2DTranslate(Matrix2D &Result, float x, float y)
{
	Result.m[0][0] = 1.0f; Result.m[0][1] = 0.0f; Result.m[0][2] = x;
  Result.m[1][0] = 0.0f; Result.m[1][1] = 1.0f; Result.m[1][2] = y;
  Result.m[2][0] = 0.0f; Result.m[2][1] = 0.0f; Result.m[2][2] = 1.0f;
}

// ---------------------------------------------------------------------------
/*
This function creates a scaling matrix from x & y and saves it in Result
*/
// ---------------------------------------------------------------------------
void Matrix2DScale(Matrix2D &Result, float x, float y)
{
  Result.m[0][0] = x;    Result.m[0][1] = 0.0f; Result.m[0][2] = 0.0f;
  Result.m[1][0] = 0.0f; Result.m[1][1] = y;    Result.m[1][2] = 0.0f;
  Result.m[2][0] = 0.0f; Result.m[2][1] = 0.0f; Result.m[2][2] = 1.0f;
}

// ---------------------------------------------------------------------------
/*
This matrix creates a rotation matrix from "Angle" whose value is in degree.
Save the resultant matrix in Result
*/
// ---------------------------------------------------------------------------
void Matrix2DRotDeg(Matrix2D &Result, float Angle)
{
  float angle = float((Angle * PI) / 180.0f);

  Result.m[0][0] = cos(angle); Result.m[0][1] = -sin(angle); Result.m[0][2] = 0.0f;
  Result.m[1][0] = sin(angle); Result.m[1][1] =  cos(angle); Result.m[1][2] = 0.0f;
  Result.m[2][0] = 0.0f;       Result.m[2][1] = 0.0f;        Result.m[2][2] = 1.0f;
}

// ---------------------------------------------------------------------------
/*
This matrix creates a rotation matrix from "Angle" whose value is in radian.
Save the resultant matrix in Result
*/
// ---------------------------------------------------------------------------
void Matrix2DRotRad(Matrix2D &Result, float Angle)
{
  Result.m[0][0] = cos(Angle); Result.m[0][1] = -sin(Angle); Result.m[0][2] = 0.0f;
  Result.m[1][0] = sin(Angle); Result.m[1][1] = cos(Angle);  Result.m[1][2] = 0.0f;
  Result.m[2][0] = 0.0f;       Result.m[2][1] = 0.0f;        Result.m[2][2] = 1.0f;
}

// ---------------------------------------------------------------------------
/*
This function multiplies the matrix Mtx with the vector Vec and saves the result in Result
Result = Mtx * Vec
*/
// ---------------------------------------------------------------------------
void Matrix2DMultVec(Vector2D &Result, Matrix2D &Mtx, Vector2D &Vec)
{
  Vector2D vecResult = Result;

  vecResult.x = (Mtx.m[0][0] * Vec.x) + (Mtx.m[0][1] * Vec.y) + Mtx.m[0][2];
  vecResult.y = (Mtx.m[1][0] * Vec.x) + (Mtx.m[1][1] * Vec.y) + Mtx.m[1][2];

  Result = vecResult;
}
