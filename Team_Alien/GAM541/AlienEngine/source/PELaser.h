#pragma once

#include "particleSystem.h"
#define MAX_LASERS		6

namespace Framework
{
	
class Shader;

struct Laser;

class PELaser : public ParticleSystem
{
private:
	struct Laser
	{
		ID3D10Buffer* vb_laser[2];
	};
private:
	Shader* advance_particles;
	Shader* render_particles;
	ID3D10EffectVectorVariable* color_ptr;
	
	ID3D10EffectScalarVariable* global_time_ptr;
	ID3D10EffectScalarVariable* elapsed_time_ptr;
	ID3D10EffectVectorVariable* start_point_ptr;
	ID3D10EffectVectorVariable* end_point_ptr;
	ID3D10EffectScalarVariable* state_ptr;
	ID3D10EffectScalarVariable* colliding_side_ptr;
	ID3D10EffectShaderResourceVariable* random_texture_ptr;

	Laser* lasers[2];
	int cur_target_buffer;

	LineListType* line_list;
	LineListType* line_list2;
	bool initializing;

	Vec4* color;
public:
	PELaser( ID3D10Device* _device );
	virtual ~PELaser();

	virtual void Initialize();
	virtual void Update( float timeslice );
	virtual void Draw( Mat4& mat_view, Mat4& mat_proj );
	virtual void Free();

	void SetLineList( LineListType* _line_list ) { line_list = _line_list; }
	void SetLineList2( LineListType* _line_list ) { line_list2 = _line_list; }
	void SetColor( Vec4* _color ){ color = _color; }
	//virtual void Load() {}
	//virtual void Unload() {}

};


}