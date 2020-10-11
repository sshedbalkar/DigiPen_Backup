///////////////////////////////////////////////////////////////////////////////////////
//
//	Camera.cpp
//	Authors: Chris Peters
//	Copyright 2010, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#include "Precompiled.h"
#include "Camera.h"
#include "Graphics.h"

namespace Framework
{
	Camera::Camera()
	{
		transform = NULL;
		GRAPHICS->PushCamera(this);
	}

	Camera::~Camera()
	{
		GRAPHICS->PopCamera();
	}

	void Camera::Initialize()
	{
		transform = GetOwner()->has(Transform);
	}


}