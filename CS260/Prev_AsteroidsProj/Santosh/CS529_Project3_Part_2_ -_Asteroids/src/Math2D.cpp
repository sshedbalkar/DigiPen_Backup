/* Start Header -------------------------------------------------------
Copyright (C) 2011 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name		: Math2D.cpp
Purpose			: Implements 2D collision math
Language		: C++/MS VS 2010 
Platform		: Microsoft Visual Studio 2010/i7 2630QM, 8GB DDR3 1600MHz, 3GB NVIDIA GT 555M/Windows 7 Home Premium
Project			: CS529_SantoshShedbalkar_Project2_2
Author			: Santosh Shedbalkar/santosh.s/60001111
Creation date	: 10/10/2011
- End Header --------------------------------------------------------*/

#include "Math2D.h"

static char strBuff[1024];
/*
This function tests if a given static point represented by (x,y) is inside a rectangle Rect

x		- x coordinate of the static point to be tested
y		- y coordinate of the static point to be tested
Rect	- centre point of the rectangle
Width	- Width of the rectangle
Height	- Height of the rectangle
*/
bool StaticPointToStaticRectLocal(float x, float y, Vector2D &Rect, float Width, float Height)
{
	//Calculate the edges of the rectangle
	float top = Rect.y - (Width / 2);
	float bot = Rect.y + (Width / 2);
	float lft = Rect.x - (Height / 2);
	float rgt = Rect.x + (Height / 2);

	return x >= lft && x <= rgt && y >= top && y <= bot;
}

/*
This function checks whether an animated point is colliding with a thick line segment (line segment with width > 0)

 - Parameters
	- Ps:		The point's starting location
	- Pe:		The point's ending location
	- LS:		The line segment
	- Ph:		This will be used to store the intersection point's coordinates (In case there's an intersection)
	- halfThickness: This is half of the thickness of the line segment

 - Returned value: Intersection time t
	- -1.0f:				If there's no intersection
	- Intersection time:	If there's an intersection
*/
float AnimatedPointToStaticThickLineSegment(Vector2D &Ps, Vector2D &Pe, LineSegment2D &LS, Vector2D &Ph, float halfThickness)
{
	Vector2D Vel;
	float ret = -1.0f, ti = -1.0f, D;
	float mNdotV = 0.0f, mNdotPs = 0.0f;
	//
	Vector2DSub(Vel, Pe, Ps);
	if((((mNdotPs = Vector2DDotProduct(LS.mN, Ps)) - LS.mNdotP0) < -halfThickness) && ((Vector2DDotProduct(LS.mN, Pe) - LS.mNdotP0) < -halfThickness))
	{
		// Both Ps and Pe fall in the inside half of the line segment. No Collision
		ret = -1.0f;
	}
	else if(((mNdotPs - LS.mNdotP0) > halfThickness) && ((Vector2DDotProduct(LS.mN, Pe) - LS.mNdotP0) > halfThickness))
	{
		// Both Ps and Pe fall in the outside half of the line segment. No Collision
		ret = -1.0f;
	}
	else if(abs(mNdotV = Vector2DDotProduct(LS.mN, Vel)) < EPSILON)
	{
		// Point is moving parallel to the line segment. No Collision
		ret = -1.0f;
	}
	else
	{
		D = ((mNdotPs - LS.mNdotP0) < 0) ? -halfThickness : halfThickness;
		ti = (LS.mNdotP0 - mNdotPs + D)/mNdotV;

		if(ti > 0 && ti <= 1)
		{
			Vector2D P0Ph, P1Ph, P0P1, P1P0;

			Vector2DScaleAdd(Ph, Vel, Ps, ti);

			Vector2DSub(P0Ph, Ph, LS.mP0);
			Vector2DSub(P0P1, LS.mP1, LS.mP0);

			if(Vector2DDotProduct(P0Ph, P0P1) >= 0)
			{
				Vector2DSub(P1Ph, Ph, LS.mP1);
				Vector2DSub(P1P0, LS.mP0, LS.mP1);

				if(Vector2DDotProduct(P1Ph, P1P0) >= 0)
				{
					// Point collides with the given line segment
					ret = ti;
				}
				else
				{
					// Point collides with infinite extension of wall … not finite wall!
					ret = -1.0f;
				}
			}
			else
			{
				// Point collides with infinite extension of wall … not finite wall!
				ret = -1.0f;
			}
		}
		else
		{
			// Point is moving away from the line segment or it will not collide in the current frame. No Collision
			ret = -1.0f;
		}
	}
	return ret;
}

