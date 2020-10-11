#pragma once

#include "VMath.h"
#include "material.h"
#include <list>

namespace Framework
{
	
class Water : public Material
{
	//
	struct UnitVertex
	{
		Vec3 position; // x, length, height
	};
	struct ClusterData
	{
		Transform* transform;
		UnitVertex* pieces;
		int count;
	};
	typedef std::list<ClusterData> ClusterListType;

private:
	ClusterListType cluster_list;
	ID3D10Buffer* vb;
	ID3D10EffectShaderResourceVariable* diffuse_map_ptr;
	ID3D10EffectScalarVariable* t_ptr;
	ID3D10EffectScalarVariable* ripple_x_ptr;
	ID3D10EffectScalarVariable* ripple_height_ptr;
	ID3D10EffectScalarVariable* ripple_initial_t_ptr;

public:
	float t;
	float ripple_initial_t;
	float ripple_x;
	float ripple_height;

public:
	Water();
	~Water();
	virtual void Initialize( ID3D10Device* device );
	virtual void Update( float timeslice );
	virtual void Draw( ID3D10Device* device, Mat4& mat_view, Mat4& mat_proj, float* reflect_altitude = NULL );
	virtual void RegisterVolume( Transform* transform, int flag );
	virtual void Free();
	virtual void Unload();
private:
	void PopulateSurface( ClusterData& cluster, float density, int layer );
};

}
