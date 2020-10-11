/* Start Header -------------------------------------------------------
Copyright (C) 2011 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name		: Matrix2D.h
Purpose			: Library of 2D matrix functionalities
Language		: C++/MS VS 2010 
Platform		: Microsoft Visual Studio 2010/i7 2630QM, 8GB DDR3 1600MHz, 3GB NVIDIA GT 555M/Windows 7 Home Premium
Project			: CS529_SantoshShedbalkar_Project3_1
Author			: Santosh Shedbalkar/santosh.s/60001111
Creation date	: 10/10/2011
- End Header --------------------------------------------------------*/

#ifndef MATRIX2D_H
#define MATRIX2D_H


#include "Vector2D.h"

typedef struct Matrix2D
{
	float m[3][3];
}Matrix2D;

//////////////////////
// New in project 3 //
//////////////////////

/*
This function sets the matrix Result to the identity matrix
*/
void Matrix2DIdentity(Matrix2D &Result);

/*
This functions calculated the transpose matrix of Mtx and saves it in Result
*/
void Matrix2DTranspose(Matrix2D &Result, Matrix2D &Mtx);

/*
This function multiplies Mtx0 with Mtx1 and saves the result in Result
Result = Mtx0*Mtx1
*/
void Matrix2DConcat(Matrix2D &Result, Matrix2D &Mtx0, Matrix2D &Mtx1);

/*
This function creates a translation matrix from x & y and saves it in Result
*/
void Matrix2DTranslate(Matrix2D &Result, float x, float y);

/*
This function creates a scaling matrix from x & y and saves it in Result
*/
void Matrix2DScale(Matrix2D &Result, float x, float y);

/*
This matrix creates a rotation matrix from "Angle" whose value is in degree.
Save the resultant matrix in Result
*/
void Matrix2DRotDeg(Matrix2D &Result, float Angle);

/*
This matrix creates a rotation matrix from "Angle" whose value is in radian.
Save the resultant matrix in Result
*/
void Matrix2DRotRad(Matrix2D &Result, float Angle);

/*
This function multiplies the matrix Mtx with the vector Vec and saves the result in Result
Result = Mtx * Vec
*/
void Matrix2DMultVec(Vector2D &Result, Matrix2D &Mtx, Vector2D &Vec);


#endif