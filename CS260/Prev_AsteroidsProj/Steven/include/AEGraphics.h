// ---------------------------------------------------------------------------
// Project Name		:	Alpha Engine
// File Name		:	AEGraphics.h
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/01/30
// Purpose			:	Graphics component header file
// History			:
// - 2008/01/30		:	- initial implementation
// ---------------------------------------------------------------------------

#ifndef AE_GRAPHICS_H
#define AE_GRAPHICS_H

// ---------------------------------------------------------------------------
// Defines



enum AEGfxBlendMode
{
	AE_GFX_BM_NONE = 0, 
	AE_GFX_BM_BLEND,  
	AE_GFX_BM_ADD,  

	AE_GFX_BM_NUM
};

// ---------------------------------------------------------------------------



enum AEGfxTextureMode
{
	AE_GFX_TM_PRECISE = 0,
	AE_GFX_TM_AVERAGE
};


// ---------------------------------------------------------------------------
// Struct/Class definitions

typedef struct AEGfxTriList
{
	LPDIRECT3DVERTEXBUFFER9 vtxBuffer;
	u32						vtxNum;
}AEGfxTriList;

// ---------------------------------------------------------------------------

typedef struct AEGfxTexture
{
	LPDIRECT3DTEXTURE9 mTexture;
	char mpName[256];
}AEGfxTexture;

// ---------------------------------------------------------------------------
// Extern variables

extern AE_API f32	gAEWinMinX; 
extern AE_API f32	gAEWinMinY; 
extern AE_API f32	gAEWinMaxX; 
extern AE_API f32	gAEWinMaxY;

// ---------------------------------------------------------------------------
// Function prototypes
// ---------------------------------------------------------------------------

#ifdef __cplusplus

extern "C"
{
#endif

// ---------------------------------------------------------------------------

AE_API int AEGfxInit();
AE_API void AEGfxReset();
AE_API void AEGfxUpdate();
AE_API void AEGfxExit();

AE_API void AEGfxStart();
AE_API void AEGfxEnd();

// Sets/Gets the camera position
AE_API void AEGfxSetCamPosition(f32 X, f32 Y);
AE_API void AEGfxGetCamPosition(f32 *pX, f32 *pY);

// Sets/Gets the camera distance, used to zoom in/out
AE_API void AEGfxSetCamZoom(f32 Distance);
AE_API f32 AEGfxGetCamZoom();

// function to set the position (translation)
AE_API void AEGfxSetPosition(f32 X, f32 Y);

// function to set the current transformation matrix to use
AE_API void AEGfxSetTransform(AEMtx33* pMtx);

/*
void			AEGfxSetBlendMode(AEGfxBlendMode blendMode);
AEGfxBlendMode  AEGfxGetBlendMode();
*/
// functions to create a mesh

AE_API void			AEGfxTriStart	();
AE_API void			AEGfxTriAdd		(f32 x0, f32 y0, u32 c0, f32 tu0, f32 tv0,
								 f32 x1, f32 y1, u32 c1, f32 tu1, f32 tv1,
								 f32 x2, f32 y2, u32 c2, f32 tu2, f32 tv2);
/*void			AEGfxTriAdd		(f32 x0, f32 y0, f32 z0, u32 c0, 
								 f32 x1, f32 y1, f32 z1, u32 c1, 
								 f32 x2, f32 y2, f32 z2, u32 c2);
 */
AE_API AEGfxTriList*	AEGfxTriEnd		();
AE_API void				AEGfxTriDraw	(AEGfxTriList* pTriList);
AE_API void				AEGfxTriFree	(AEGfxTriList* pTriList);


AE_API AEGfxTexture*	AEGfxTextureLoad(char *pFileName);
AE_API void				AEGfxTextureUnload(AEGfxTexture *pTexture);
AE_API void				AEGfxTextureSet(AEGfxTexture *pTexture);			// Use NULL for 'no texture'


AE_API AEGfxTexture*	AEGfxTextureLoadFromMemory(unsigned long *pColors, unsigned int Width, unsigned int Height);
AE_API void				AEGfxSaveTextureToFile(AEGfxTexture* pTexture, char *pFileName);

AE_API void				AEGfxSetTextureMode(enum AEGfxTextureMode TM);

// functions to draw simple primitives
// * these are slow!!! use with caution!!
// * DO NOT call any of these function while building mesh using above functions
AE_API void			AEGfxPoint		(f32 x0, f32 y0, f32 z0, u32 c0);
AE_API void			AEGfxLine		(f32 x0, f32 y0, f32 z0, u32 c0, f32 x1, f32 y1, f32 z1, u32 c1);
AE_API void			AEGfxTri		(f32 x0, f32 y0, f32 z0, u32 c0, 
								 f32 x1, f32 y1, f32 z1, u32 c1, 
								 f32 x2, f32 y2, f32 z2, u32 c2);
AE_API void			AEGfxQuad		(f32 x0, f32 y0, f32 z0, u32 c0, 
								 f32 x1, f32 y1, f32 z1, u32 c1, 
								 f32 x2, f32 y2, f32 z2, u32 c2, 
								 f32 x3, f32 y3, f32 z3, u32 c3);
AE_API void			AEGfxBox		(f32 x0, f32 y0, f32 z0, f32 sizeX, f32 sizeY, f32 sizeZ, u32 c0, u32 c1);
AE_API void			AEGfxSphere		(f32 x0, f32 y0, f32 z0, f32 radius, u32 c0, u32 c1, u32 division);
AE_API void			AEGfxCone		(f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1, f32 radius, u32 c0, u32 c1, u32 division);
AE_API void			AEGfxAxis		(f32 scale);

AE_API u32				AEGfxColInterp	(u32 c0, u32 c1, f32 t);

// font related functions
AE_API void			AEGfxPrint		(s32 x, s32 y, u32 color, s8* pStr);

// ---------------------------------------------------------------------------

#ifdef __cplusplus 
}
#endif

// ---------------------------------------------------------------------------

#endif // AE_GRAPHICS_H

