///////////////////////////////////////////////////////////////////////////////////////
///
///	\file WindowsIncludes.h
///	Include the needed parts of the windows header.
///	
///	Authors: 
///	Copyright 2010, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////////////
// prevent min/max from being #defined
#define NOMINMAX


// windows xp
#define WINVER 0x0500
#define _WIN32_WINNT 0x0500

// internet explorer 6
#define _WIN32_IE 0x0600

#include <windows.h>
#include <mmsystem.h>
#include <WinDef.h>