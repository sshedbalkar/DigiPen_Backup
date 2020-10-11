///////////////////////////////////////////////////////////////////////////////////////
//
//	Transform.cpp - The Transform Component
//	Authors: 
//	Copyright 2010, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#include "Precompiled.h"
#include "Transform.h"
#include "DebugTools.h"
#include "Utility.h"

namespace Framework
{
	Transform::Transform()
	{
		Position = Vec3(0,0,0);
		Scale = Vec3(1,1,1);
		/*Rotation = 0.0f;
		RotationX = 0.0f;
		RotationZ = 0.0f;*/
		QUATERNION = D3DXQUATERNION(0,0,0,1);
	}

	void Transform::Serialize(ISerializer& stream)
	{
		//StreamRead(stream,Position);
		//StreamRead(stream,Rotation);
		//
		float arr[3];
		ISerializer* str = stream.clone();
		if(str->isGood())
		{
			str->readProperty("Position", *str);
			str->readValue("value", arr, 3);
			Position.x = arr[0];
			Position.y = arr[1];
			Position.z = arr[2];
			//
			str->goToParent();
			/*if(str->readProperty("Rotation", *str))
			{
				if(str->readValue("value", Rotation))
				{
					Rotation *= (float)D3DX_PI/180.0f;
				}
			}
			str->goToParent();*/
			if(str->readProperty("Scale", *str))
			{
				//str->readValue("value", Scale);
				str->readValue("value", arr, 3);
				Scale.x = arr[0];
				Scale.y = arr[1];
				Scale.z = arr[2];
			}
			//std::cout<<"Transform serialized: Pos: " <<Position << ", rot: "<<Rotation<<", Scale: "<<Scale<<"\n";
		}
		delete str;
	}
	bool Transform::Deserialize(ISerializer& stream)
	{
		const int length = 100;
		char str[length];
		ISerializer* strm = stream.clone();
		strm->writeNode( "Component" );
		strm->readProperty( "Component", *strm );
		//
		strm->writeProperty( "Type", GetName() );
		//
		sprintf_s( str, length, "%4.2f,%4.2f,%4.2f", Position.x, Position.y, Position.z );
		strm->writeNodeAndValue( "Position", str );
		//
		sprintf_s( str, length, "%4.2f,%4.2f,%4.2f", Scale.x, Scale.y, Scale.z );
		strm->writeNodeAndValue( "Scale", str );
		//
		//sprintf_s( str, length, "%3.4f,%3.4f,%3.4f,%3.4f", rotation.x, rotation.y, rotation.z , rotation.w);
		//strm->writeNodeAndValue( "Rotation", str );
		//
		delete strm;
		//
		return true;
	}
	//void Transform::BuildMatrix( Mat4& mat )
	//{
	//	Mat4 mat_rotation;
	//	Mat4 mat_scale;
	//	D3DXMatrixRotationYawPitchRoll( &mat_rotation, Rotation, RotationX, RotationZ );
	//	D3DXMatrixScaling( &mat_scale, Scale.x, Scale.y, Scale.z );
	//	//std::cout <<Scale.x <<" "<<Scale.y <<" "<<Scale.z<<std::endl;
	//	D3DXMatrixMultiply( &mat, &mat_rotation, &mat_scale );
	//	mat._41 = Position.x;
	//	mat._42 = Position.y;
	//	mat._43 = Position.z;

	//	//std::cout <<mat._11<<" "<<mat._12<<" "<<mat._13<<" "<<mat._14<<std::endl;
	//	//std::cout <<mat._21<<" "<<mat._22<<" "<<mat._23<<" "<<mat._24<<std::endl;
	//	//std::cout <<mat._31<<" "<<mat._32<<" "<<mat._33<<" "<<mat._34<<std::endl;
	//	//std::cout <<mat._41<<" "<<mat._42<<" "<<mat._43<<" "<<mat._44<<std::endl<<std::endl;
	//}

	//void Transform::BuildMatrixNoScale( Mat4& mat )
	//{
	//	D3DXMatrixRotationYawPitchRoll( &mat, Rotation, RotationX, RotationZ );
	//	mat._41 = Position.x;
	//	mat._42 = Position.y;
	//	mat._43 = Position.z;
	//}
	
	float Transform::GetArea()
	{
		return Scale.x * Scale.y;
	}

	Vec4 TransformIntoQuaterion(float DoubleAngle, const Vec3& axis ){
		float sine = sin(DoubleAngle);
		return Vec4(sine*axis.x, sine*axis.y, sine*axis.z, cos(DoubleAngle));
	}
}



