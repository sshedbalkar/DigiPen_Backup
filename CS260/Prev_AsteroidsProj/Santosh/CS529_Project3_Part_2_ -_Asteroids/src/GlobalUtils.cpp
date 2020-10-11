/* Start Header -------------------------------------------------------
Copyright (C) 20xx DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name		: Global.cpp
Purpose			: Place to keep global data and global functions
Language		: C++/MS C++ 2010 Express
Platform		: Microsoft Visual C++ 2010/i7 2630QM, 8GB DDR3 1600MHz, 3GB NVIDIA GT 555M/Windows 7 Home Premium
Project			: CS529_SantoshShedbalkar_Project1_1
Author			: Santosh Shedbalkar/santosh.s/60001111
Creation date	: 09/19/2011
- End Header --------------------------------------------------------*/

#include "GlobalUtils.h"

// ---------------------------------------------------------------------------
// defines output stream where the output will be written

FILE *outputStream = 0;
bool outputOK;
// ----------------------------------------------------------------------------

int openOutputStream(void);
int closeOutputStream(void);

// ----------------------------------------------------------------------------
int Global_initialize(void)
{
	if(openOutputStream() == CODE_SUCCESS)
	{
		outputOK = true;
		output_To_File("GlobalUtils: Initialize\n");
		return CODE_SUCCESS;
	}
	return CODE_FAILURE;
}
int openOutputStream(void)
{
	FILE *file;
	int returnCode = CODE_SUCCESS;
	//
	if (!(file = fopen("Output.txt", "wb" )))
	{
		std::cout << "[ERROR] GlobalUtils::Global_initialize : File \"Output.txt\" could not be opened for reading/writing \n";
		returnCode = CODE_FAILURE;
	} 
	else 
	{
		outputStream = file;
	}
	return returnCode;
}
int Global_destroy(void)
{
	output_To_File("GlobalUtils: Exit");
	return closeOutputStream();
}
int closeOutputStream(void)
{
	if(fclose(outputStream) == EOF)
	{
		std::cout << "[ERROR] GlobalUtils::Global_destroy : File \"Output.txt\" could not be closed\n";
		return CODE_FAILURE;
	}
	else
	{
		outputStream = 0;
		return CODE_SUCCESS;
	}
}
void resetOutputStream(void)
{
	closeOutputStream();
	openOutputStream();
}
// ---------------------------------------------------------------------------
// function that writes a string to an output stream

int output_To_File(const char* string)
{
	if(outputOK)
	{
		if(outputStream)
		{
			if(fputs(string, outputStream) == EOF)
			{
				std::cout << "[ERROR] Global::output_To_File : Error writing to output stream\n";
				return CODE_FAILURE;
			}
			else
			{
				fflush(outputStream);
				std::cout << string;
				return CODE_SUCCESS;
			}
		}
	}
}