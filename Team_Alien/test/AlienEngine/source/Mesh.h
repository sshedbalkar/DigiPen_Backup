#pragma once

#include "Component.h"
#include "VMath.h"
#include "DirectXIncludes.h"
#include <string>
#include <list>

namespace Framework
{

class MeshInstance;
class Transform;/////

class Mesh : public GameComponent
{
	struct SkeletalMeshVertex
	{
		Vec3 position;
		Vec3 normal;
		Vec4 color;
		int bone_index;
	};

	struct StaticMeshVertex
	{
		Vec3 position;
		Vec3 normal;
		Vec4 color;
	};

	struct StaticMeshInstanceData
	{
		Mat4 mat_world;
	};

	typedef std::list<Transform*> InstanceList; ////for testing

private:
	bool animated;
	std::string filename;
	ID3D10Buffer* vertex_buffer[2];
	InstanceList transforms;/////for testing
	int vertex_count;

public:
	Mesh();
	~Mesh();

	///Component Serialization Interface see Serialization.h for details.
	virtual void Serialize(ISerializer& str);
	virtual void Initialize();
	virtual void Load(){};
	virtual void Unload(){};/// for restarting level
	virtual void Free(){};/// un-initialize

	void AddInstance( MeshInstance* instance );
	void AddInstance( Transform* transform );//// for testing purpose
	void BuildVertexBuffer( ID3D10Device* device );
	void SetPerMeshConstants( ID3D10Device* device );
	bool SetPerInstanceConstants( ID3D10Device* device );
	bool SetVertexBuffer( ID3D10Device* device );

	int GetVertexCount();
	int GetInstanceCount();
private:
	void BuildVertexBufferSkeletal( ID3D10Device* device );
	void BuildVertexBufferStatic( ID3D10Device* device );
	void BuildVertexBufferTriangle( ID3D10Device* device );

	bool SetVertexBufferSkeletal( ID3D10Device* device );
	bool SetVertexBufferStatic( ID3D10Device* device );
};

}