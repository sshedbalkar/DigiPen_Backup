///////////////////////////////////////////////////////////////////////////////////////
///
///	\file Transform.h 
/// Defines the Transform Component.
///
///	Authors: 
///	Copyright 2010, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////////////
#pragma once //Makes sure this header is only included once

#include "VMath.h"
#include "Composition.h"

namespace Framework
{
	///The transform component provides a shared position and rotation.
	class Transform : public GameComponent
	{
	public:	
		Transform();
		Vec3 Position;
		/*float Rotation;
		float RotationX;
		float RotationZ;*/
		D3DXQUATERNION QUATERNION;
		Vec3 Scale;
		void Serialize(ISerializer& str);
		bool Deserialize(ISerializer& str);

		void BuildMatrix( Mat4& mat );
		void BuildMatrixNoScale( Mat4& mat );
		float GetArea();
	};

	Vec4 TransformIntoQuaterion(float DoubleAngle, const Vec3& axis );
}
