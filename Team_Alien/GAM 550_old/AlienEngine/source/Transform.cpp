///////////////////////////////////////////////////////////////////////////////////////
//
//	Transform.cpp - The Transform Component
//	Authors: 
//	Copyright 2010, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#include "Precompiled.h"
#include "Transform.h"
#include "Utility.h"

namespace Framework
{
	Transform::Transform()
	{
		Position = Vec3(0,0,0);
		Scale = Vec3(1,1,1);
		rotation = D3DXQUATERNION( 0.0, 0.0, 0.0, 1.0 );
	}

	void Transform::Serialize(ISerializer& stream)
	{
		//StreamRead(stream,Position);
		//StreamRead(stream,Rotation);
		//
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
		sprintf_s( str, length, "%3.4f,%3.4f,%3.4f,%3.4f", rotation.x, rotation.y, rotation.z , rotation.w);
		strm->writeNodeAndValue( "Rotation", str );
		//
		delete strm;
		//
		return true;
	}

	void Transform::BuildMatrix( D3DXMATRIX& mat )
	{
		//D3DXMATRIX mat_rotation;
		//D3DXMATRIX mat_scale;
		D3DXMatrixRotationQuaternion( &mat, &rotation );
		//D3DXMatrixScaling( &mat_scale, Scale.x, Scale.y, Scale.z );
		//std::cout <<Scale.x <<" "<<Scale.y <<" "<<Scale.z<<std::endl;
		//D3DXMatrixMultiply( &mat, &mat_rotation, &mat_scale );
		//D3DXMatrixIdentity( &mat );
		mat._41 = Position.x;
		mat._42 = Position.y;
		mat._43 = Position.z;
		D3DXMatrixTranspose( &mat, &mat );
		//std::cout <<mat._11<<" "<<mat._12<<" "<<mat._13<<" "<<mat._14<<std::endl;
		//std::cout <<mat._21<<" "<<mat._22<<" "<<mat._23<<" "<<mat._24<<std::endl;
		//std::cout <<mat._31<<" "<<mat._32<<" "<<mat._33<<" "<<mat._34<<std::endl;
		//std::cout <<mat._41<<" "<<mat._42<<" "<<mat._43<<" "<<mat._44<<std::endl<<std::endl;
	}
		
	float Transform::GetArea()
	{
		return Scale.x * Scale.y;
	}
}



