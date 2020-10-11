#pragma once

#include "particleSystem.h"

namespace Framework
{
	
class Shader;

class PEClouds : public ParticleSystem
{
	Shader* advance_particles;
	Shader* render_particles;
	ID3D10EffectShaderResourceVariable* random_texture_ptr;
	ID3D10EffectScalarVariable* global_time_ptr;
	ID3D10EffectScalarVariable* elapsed_time_ptr;
	ID3D10EffectVectorVariable* screen_range_ptr;

public:
	PEClouds( ID3D10Device* _device );
	virtual ~PEClouds();

	virtual void Initialize();
	virtual void Update( float timeslice );
	virtual void Draw( Mat4& mat_view, Mat4& mat_proj );
	virtual void Free();

};


}