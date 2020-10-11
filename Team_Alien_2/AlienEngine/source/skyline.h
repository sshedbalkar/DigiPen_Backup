#pragma once

#include "VMath.h"
#include "material.h"
#include <list>

namespace Framework
{
	
class Skyline : public Material
{
	//
	struct UnitVertex
	{
		Vec3 position; // x, length, height
		Vec3 tip; 
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
	ID3D10EffectShaderResourceVariable* normal_map_ptr;
	ID3D10EffectShaderResourceVariable* diffuse_ptr;
	ID3D10EffectMatrixVariable* mat_world_ptr;

public:
	Skyline();
	~Skyline();
	virtual void Initialize( ID3D10Device* device );
	virtual void Update( float timeslice );
	virtual void Draw( ID3D10Device* device, Mat4& mat_view, Mat4& mat_proj, float* reflect_altitude = NULL );
	virtual void RegisterVolume( Transform* transform, int flag );
	virtual void Free();
	virtual void Unload();
private:
	void PopulateSurface( ClusterData& cluster, float density, int flag );
};

}
