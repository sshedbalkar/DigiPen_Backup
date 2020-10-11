#pragma once

#include "VMath.h"
#include "material.h"
#include <list>

namespace Framework
{
	
class Spikes : public Material
{
	//
	struct SpikesPieceData
	{
		Vec3 position;// x -> x position; y -> width; z-> height
	};
	struct ClusterData
	{
		int reflection_index;
		Transform* transform;
		SpikesPieceData* pieces;
		int count;
	};
	typedef std::list<ClusterData> ClusterListType;

private:
	ClusterListType cluster_list;
	ID3D10Buffer* vb;
	ID3D10EffectMatrixVariable* mat_world_ptr;

public:
	Spikes();
	~Spikes();
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
