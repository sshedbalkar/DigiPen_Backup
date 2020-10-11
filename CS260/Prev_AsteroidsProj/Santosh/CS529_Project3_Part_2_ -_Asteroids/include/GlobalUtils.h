/* Start Header -------------------------------------------------------
Copyright (C) 20xx DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name		: Global.h
Purpose			: Application Global header file
Language		: C++/MS C++ 2010 Express
Platform		: Microsoft Visual C++ 2010/i7 2630QM, 8GB DDR3 1600MHz, 3GB NVIDIA GT 555M/Windows 7 Home Premium
Project			: CS529_SantoshShedbalkar_Project1_1
Author			: Santosh Shedbalkar/santosh.s/60001111
Creation date	: 09/19/2011
- End Header --------------------------------------------------------*/

#include <stdio.h>
#include <iostream>

// ---------------------------------------------------------------------------
// enumeration of values returned by functions to indicate their success/failure

enum FUNCT_RETURN_CODES
{
	CODE_SUCCESS,
	CODE_FAILURE
};


// ---------------------------------------------------------------------------

extern FILE *outputStream;
extern bool outputOK;

// ---------------------------------------------------------------------------
// global functions defined by Global.cpp

int Global_initialize(void);
int Global_destroy(void);
int output_To_File(const char* string);
void resetOutputStream(void);
