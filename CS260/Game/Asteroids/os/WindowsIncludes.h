///////////////////////////////////////////////////////////////////////////////////////
///
///	\file WindowsIncludes.h
///	Include the needed parts of the windows header.
///	
///	Authors: Chris Peters
///	Copyright 2010, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////////////
// prevent min/max from being #defined
#pragma once
//#define NOMINMAX
//#define WIN32_LEAN_AND_MEAN
// prevent winsock1 from being included
//#define _WINSOCKAPI_

// windows xp
//#define WINVER 0x0500
//#define _WIN32_WINNT 0x0500

// internet explorer 6
//#define _WIN32_IE 0x0600
#include <winsock2.h>
#include <Windows.h>



//#include <mmsystem.h>