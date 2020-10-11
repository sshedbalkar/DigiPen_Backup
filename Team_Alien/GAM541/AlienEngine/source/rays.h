#pragma once //Makes sure this header is only included once

#include "Graphics.h"
#include "Physics.h"
#include "Engine.h"
#include "GameData.h"
#include "VMath.h"
//
namespace Framework
{

	enum Colors{
		COLORS_Normal=0,
		COLORS_Connecting,
		COLORS_Teleporting
	};
	
	class Rays
	{
		Vec4 m_BeamColor;
		LineListType* line_list;
		LineListType* line_list_boss;
		int numberOfRays;
		GOC* lastbody;
		GOC* BossLastBody;
		bool m_IsRayTouchingTeleportable;
		float m_TimeToTeleport;
		float m_currentTimer;
		bool m_AllowTeleport;
		bool m_AllowDrawRays;
		float m_BossTimer;
	public:
		Rays();
		~Rays(){delete line_list; delete line_list_boss;}
		bool IsRayTouchingTeleportable(){return m_IsRayTouchingTeleportable;}
		void Update(Vec2 heroPos,Vec2 mousePos,float);
		void DestroyRays();
		void StartTeleportation(bool);
		void CollisionCheck();
		void BossRayCollisionCheck();
		void Teleport();
		float FindX(float y);
		float FindY(float x);
		float FindX1(float y);
		float FindY1(float x);
		void SetNewBeamColor(Colors c);
		float SquaredDistance(Vec2 a,Vec2 b);
		bool IsLastBodyABlock();
		void SetAllowDrawRays(bool b){m_AllowDrawRays=b;}
		bool IsAllowedDrawRays( ){return m_AllowDrawRays;}
	};

	extern Rays *RAYS;
}


