#pragma once //Makes sure this header is only included once

#include<vector>
#include "VMath.h"
#include "Body.h"

namespace Framework
{
	class CheckPoints
	{
		private:
			std::vector<Vec3> checkPData;
		public:
			CheckPoints(){}
			~CheckPoints(){}
			void Initialize();
			void Load();
	};
}