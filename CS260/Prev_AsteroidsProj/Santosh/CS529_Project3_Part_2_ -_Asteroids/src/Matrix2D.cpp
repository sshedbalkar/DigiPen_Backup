/* Start Header -------------------------------------------------------
Copyright (C) 2011 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name		: Matrix2D.cpp
Purpose			: Implements 2D matrix functionalities
Language		: C++/MS VS 2010 
Platform		: Microsoft Visual Studio 2010/i7 2630QM, 8GB DDR3 1600MHz, 3GB NVIDIA GT 555M/Windows 7 Home Premium
Project			: CS529_SantoshShedbalkar_Project3_1
Author			: Santosh Shedbalkar/santosh.s/60001111
Creation date	: 10/10/2011
- End Header --------------------------------------------------------*/

#include "Matrix2D.h"

//////////////////////
// New in project 3 //
//////////////////////

/*
This function sets the matrix Result to the identity matrix
*/
void Matrix2DIdentity(Matrix2D &Result)
{
	Result.m[0][0] = 1.0f; Result.m[0][1] = 0.0f; Result.m[0][2] = 0.0f;
	Result.m[1][0] = 0.0f; Result.m[1][1] = 1.0f; Result.m[1][2] = 0.0f;
	Result.m[2][0] = 0.0f; Result.m[2][1] = 0.0f; Result.m[2][2] = 1.0f;
}

// ---------------------------------------------------------------------------

/*
This function calculates the transpose matrix of Mtx and saves it in Result
*/
void Matrix2DTranspose(Matrix2D &Result, Matrix2D &Mtx)
{
	Result.m[0][0] = Mtx.m[0][0];
	Result.m[0][1] = Mtx.m[1][0];
	Result.m[0][2] = Mtx.m[2][0];
	Result.m[1][0] = Mtx.m[0][1];
	Result.m[1][1] = Mtx.m[1][1];
	Result.m[1][2] = Mtx.m[2][1];
	Result.m[2][0] = Mtx.m[0][2];
	Result.m[2][1] = Mtx.m[1][2];
	Result.m[2][2] = Mtx.m[2][2];
}

// ---------------------------------------------------------------------------

/*
This function multiplies Mtx0 with Mtx1 and saves the result in Result
Result = Mtx0*Mtx1
*/
void Matrix2DConcat(Matrix2D &Result, Matrix2D &Mtx0, Matrix2D &Mtx1)
{
	float mresult[3][3];

	int i, j, k;

	for(i = 0; i < 3; ++i)
	{
		for(j = 0; j < 3; ++j)
		{
			mresult[i][j] = 0;
			for(k = 0; k < 3; ++k)
			{
				mresult[i][j] += Mtx0.m[i][k] * Mtx1.m[k][j];
			}
		}
	}

	Result.m[0][0] = mresult[0][0];
	Result.m[0][1] = mresult[0][1];
	Result.m[0][2] = mresult[0][2];
	Result.m[1][0] = mresult[1][0];
	Result.m[1][1] = mresult[1][1];
	Result.m[1][2] = mresult[1][2];
	Result.m[2][0] = mresult[2][0];
	Result.m[2][1] = mresult[2][1];
	Result.m[2][2] = mresult[2][2];
}

// ---------------------------------------------------------------------------

/*
This function creates a translation matrix from x & y and saves it in Result
*/
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
void Matrix2DScale(Matrix2D &Result, float x, float y)
{
	Result.m[0][0] = x; Result.m[0][1] = 0.0f; Result.m[0][2] = 0.0f;
	Result.m[1][0] = 0.0f; Result.m[1][1] = y; Result.m[1][2] = 0.0f;
	Result.m[2][0] = 0.0f; Result.m[2][1] = 0.0f; Result.m[2][2] = 1.0f;
}

// ---------------------------------------------------------------------------

/*
This matrix creates a rotation matrix from "Angle" whose value is in degree.
Save the resultant matrix in Result
*/
void Matrix2DRotDeg(Matrix2D &Result, float Angle)
{
	Matrix2DRotRad(Result, (float) (Angle * PI) / (180));
}

// ---------------------------------------------------------------------------

/*
This matrix creates a rotation matrix from "Angle" whose value is in radian.
Save the resultant matrix in Result
*/
void Matrix2DRotRad(Matrix2D &Result, float Angle)
{
	float c = cosf(Angle), s = sinf(Angle);
	
	Result.m[0][0] = c; Result.m[0][1] = -s; Result.m[0][2] = 0.0f;
	Result.m[1][0] = s; Result.m[1][1] = c; Result.m[1][2] = 0.0f;
	Result.m[2][0] = 0.0f; Result.m[2][1] = 0.0f; Result.m[2][2] = 1.0f;
}

// ---------------------------------------------------------------------------

/*
This function multiplies the matrix Mtx with the vector Vec and saves the result in Result
Result = Mtx * Vec
*/
void Matrix2DMultVec(Vector2D &Result, Matrix2D &Mtx, Vector2D &Vec)
{
	float x, y;
	x = Mtx.m[0][0] * Vec.x + Mtx.m[0][1] * Vec.y + Mtx.m[0][2];
	y = Mtx.m[1][0] * Vec.x + Mtx.m[1][1] * Vec.y + Mtx.m[1][2];
	
	Result.x = x;
	Result.y = y;
}

// ---------------------------------------------------------------------------
