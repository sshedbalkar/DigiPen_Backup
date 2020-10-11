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
#include "DirectXIncludes.h"
#include "Containers.h"
#include "DebugDiagnostic.h"

//#ifdef _DEBUG
//    #define DEBUG_NEW   new( _NORMAL_BLOCK, __FILE__, __LINE__)
//#else
//    #define DEBUG_NEW
//#endif // _DEBUG
//#include <crtdbg.h>
//#ifdef _DEBUG
//	#define new DEBUG_NEW
//#endif
