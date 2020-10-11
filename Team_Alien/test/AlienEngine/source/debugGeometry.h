/*////////////////////////////////////////////////////////////////////////////////////
//
//	DebugGeometry.h
//
//	Authors: Zheng
//	Instruction:
		Create component DebugGeometry and call Initialize() during loading time.
		After that, you can modify the debug_shape_list when needed( Push new shapes or remove them from the list ).
		Call Free() before delete the entire component.
		
	example( after Creation and Initialization of DebugGeometry Component, add and maintain a debug rectangle ):
		add+initialize:
			DebugGeometry::DebugShapeData;
			shape.type = DebugGeometry::DEBUG_SHAPE_RECTANGLE;
			shape.v0 = Vec3( 0.0f,0.0f,0.0f );
			shape.v1 = Vec3( 0.5f,1.0f,0.0f );
			shape.color = Vec4( 0.0f,0.0f,0.0f,1.0f );
			debug_geometry->debug_shape_list.push_back( shape );

		maintain:
			DebugGeometry::DebugShapeData* shape = &debug_geometry->debug_shape_list[0];
			shape->v1 = Vec3( 1.0f,1.0f,1.0f );

//
/////////////////////////////////////////////////////////////////////////////////////*/
#pragma once

#include "VMath.h"
#include "Component.h"
#include <vector>


namespace Framework
{

	class DebugGeometry : public GameComponent
	{
	public:
		enum DebugShapeType
		{
			DEBUG_SHAPE_LINESEGMENT=0,	//// use v0,v1 to indicate two point that defines a line seg
			DEBUG_SHAPE_ARROW,			//// line segment with direction, use v0 for the start point, v1 for the end point
			DEBUG_SHAPE_RECTANGLE,		//// 2d rectangle with depth, v0 defines the bottom left point and the depth,
										//// while first two elements of v1 define the width and height
			DEBUG_SHAPE_CIRCLE,			//// 2d circle with depth, v0 defines center and depth, first element of v1 defines the radius
			DEBUG_SHAPE_PRISM			//// axis-aligned rectangular prism, v0 defines ( -x,-y,-z ), v1 defines( +x,+y,+z )
		};
		struct DebugShapeData
		{
			DebugShapeType type;
			Vec3 v0;
			Vec3 v1;
			Vec4 color;
		};
		typedef std::vector<DebugShapeData> DebugShapeListType;
	
	public:
		DebugShapeListType debug_shape_list;

	public:
		DebugGeometry() {};
		~DebugGeometry() {};

		virtual void Initialize();
		virtual void Free();
		virtual void SendMessage(Message *){};
		virtual void Serialize(ISerializer& str){};
		virtual void Load(){};
		virtual void Unload(){};

	};

}
