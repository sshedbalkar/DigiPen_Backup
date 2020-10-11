///////////////////////////////////////////////////////////////////////////////////////
///
///	\file Precompiled.h
///	Precompiled Header file which includes the large unchanging header of stl, windows
///	DirectX. These are 'precompiled once' and used for every cpp. This helps to 
///	greatly reduce compile times.
///
///	Authors: 
///	Copyright 2010, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////////////
#include "WindowsIncludes.h"
//#include "DirectXIncludes.h"
#include <D3D11.h>
#include <D3DX10math.h>
#include "Containers.h"
#include "DebugDiagnostic.h"
#include <iostream>
#include <sstream>
#include "../AntTweakBar/include/AntTweakBar.h"

#ifdef _DEBUG
    #define DEBUG_NEW   new( _NORMAL_BLOCK, __FILE__, __LINE__)
	#include <crtdbg.h>
#else
    #define DEBUG_NEW
#endif // _DEBUG
#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

#define ALLOW_DRAGNDROP


#define S_DELETE(x)			{if(x) delete(x);x=NULL;}
#define S_RELEASE(x)		{if(x) x->Release();x=NULL;}