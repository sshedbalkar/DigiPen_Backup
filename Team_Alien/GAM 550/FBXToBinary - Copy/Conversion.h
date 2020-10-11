///////////////////////////////////////////////////////////////////////////////
//
//	Conversion.h
//	Converts homgeneous transform matrices between coordinate systems.
//
//	Authors: Chris Peters
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////
#pragma once

class Converter
{
public:	
	Converter(FbxScene* pScene);
	void ConvertMatrix( FbxAMatrix& sourceMatX);
	void ConvertMeshMatrix( FbxAMatrix& m);
	FbxMatrix ConversionMatrix;
	bool NeedToChangedWinding;
};
