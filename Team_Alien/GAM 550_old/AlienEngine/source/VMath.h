///////////////////////////////////////////////////////////////////////////////////////
///
///	\file VMath.h
///	Typedefs the DirectX Extension math library and provides some utility functions.
///
///	Authors: 
///	Copyright 2010, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////////////
#pragma once //Makes sure this header is only included once

//Include our math headers
//#include "Serialization.h"
//#ifdef _DEBUG
//    #define DEBUG_NEW   new( _NORMAL_BLOCK, __FILE__, __LINE__)
//#else
//    #define DEBUG_NEW
//#endif // _DEBUG
//#include <crtdbg.h>
//#ifdef _DEBUG
//	#define new DEBUG_NEW
//#endif
#include <D3DX10math.h>

namespace Framework
{
#define EPSILON 0.0001
	typedef D3DXVECTOR2 Vec2;
	typedef D3DXVECTOR3 Vec3;
	typedef D3DXVECTOR4 Vec4;
	typedef D3DXMATRIXA16 Mat4;

	inline float max( float a, float b )
	{
		return a > b ? a : b;
	}
	inline float min( float a, float b )
	{
		return a > b ? b : a;
	}
	inline int max( int a, int b )
	{
		return a > b ? a : b;
	}
	inline int min( int a, int b )
	{
		return a > b ? b : a;
	}

	inline float dot( Vec2& a, Vec2& b )
	{
		return a.x * b.x + a.y * b.y;
	}

	inline float lengthSquared(const Vec2& a)
	{
		return a.x * a.x + a.y * a.y;
	}
	inline float lengthSquared(const Vec3& a)
	{
		return a.x * a.x + a.y * a.y + a.z * a.z;
	}
	inline float lengthSquared(const Vec4& a)
	{
		return a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w;
	}
	//
	inline float normalize(Vec2& a)
	{
		float len = sqrt(lengthSquared(a));
		if(abs(len) > EPSILON)
		{
			a /= len;
		}
		return len;
	}
	inline float normalize(Vec3& a)
	{
		float len = sqrt(lengthSquared(a));
		if(abs(len) > EPSILON)
		{
			a /= len;
		}
		return len;
	}
	inline float normalize(Vec4& a)
	{
		float len = sqrt(lengthSquared(a));
		if(abs(len) > EPSILON)
		{
			a /= len;
		}
		return len;
	}

	inline float Rand( float lower, float higher )
	{
		float ret = (float)(rand()%10000) / 9999.0f;
		ret *= ( higher - lower );
		ret += lower;
		return ret;
	}

	inline float GaussianRand( float expectation, float deviation )
	{
		float r0 = (float)(rand()%10000) / 5000.0f;
		float r1 = (float)(rand()%10000) / 5000.0f;
		float r2 = (float)(rand()%10000) / 5000.0f;

		float result = ( r0 + r1 + r2 ) / 3.0f;
		result -= 1.0f;
		result *= deviation;
		result += expectation;
		return result;
	}
	
	#define ABS(a) (((a) < 0) ? -(a) : (a))
	inline bool LineIntersect( float & x1, float & y1, float & x2, float & y2, float & x3, float & y3, float & x4, float & y4 )
	{
		float y4y3 = y4 - y3;
		float y1y3 = y1 - y3;
		float y2y1 = y2 - y1;
		float x4x3 = x4 - x3;
		float x2x1 = x2 - x1;
		float x1x3 = x1 - x3;
		float denom  = y4y3 * x2x1 - x4x3 * y2y1;
		float numera = x4x3 * y1y3 - y4y3 * x1x3;
		float numerb = x2x1 * y1y3 - y2y1 * x1x3;

		const float eps = 0.0001f;
		if (ABS(numera) < eps && ABS(numerb) < eps && ABS(denom) < eps)
		{	//Lines coincident (on top of each other)
			return true;
		}

		if (ABS(denom) < eps)
		{	//Lines parallel
			return false;
		}

		float mua = numera / denom;
		float mub = numerb / denom;
		if (mua < 0 || mua > 1 || mub < 0 || mub > 1)
		{	//No intersection
			return false;
		}
		else
		{	//Intersection
			return true;
		}
	}

	typedef unsigned int uint;
}
