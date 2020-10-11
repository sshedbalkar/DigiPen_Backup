#pragma once //Makes sure this header is only included once

#include<iostream>
#include<vector>
#include "VMath.h"

namespace Framework
{
	class CameraControl
	{
		private:
			std::vector<Vec3> camData;
		public:
			CameraControl(){}
			~CameraControl(){}
			void Initialize();
			void Update(float dt);
	};
}