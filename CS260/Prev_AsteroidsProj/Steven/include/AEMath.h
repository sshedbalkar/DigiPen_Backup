// ---------------------------------------------------------------------------
// Project Name		:	Alpha Engine
// File Name		:	AEMath.h
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/01/31
// Purpose			:	main header file for the math library
// History			:
// - 2008/01/31		:	- initial implementation
// ---------------------------------------------------------------------------

#ifndef AE_MATH_H
#define AE_MATH_H

// ---------------------------------------------------------------------------

#include "AEVec2.h"
#include "AEMtx33.h"

// ---------------------------------------------------------------------------

#ifndef EPSILON
	#define	EPSILON	0.00001f
#endif

#ifndef PI
	#define	PI		3.1415926535897932384626433832795f
#endif

#define	HALF_PI	(PI * 0.5f)
#define	TWO_PI	(PI * 2.0f)

// ---------------------------------------------------------------------------
#ifdef __cplusplus

extern "C"
{
#endif

AE_API void MathInit();

// ---------------------------------------------------------------------------

AE_API f32 AEDegToRad(f32 x);
AE_API f32 AERadToDeg(f32 x);

AE_API f32 AESin	(f32 x);
AE_API f32 AECos	(f32 x);
AE_API f32 AETan	(f32 x);
AE_API f32 AEASin	(f32 x);
AE_API f32 AEACos	(f32 x);
AE_API f32 AEATan	(f32 x);

#define AESinDeg(x)		AESin(AEDegToRad(x))
#define AECosDeg(x)		AECos(AEDegToRad(x))
#define AETanDeg(x)		AETan(AEDegToRad(x))
#define AEASinDeg(x)	AERadToDeg(AEASin(x))
#define AEACosDeg(x)	AERadToDeg(AEACos(x))
#define AEATanDeg(x)	AERadToDeg(AEATan(x))

// ---------------------------------------------------------------------------

AE_API u32		AEIsPowOf2	(u32 x);
AE_API u32		AENextPowOf2(u32 x);
AE_API u32		AELogBase2	(u32 x);

AE_API f32		AEClamp		(f32 x, f32 x0, f32 x1);
AE_API f32		AEWrap		(f32 x, f32 x0, f32 x1);
AE_API f32		AEMin		(f32 x, f32 y);
AE_API f32		AEMax		(f32 x, f32 y);
AE_API int		AEInRange	(f32 x, f32 x0, f32 x1);

// ---------------------------------------------------------------------------

AE_API f32 AECalcDistPointToCircle		(AEVec2* pPos, AEVec2* pCtr, f32 radius);
AE_API f32 AECalcDistPointToRect		(AEVec2* pPos, AEVec2* pRect, f32 sizeX, f32 sizeY);
AE_API f32 AECalcDistPointToLineSeg	(AEVec2* pPos, AEVec2* pLine0, AEVec2* pLine1);
AE_API f32 AECalcDistPointToConvexPoly	(AEVec2* pPos, AEVec2* pVtx, u32 vtxNum);

AE_API f32 AECalcDistCircleToCircle	(AEVec2* pCtr0, f32 radius0, AEVec2* pCtr1, f32 radius1);
AE_API f32 AECalcDistCircleToRect		(AEVec2* pCtr0, f32 radius0, AEVec2* pRect0, f32 sizeX0, f32 sizeY0);
AE_API f32 AECalcDistCircleToLineSeg	(AEVec2* pPos, AEVec2* pLine0, AEVec2* pLine1);
AE_API f32 AECalcDistCircleToConvexPoly(AEVec2* pPos, AEVec2* pVtx, u32 vtxNum);

AE_API f32 AECalcDistRectToRect	(AEVec2* pRect0, f32 sizeX0, f32 sizeY0, AEVec2* pRect1, f32 sizeX1, f32 sizeY1, AEVec2* pNormal);

// ---------------------------------------------------------------------------

AE_API int AETestPointToCircle	(AEVec2* pPos, AEVec2* pCtr, f32 radius);
AE_API int AETestPointToRect		(AEVec2* pPos, AEVec2* pRect, f32 sizeX, f32 sizeY);

AE_API int AETestCircleToCircle	(AEVec2* pCtr0, f32 radius0, AEVec2* pCtr1, f32 radius1);
AE_API int AETestCircleToRect		(AEVec2* pCtr0, f32 radius0, AEVec2* pRect0, f32 sizeX0, f32 sizeY0);

AE_API int AETestRectToRect		(AEVec2* pRect0, f32 sizeX0, f32 sizeY0, AEVec2* pRect1, f32 sizeX1, f32 sizeY1);

// ---------------------------------------------------------------------------

// sweep a circle with radius 'radius' from ctr0 -> ctr1 againts a point
// * return -ve if circle does not touch the point at any time
AE_API f32 AESweepCircleToPoint	(AEVec2* pCtr0, AEVec2* pCtr1, f32 radius, AEVec2* pP);

// sweep a circle with radius 'radius' from ctr0 -> ctr1 againts a line segment
// * return -ve if circle does not intersect the line segment at any time
AE_API f32 AESweepCircleToLineSeg	(AEVec2* pCtr0, AEVec2* pCtr1, f32 radius, AEVec2* pP0, AEVec2* pP1, AEVec2* pN);


//TO TEST
//Sweeps a moving point against a static line
AE_API f32 AESweepPointToLine		(AEVec2 *pPos, AEVec2 *pVel, AEVec2 *pPnt, AEVec2 *pDirection);

//TO TEST
//Sweeps a moving circle against a static line
AE_API f32 AESweepCircleToLine		(AEVec2 *pCtr, f32 radius, AEVec2 *pVel, AEVec2 *pPnt, AEVec2 *pDirection);

//TO TEST
//Reflects a moving point on a static line. Returns 0 if there is no
//collision between the point and the line.
AE_API int AEReflectPointOnLine	(AEVec2 *pPos, AEVec2 *pVel, AEVec2 *pPnt, AEVec2 *pDirection, AEVec2 *pNewPosition, AEVec2 *pNewVelocity);


//TO TEST
//Reflects a moving circle on a static line. Returns 0 if there is no
//collision between the circle and the line.
AE_API int AEReflectCircleOnLine	(AEVec2 *pCtr, f32 radius, AEVec2 *pVel, AEVec2 *pPnt, AEVec2 *pDirection, AEVec2 *pNewPosition, AEVec2 *newVelocity);

// ---------------------------------------------------------------------------

#ifdef __cplusplus 
}
#endif

// ---------------------------------------------------------------------------

#endif // AE_MATH_H