/*
This function reflects an animated point on a thick line segment.
It should first make sure that the animated point is intersecting with the line 

 - Parameters
	- Ps:		The point's starting location
	- Pe:		The point's ending location
	- LS:		The line segment
	- Ph:		This will be used to store the intersection point's coordinates (In case there's an intersection)
	- R:		Reflected vector R
	- halfThickness: This is half of the thickness of the line segment

 - Returned value: Intersection time t
	- -1.0f:				If there's no intersection
	- Intersection time:	If there's an intersection
*/
float ReflectAnimatedPointOnStaticThickLineSegment(Vector2D &Ps, Vector2D &Pe, LineSegment2D &LS, Vector2D &Ph, Vector2D &R, float halfThickness)
{
	float ti = AnimatedPointToStaticThickLineSegment(Ps, Pe, LS, Ph, halfThickness);
	if(ti > 0 && ti <= 1)
	{
		Vector2D mI, temp1;
		Vector2DSub(mI, Pe, Ph);
		Vector2DScale(temp1, LS.mN, 2 * Vector2DDotProduct(mI, LS.mN));
		Vector2DSub(R, mI, temp1);
	}
	return ti;
}

/*
This function determines if a static point is inside a static circle 

 - Parameters
	- P:		The static point whose location should be determined
	- Center:	Center of the static circle
	- Radius:	Radius of the static circle

 - Returned value: Boolean
	- True:		If the point and circle are intersecting
	- False:	If the point and circle are not intersecting
*/

bool StaticPointToStaticCircle(Vector2D &P, Vector2D &Center, float Radius)
{
	return Vector2DSquareDistance(P, Center) <= (Radius * Radius);
}

/*
This function determines the distance separating a point from a line

 - Parameters
	- P:		The point whose location should be determined
	- LS:		The line segment

 - Returned value: The distance. Note that the returned value should be:
	- Negative if the point is in the line's inside half plane
	- Positive if the point is in the line's outside half plane
	- Zero if the point is on the line
*/
float StaticPointToStaticLineSegment(Vector2D &P, LineSegment2D &LS)
{
	return Vector2DDotProduct(LS.mN, P) - LS.mNdotP0;
}

/*
This function checks whether an animated point is colliding with a line segment

 - Parameters
	- Ps:		The point's starting location
	- Pe:		The point's ending location
	- LS:		The line segment
	- Ph:		This will be used to store the intersection point's coordinates (In case there's an intersection)

 - Returned value: Intersection time t
	- -1.0f:				If there's no intersection
	- Intersection time:	If there's an intersection
*/
float AnimatedPointToStaticLineSegment(Vector2D &Ps, Vector2D &Pe, LineSegment2D &LS, Vector2D &Ph)
{
	return AnimatedPointToStaticThickLineSegment(Ps, Pe, LS, Ph, 0.0f);
}

/*
This function checks whether an animated circle is colliding with a line segment

 - Parameters
	- Ps:		The center's starting location
	- Pe:		The center's ending location
	- Radius:	The circle's radius
	- LS:		The line segment
	- Ph:		This will be used to store the intersection point's coordinates (In case there's an intersection)

 - Returned value: Intersection time t
	- -1.0f:				If there's no intersection
	- Intersection time:	If there's an intersection
*/
float AnimatedCircleToStaticLineSegment(Vector2D &Ps, Vector2D &Pe, float Radius, LineSegment2D &LS, Vector2D &Ph)
{
	return AnimatedPointToStaticThickLineSegment(Ps, Pe, LS, Ph, Radius);
}

