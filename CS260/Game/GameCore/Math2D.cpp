// ---------------------------------------------------------------------------
// File Name : Math2D.cpp
// Author		 : Steven George
// Brief     : Implementation for Math2D
// ---------------------------------------------------------------------------
#include "Math2D.h"

// ---------------------------------------------------------------------------
/*
This function checks if the point P is colliding with the circle whose
center is "Center" and radius is "Radius"
*/
// ---------------------------------------------------------------------------
bool StaticPointToStaticCircle(Vector2D &P, Vector2D &Center, float Radius)
{
  if (((P.x - Center.x)*(P.x - Center.x) + (P.y - Center.y)*(P.y - Center.y))	< Radius * Radius)
  {
		return true;
  }

	else
  {
		return false;
  }
}

// ---------------------------------------------------------------------------
/*
This function checks if the point Pos is colliding with the rectangle
whose center is Rect, width is "Width" and height is Height
*/
// ---------------------------------------------------------------------------
bool StaticPointToStaticRect(Vector2D &Pos, Vector2D &Rect, float Width, float Height)
{
  if(Pos.x <= Rect.x || Pos.x >= (Rect.x + Width) || 
     Pos.y <= Rect.y || Pos.y >= (Rect.y + Height))
  {
    return false;
  }

  return true;
}

// ---------------------------------------------------------------------------
/*
This function checks for collision between 2 circles.
Circle0: Center is Center0, radius is "Radius0"
Circle1: Center is Center1, radius is "Radius1"
*/
// ---------------------------------------------------------------------------
bool StaticCircleToStaticCircle(Vector2D &Center0, float Radius0, Vector2D &Center1, float Radius1)
{
  float r = (Radius0 + Radius1) * (Radius0 + Radius1);

  if(((Center0.x - Center1.x)*(Center0.x - Center1.x) + (Center0.y - Center1.y) * (Center0.y - Center1.y)) <= r)
  {
		return true;
  }

	return false;
}

// ---------------------------------------------------------------------------
/*
This functions checks if 2 rectangles are colliding
Rectangle0: Center is pRect0, width is "Width0" and height is "Height0"
Rectangle1: Center is pRect1, width is "Width1" and height is "Height1"
*/
// ---------------------------------------------------------------------------
bool StaticRectToStaticRect(Vector2D &Rect0, float Width0, float Height0, Vector2D &Rect1, float Width1, float Height1)
{
  float left0   = Rect0.x;
  float right0  = Rect0.x + Width0;
  float top0    = Rect0.y;
  float bottom0 = Rect0.y + Height0;

  float left1   = Rect1.x;
  float right1  = Rect1.x + Width1;
  float top1    = Rect1.y;
  float bottom1 = Rect1.y + Height1;

  return !(left1   > right0  ||
           right1  < left0   ||
           top1    > bottom0 ||
           bottom1 < top0);
}
