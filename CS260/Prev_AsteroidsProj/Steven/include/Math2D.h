// ---------------------------------------------------------------------------
// File Name : Math2D.h
// Author		 : Steven George
// Brief     : Interface file for Math2D
// ---------------------------------------------------------------------------
#ifndef MATH2D_H
#define MATH2D_H

#include "Vector2D.h"

// ---------------------------------------------------------------------------
/*
This function checks if the point P is colliding with the circle whose
center is "Center" and radius is "Radius"
*/
// ---------------------------------------------------------------------------
bool StaticPointToStaticCircle(Vector2D &P, Vector2D &Center, float Radius);

// ---------------------------------------------------------------------------
/*
This function checks if the point Pos is colliding with the rectangle
whose center is Rect, width is "Width" and height is Height
*/
// ---------------------------------------------------------------------------
bool StaticPointToStaticRect(Vector2D &Pos, Vector2D &Rect, float Width, float Height);

// ---------------------------------------------------------------------------
/*
This function checks for collision between 2 circles.
Circle0: Center is Center0, radius is "Radius0"
Circle1: Center is Center1, radius is "Radius1"
*/
// ---------------------------------------------------------------------------
bool StaticCircleToStaticCircle(Vector2D &Center0, float Radius0, Vector2D &Center1, float Radius1);

// ---------------------------------------------------------------------------
/*
This functions checks if 2 rectangles are colliding
Rectangle0: Center is pRect0, width is "Width0" and height is "Height0"
Rectangle1: Center is pRect1, width is "Width1" and height is "Height1"
*/
// ---------------------------------------------------------------------------
bool StaticRectToStaticRect(Vector2D &Rect0, float Width0, float Height0, Vector2D &Rect1, float Width1, float Height1);

#endif