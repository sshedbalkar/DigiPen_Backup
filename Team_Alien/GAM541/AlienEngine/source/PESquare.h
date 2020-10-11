#pragma once

#include "particleSystem.h"

namespace Framework
{
	
class Shader;

class PESquare : public ParticleSystem
{
	Shader* advance_particles;
	Shader* render_particles;
	ID3D10EffectScalarVariable* flag_ptr;
	ID3D10EffectVectorVariable* color_ptr;

	ID3D10EffectVectorVariable* initial_position_bottomleft_ptr;
	ID3D10EffectVectorVariable* initial_position_topright_ptr;
	ID3D10EffectScalarVariable* particle_radius_ptr;
	ID3D10EffectVectorVariable* initial_velocity_ptr;
	ID3D10EffectVectorVariable* velocity_deviation_ptr;
	ID3D10EffectScalarVariable* life_span_ptr;
	ID3D10EffectScalarVariable* life_span_deviation_ptr;
	ID3D10EffectScalarVariable* global_time_ptr;
	ID3D10EffectScalarVariable* elapsed_time_ptr;
	ID3D10EffectVectorVariable* acceleration_ptr;
	ID3D10EffectScalarVariable* deactivating_ptr;
	ID3D10EffectShaderResourceVariable* random_texture_ptr;
	ID3D10EffectMatrixVariable* mat_world_ptr;

public:
	PESquare( ID3D10Device* _device );
	virtual ~PESquare();

	virtual void Initialize();
	virtual void Update( float timeslice );
	virtual void Draw( Mat4& mat_view, Mat4& mat_proj );
	virtual void Free();
	//virtual void Load() {}
	//virtual void Unload() {}

	//virtual void RegisterEmitter( ParticleEmitter* emitter );
	//virtual void RemoveEmitter( ParticleEmitter* emitter );

};


}