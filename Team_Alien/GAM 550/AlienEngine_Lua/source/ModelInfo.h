#pragma once //Makes sure this header is only included once
#include <string>
#include "Model.h"
#include "ModelClass.h"

namespace Framework
{
	
	class ModelInfo
	{
	public:	
		ModelInfo();
		~ModelInfo();
		std::string texture1;
		std::string texture2;
		std::string texture3;
		std::string swaptexture1;
		std::string swaptexture2;
		std::string swaptexture3;
		std::string bin;
		MultiTextureType textureType;
		

	};

	class Model2Info
	{
	public:	
		Model2Info();
		~Model2Info();
		std::string texture1;
		std::string texture2;
		std::string texture3;
		std::string swaptexture1;
		std::string swaptexture2;
		std::string swaptexture3;
		std::string bin;
		BumpType textureType;
		

	};



}


