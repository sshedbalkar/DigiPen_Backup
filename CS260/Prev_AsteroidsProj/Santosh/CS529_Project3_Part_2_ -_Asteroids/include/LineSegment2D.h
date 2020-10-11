/* Start Header -------------------------------------------------------
Copyright (C) 2011 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name		: LineSegment.h
Purpose			: header file for the Line segment Math API
Language		: C++/MS VS 2010 
Platform		: Microsoft Visual Studio 2010/i7 2630QM, 8GB DDR3 1600MHz, 3GB NVIDIA GT 555M/Windows 7 Home Premium
Project			: CS529_SantoshShedbalkar_Project2_2
Author			: Santosh Shedbalkar/santosh.s/60001111
Creation date	: 09/29/2011
- End Header --------------------------------------------------------*/

#ifndef LINESEGMENT2D_H
#define LINESEGMENT2D_H

#include "Vector2D.h"



typedef struct LineSegment2D
{
	Vector2D mP0;		// Point on the line
	Vector2D mP1;		// Point on the line
	Vector2D mN;		// Line's normal
	float mNdotP0;		// To avoid computing it every time it's needed
}LineSegment2D;


/*
This function builds a 2D line segment's data using 2 points
 - Computes the normal (Unit Vector)
 - Computes the dot product of the normal with one of the points

 - Parameters
	- LS:		The to-be-built line segment
	- Point0:	One point on the line
	- Point1:	Another point on the line

 - Returns true if the lise equation was built successfully 
*/
bool BuildLineSegment2D(LineSegment2D &LS, Vector2D &Point0, Vector2D &Point1);




#endif