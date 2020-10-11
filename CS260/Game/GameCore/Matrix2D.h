// ---------------------------------------------------------------------------
// File Name : Vector2D.h
// Author		 : Steven George
// Brief     : Interface file for Matrix2D
// ---------------------------------------------------------------------------
#ifndef MATRIX2D_H
#define MATRIX2D_H

#ifndef PI
	#define	PI 3.1415926535897932384626433832795f
#endif

#include "Vector2D.h"

typedef struct Matrix2D
{
	float m[3][3];
}Matrix2D;

// ---------------------------------------------------------------------------
/*
This function sets the matrix Result to the identity matrix
*/
// ---------------------------------------------------------------------------
void Matrix2DIdentity(Matrix2D &Result);

// ---------------------------------------------------------------------------
/*
This functions calculated the transpose matrix of Mtx and saves it in Result
*/
// ---------------------------------------------------------------------------
void Matrix2DTranspose(Matrix2D &Result, Matrix2D &Mtx);

// ---------------------------------------------------------------------------
/*
This function multiplies Mtx0 with Mtx1 and saves the result in Result
Result = Mtx0*Mtx1
*/
// ---------------------------------------------------------------------------
void Matrix2DConcat(Matrix2D &Result, Matrix2D &Mtx0, Matrix2D &Mtx1);

// ---------------------------------------------------------------------------
/*
This function creates a translation matrix from x & y and saves it in Result
*/
// ---------------------------------------------------------------------------
void Matrix2DTranslate(Matrix2D &Result, float x, float y);

// ---------------------------------------------------------------------------
/*
This function creates a scaling matrix from x & y and saves it in Result
*/
// ---------------------------------------------------------------------------
void Matrix2DScale(Matrix2D &Result, float x, float y);

// ---------------------------------------------------------------------------
/*
This matrix creates a rotation matrix from "Angle" whose value is in degree.
Save the resultant matrix in Result
*/
// ---------------------------------------------------------------------------
void Matrix2DRotDeg(Matrix2D &Result, float Angle);

// ---------------------------------------------------------------------------
/*
This matrix creates a rotation matrix from "Angle" whose value is in radian.
Save the resultant matrix in Result
*/
// ---------------------------------------------------------------------------
void Matrix2DRotRad(Matrix2D &Result, float Angle);

// ---------------------------------------------------------------------------
/*
This function multiplies the matrix Mtx with the vector Vec and saves the result in Result
Result = Mtx * Vec
*/
// ---------------------------------------------------------------------------
void Matrix2DMultVec(Vector2D &Result, Matrix2D &Mtx, Vector2D &Vec);


#endif