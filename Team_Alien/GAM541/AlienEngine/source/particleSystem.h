#pragma once

#include <D3D10.h>
#include "ObjectLinkedList.h"
#include "VMath.h"

namespace Framework
{

class ParticleEmitter;
class Emitter;
class D3DManager;

class ParticleSystem
{
protected:
	typedef ObjectLinkList<Emitter> EmitterListType;
	///particle vertex format
	struct ParticleVertex
	{
		Vec3 v0;
		Vec3 v1;
		Vec3 v2;
		unsigned int type;
	};

protected:
	EmitterListType emitters;
	ID3D10Device* device;
	ID3D10Buffer* vb_launcher;

public:
	ParticleSystem( ID3D10Device* _device ) : device(_device) {};
	virtual ~ParticleSystem() {};

	virtual void Initialize() = 0;
	virtual void Update( float timeslice ) = 0;
	virtual void Draw( Mat4& mat_view, Mat4& mat_proj ) = 0;
	virtual void Free() = 0;
	virtual void Load() {}
	virtual void Unload() {}

	virtual void RegisterEmitter( Emitter* emitter );
	virtual void RemoveEmitter( Emitter* emitter );

protected:
	void InitializeLauncherVB( int num_launchers );

};

};