/*
This function reflects an animated point on a line segment.
It should first make sure that the animated point is intersecting with the line 

 - Parameters
	- Ps:		The point's starting location
	- Pe:		The point's ending location
	- LS:		The line segment
	- Ph:		This will be used to store the intersection point's coordinates (In case there's an intersection)
	- R:		Reflected vector R

 - Returned value: Intersection time t
	- -1.0f:				If there's no intersection
	- Intersection time:	If there's an intersection
*/
float ReflectAnimatedPointOnStaticLineSegment(Vector2D &Ps, Vector2D &Pe, LineSegment2D &LS, Vector2D &Ph, Vector2D &R)
{
	return ReflectAnimatedPointOnStaticThickLineSegment(Ps, Pe, LS, Ph, R, 0.0f);
}

/*
This function reflects an animated circle on a line segment.
It should first make sure that the animated point is intersecting with the line 

 - Parameters
	- Ps:		The center's starting location
	- Pe:		The center's ending location
	- Radius:	The circle's radius
	- LS:		The line segment
	- Ph:		This will be used to store the intersection point's coordinates (In case there's an intersection)
	- R:		Reflected vector R

 - Returned value: Intersection time t
	- -1.0f:				If there's no intersection
	- Intersection time:	If there's an intersection
*/
float ReflectAnimatedCircleOnStaticLineSegment(Vector2D &Ps, Vector2D &Pe, float Radius, LineSegment2D &LS, Vector2D &Ph, Vector2D &R)
{
	return ReflectAnimatedPointOnStaticThickLineSegment(Ps, Pe, LS, Ph, R, Radius);
}

/*
This function checks whether an animated point is colliding with a static circle

 - Parameters
	- Ps:		The point's starting location
	- Pe:		The point's ending location
	- Center:	The circle's center
	- Radius:	The circle's radius
	- Pi:		This will be used to store the intersection point's coordinates (In case there's an intersection)

 - Returned value: Intersection time t
	- -1.0f:		If there's no intersection
	- Intersection time:	If there's an intersection
*/


