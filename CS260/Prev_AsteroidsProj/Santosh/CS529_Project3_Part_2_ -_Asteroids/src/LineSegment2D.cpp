/* Start Header -------------------------------------------------------
Copyright (C) 2011 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name		: LineSegment2D.cpp
Purpose			: Implements 2D line segment mathematical model
Language		: C++/MS VS 2010 
Platform		: Microsoft Visual Studio 2010/i7 2630QM, 8GB DDR3 1600MHz, 3GB NVIDIA GT 555M/Windows 7 Home Premium
Project			: CS529_SantoshShedbalkar_Project2_2
Author			: Santosh Shedbalkar/santosh.s/60001111
Creation date	: 09/29/2011
- End Header --------------------------------------------------------*/

#include "LineSegment2D.h"

bool BuildLineSegment2D(LineSegment2D &LS, Vector2D &Point0, Vector2D &Point1)
{
	if(Vector2DSquareDistance(Point0, Point1) == 0)
	{
		return false;
	}
	else
	{
		Vector2DSet(LS.mP0, Point0.x, Point0.y);
		Vector2DSet(LS.mP1, Point1.x, Point1.y);
		Vector2DSet(LS.mN, Point1.y - Point0.y, Point0.x - Point1.x);
		Vector2DNormalize(LS.mN, LS.mN);
		LS.mNdotP0 = Vector2DDotProduct(LS.mN, LS.mP0);
	}
	return true;
}