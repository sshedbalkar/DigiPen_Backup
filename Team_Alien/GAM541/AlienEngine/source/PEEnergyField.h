#pragma once

#include "particleSystem.h"

namespace Framework
{
	
class Shader;

class PEEnergyField : public ParticleSystem
{
	Shader* advance_particles;
	Shader* render_particles;
	ID3D10EffectVectorVariable* color_ptr;

	ID3D10EffectVectorVariable* half_scale_ptr;
	ID3D10EffectScalarVariable* global_time_ptr;
	ID3D10EffectScalarVariable* elapsed_time_ptr;
	ID3D10EffectScalarVariable* deactivating_ptr;
	ID3D10EffectShaderResourceVariable* random_texture_ptr;
	ID3D10EffectMatrixVariable* mat_world_ptr;

public:
	PEEnergyField( ID3D10Device* _device );
	virtual ~PEEnergyField();

	virtual void Initialize();
	virtual void Update( float timeslice );
	virtual void Draw( Mat4& mat_view, Mat4& mat_proj );
	virtual void Free();
	//virtual void Load() {}
	//virtual void Unload() {}

};


}