float AnimatedPointToStaticCircle(Vector2D &Ps, Vector2D &Pe, Vector2D &Center, float Radius, Vector2D &Pi)
{
	
	float ti = -1.0f, m, nSquare, radSquare, a, b, c, delta, radSquare2;
	Vector2D Vel, VelNorm, mPsCenter, vTemp1;
	//
	Vector2DSub(Vel, Pe, Ps);
	Vector2DNormalize(VelNorm, Vel);
	Vector2DSub(mPsCenter, Center, Ps);
	m = Vector2DDotProduct(mPsCenter, VelNorm);
	radSquare = Radius * Radius;
	radSquare2 = Vector2DSquareDistance(Ps, Center);
	//
	//	Check whether the circle lies behind the origin of the ray	
	if((m >= 0.0f) || (radSquare2 <= radSquare))
	{
		//output_To_File("circle is not behind the ray origin\n");
		nSquare = Vector2DSquareLength(mPsCenter) - (m * m);
		//
		//	Check whether the ray misses the circle
		if(nSquare <= radSquare)
		{
			//output_To_File("ray doesnt miss the circle\n");
			a = Vector2DDotProduct(Vel, Vel);
			Vector2DScale(vTemp1, mPsCenter, -2);
			b = Vector2DDotProduct(vTemp1, Vel);
			c = Vector2DDotProduct(mPsCenter, mPsCenter) - (Radius * Radius);
			delta = (b * b) - (4 * a * c);
			//
			if(abs(delta) < EPSILON)
			{
				//	The ray is a tangent to the circle, so there is only one intersection point
				ti = -b / (2 * a);
			}
			else if(delta > 0)
			{
				//	The ray intersects with the circle at 2 points. Select the one which is closest
				float ti0 = (-b + sqrt(delta)) / (2 * a);
				float ti1 = (-b - sqrt(delta)) / (2 * a);
				//
				ti = (ti0 < ti1) ? ti0 : ti1;
			}
			//sprintf_s(strBuff, "actual t = %0.3f\n", ti);
			//output_To_File(strBuff);
			if(ti > 0 && ti <= 1)
			{
				// The ray intersects with the circle in the current frame
				Vector2DScaleAdd(Pi, Vel, Ps, ti);
			}
			else
			{
				ti = -1.0f;
			}
		}
	}	
	return ti;
}
/*
float AnimatedPointToStaticCircle(Vector2D &Ps, Vector2D &Pe, Vector2D &Center, float Radius, Vector2D &Pi)
{
	Vector2D v,BC;
	float a,b,c,test,t1,t2;
	Vector2DSub(v,Pe,Ps);
	Vector2DSub(BC,Center,Ps);
	a= Vector2DDotProduct(v,v);
	b= (-2) * Vector2DDotProduct(BC,v );
	c= Vector2DDotProduct(BC,BC)-(Radius*Radius);
	test= (b*b)-(4*a*c);
	if(test<0)
		return -1.0f;
	else 
	{
		if(test<EPSILON && test>-EPSILON)
		{
			t1=(-b)/(2*a);
			if((t1>1) || (t1<0))
				return -1.0f;
			else
			{
			Vector2DScaleAdd(Pi,v,Ps,t1);
			return t1;
			}
		} else if(test>EPSILON)
		{
			t1=((-b) - (sqrt(test))) / (2*a);
			t2=((-b) + (sqrt(test))) / (2*a);
			
			if(t1<t2)
			{
				if((t1>1) || (t1<0))
				return -1.0f;
				else
				{
				Vector2DScaleAdd(Pi,v,Ps,t1);
				return t1;
				}
			}else
			
			{
				if((t2>1) || (t2<0))
				return -1.0f;
				else
				{
				Vector2DScaleAdd(Pi,v,Ps,t2);
				return t2;
				}
			}
		}
	}
}
*/
/*
This function reflects an animated point on a static circle.
It should first make sure that the animated point is intersecting with the circle 

 - Parameters
	- Ps:		The point's starting location
	- Pe:		The point's ending location
	- Center:	The circle's center
	- Radius:	The circle's radius
	- Pi:		This will be used to store the intersection point's coordinates (In case there's an intersection)
	- R:		Reflected vector R

 - Returned value: Intersection time t
	- -1.0f:		If there's no intersection
	- Intersection time:	If there's an intersection
*/
float ReflectAnimatedPointOnStaticCircle(Vector2D &Ps, Vector2D &Pe, Vector2D &Center, float Radius, Vector2D &Pi, Vector2D &R)
{
	float ti = AnimatedPointToStaticCircle(Ps, Pe, Center, Radius, Pi);
	//sprintf_s(strBuff, "Int time = %0.3f\n", ti);
	//output_To_File(strBuff);
	if(ti > 0 && ti <= 1)
	{
		float fTemp1;
		Vector2D m, n;
		//
		Vector2DSub(m, Ps, Pi);
		Vector2DSub(n, Center, Pi);
		Vector2DNormalize(n, n);
		//
		fTemp1 = Vector2DDotProduct(m, n);
		Vector2DScaleSub(R, n, m, 2 * fTemp1);
	}
	return ti;
}

/*
This function checks whether an animated circle is colliding with a static circle

 - Parameters
	- Center0s:		The starting position of the animated circle's center 
	- Center0e:		The ending position of the animated circle's center 
	- Radius0:		The animated circle's radius
	- Center1:		The static circle's center
	- Radius1:		The static circle's radius
	- Pi:			This will be used to store the intersection point's coordinates (In case there's an intersection)

 - Returned value: Intersection time t
	- -1.0f:		If there's no intersection
	- Intersection time:	If there's an intersection
*/
float AnimatedCircleToStaticCircle(Vector2D &Center0s, Vector2D &Center0e, float Radius0, Vector2D &Center1, float Radius1, Vector2D &Pi)
{
	return AnimatedPointToStaticCircle(Center0s, Center0e, Center1, Radius0 + Radius1, Pi);
}

