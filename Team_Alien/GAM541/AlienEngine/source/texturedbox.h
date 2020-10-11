#pragma once

#include "VMath.h"
#include "material.h"
#include "ObjectLinkedList.h"
#include <list>

namespace Framework
{

class Texture;
class VisualPlatform;
	
class TexturedBox : public Material
{
	//
	struct BoxData
	{
		Vec3 position;
		Vec3 size;
		Vec3 extra;
		unsigned int flag;
	};
	struct ClusterData
	{
		std::vector<Transform*> transforms;
		std::vector<Vec3> extra;
		std::vector<unsigned int> flags;
		Texture** texture;
	};
	typedef std::list<ClusterData> ClusterListType;

private:
	ClusterListType cluster_list;
	ID3D10Buffer* vb;
	
	ID3D10EffectShaderResourceVariable* diffuse_map_ptr;
	ID3D10EffectShaderResourceVariable* normal_map_ptr;
	ID3D10EffectShaderResourceVariable* mask_ptr;

	Texture** texture;
	int shininess;
	float bloom_intensity;
	unsigned int filter_flag;
	float tex_method;
public:
	TexturedBox();
	~TexturedBox();
	virtual void Initialize( ID3D10Device* device );
	virtual void Update( float timeslice );
	virtual void Draw( ID3D10Device* device, Mat4& mat_view, Mat4& mat_proj, float* reflect_altitude = NULL );
	virtual void RegisterVolume( Transform* transform, int flag );
	virtual void Free();
	virtual void Unload();

	void SetTexture( Texture** _texture, int _shininess, float _bloom_intensity, float _tex_method, unsigned int flag ) 
	{ texture = _texture; shininess = _shininess; bloom_intensity = _bloom_intensity; tex_method = _tex_method; filter_flag = flag;};
private:
	void PopulateBox( ClusterData& cluster, int flag );
};

}