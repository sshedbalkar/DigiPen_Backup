#pragma once

#include "VMath.h"
#include "DirectXIncludes.h"
#include <string>
#include <list>
#include <map>
#include "ObjectLinkedList.h"

namespace Framework
{

class MeshInstance;
class Shader;

class Mesh
{
	struct MeshVertex
	{
		Vec3 position[4];
		Vec4 color;
		Vec2 texcoord;
		int bones[4];
		float weights[4];
//		int flag;
	};
	struct MeshVertexStatic
	{
		Vec3 position;
		Vec3 normal;
		Vec4 color;
	};
	// MD5 data structures
	struct MeshPartData
	{
		Vec3 center;
		Vec4 color;
	};
	struct MD5Vert
	{
		MD5Vert() { bones[0]=bones[1]=bones[2]=bones[3]=-1; }
		Vec2 texcoord;
		int start_weight;
		int count_weight;
		Vec3 position[4];
		int bones[4];
		float weights[4];
	};
	struct MD5Tri
	{
		int v0, v1, v2;
	};
	struct MD5Weight
	{
		int joint;
		float bias;
		Vec3 pos;
	};
	struct MD5Mesh
	{
		MD5Mesh():verts(NULL),tris(NULL),weights(NULL) {}
		~MD5Mesh() { delete [] verts; delete [] tris; delete [] weights; }
		std::string material;
		int num_verts,num_tris, num_weights;
		MD5Vert* verts;
		MD5Tri* tris;
		MD5Weight* weights;
		MeshPartData* part;
	};
	struct Joint
	{
		Joint() : joint_pos( Vec3(0.0,0.0,0.0 ) ), joint_orient( Vec3(0.0,0.0,0.0) ), offspring(-1),sibling(-1) {}
		std::string name;
		Vec3 joint_pos;
		Vec4 joint_orient;
		int sibling;
		int offspring;
	};
	struct MeshInstanceData
	{
		Mat4 mat_world;
		unsigned int flag;
	};
	typedef ObjectLinkList<MeshInstance> InstanceListType;
	typedef std::map<std::string, MeshPartData*> MeshPartMapType;

private:
	bool animated;
	ID3D10Buffer* vertex_buffer[2];
	InstanceListType instances;
	int vertex_count;

	int num_joints;
	MeshPartMapType mesh_parts;
	ID3D10EffectShaderResourceVariable* texture_ptr;

public:
	Mesh();
	~Mesh();

	void Free();
	void Unload();

	void RegisterInstance( MeshInstance* instance );
	void RemoveInstance( MeshInstance* instance );
	void Draw( ID3D10Device* device, Shader* static_shader, Shader* animated_shader );
	void Update( float timeslice );
	void BuildVertexBuffer( ID3D10Device* device, const char* filename, bool animated  );

	int GetVertexCount();
	int GetInstanceCount();
private:
	void BuildVBAnimated( ID3D10Device* device, const char* filename );
	void BuildVBStatic( ID3D10Device* device, const char* filename );

	void DrawAnimated( ID3D10Device* device, Shader* shader );
	void DrawStatic( ID3D10Device* device, Shader* shader );

	void LoadMeshFromMD5( const char* filename, MeshVertex** vertices );
	void LoadMeshFromCollada( const char* filename, MeshVertexStatic** vertices );
	void AddOffspringToJoint( int parent, int offspring );

};

}