/*
This function reflects an animated circle on a static circle.
It should first make sure that the animated circle is intersecting with the static one 

 - Parameters
	- Center0s:		The starting position of the animated circle's center 
	- Center0e:		The ending position of the animated circle's center 
	- Radius0:		The animated circle's radius
	- Center1:		The static circle's center
	- Radius1:		The static circle's radius
	- Pi:			This will be used to store the intersection point's coordinates (In case there's an intersection)
	- R:			Reflected vector R

 - Returned value: Intersection time t
	- -1.0f:		If there's no intersection
	- Intersection time:	If there's an intersection
*/
float ReflectAnimatedCircleOnStaticCircle(Vector2D &Center0s, Vector2D &Center0e, float Radius0, Vector2D &Center1, float Radius1, Vector2D &Pi, Vector2D &R)
{
	return ReflectAnimatedPointOnStaticCircle(Center0s, Center0e, Center1, Radius0 + Radius1, Pi, R);
}

//////////////////////
// New in project 3 //
//////////////////////

/*
This function checks if the point Pos is colliding with the rectangle
whose center is Rect, width is "Width" and height is Height
*/
bool StaticPointToStaticRect(Vector2D &Pos, Vector2D &Rect, float Width, float Height)
{
	//Calculate the edges of the rectangle
	float &x = Pos.x, &y = Pos.y;
	float top = Rect.y - (Width / 2);
	float bot = Rect.y + (Width / 2);
	float lft = Rect.x - (Height / 2);
	float rgt = Rect.x + (Height / 2);

	return x >= lft && x <= rgt && y >= top && y <= bot;
}

/*
This function checks for collision between 2 circles.
Circle0: Center is Center0, radius is "Radius0"
Circle1: Center is Center1, radius is "Radius1"
*/
bool StaticCircleToStaticCircle(Vector2D &Center0, float Radius0, Vector2D &Center1, float Radius1)
{
	return Vector2DSquareDistance(Center0, Center1) <= ((Radius0 + Radius1) * (Radius0 + Radius1));
}

/*
This functions checks if 2 rectangles are colliding
Rectangle0: Center is pRect0, width is "Width0" and height is "Height0"
Rectangle1: Center is pRect1, width is "Width1" and height is "Height1"
*/
bool StaticRectToStaticRect(Vector2D &Rect0, float Width0, float Height0, Vector2D &Rect1, float Width1, float Height1)
{
	//calculate the edges of both the rectangles
	float top0 = Rect0.y - (Width0 / 2);
	float bot0 = Rect0.y + (Width0 / 2);
	float lft0 = Rect0.x - (Height0 / 2);
	float rgt0 = Rect0.x + (Height0 / 2);
	float top1 = Rect1.y - (Width1 / 2);
	float bot1 = Rect1.y + (Width1 / 2);
	float lft1 = Rect1.x - (Height1 / 2);
	float rgt1 = Rect1.x + (Height1 / 2);

	//check if any of the vertices of Rect0 are inside Rect1
	if( StaticPointToStaticRectLocal(lft0, top0, Rect1, Width1, Height1) ||
		StaticPointToStaticRectLocal(rgt0, top0, Rect1, Width1, Height1) ||
		StaticPointToStaticRectLocal(lft0, bot0, Rect1, Width1, Height1) ||
		StaticPointToStaticRectLocal(rgt0, bot0, Rect1, Width1, Height1))
		return true;

	//check if any of the vertices of Rect1 are inside Rect0
	if( StaticPointToStaticRectLocal(lft1, top1, Rect0, Width0, Height0) ||
		StaticPointToStaticRectLocal(rgt1, top1, Rect0, Width0, Height0) ||
		StaticPointToStaticRectLocal(lft1, bot1, Rect0, Width0, Height0) ||
		StaticPointToStaticRectLocal(rgt1, bot1, Rect0, Width0, Height0))
		return true;

	return false;
}

