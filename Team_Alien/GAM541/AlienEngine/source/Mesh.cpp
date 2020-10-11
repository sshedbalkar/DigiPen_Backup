#include "Mesh.h"
#include "animation.h"
#include "MeshInstance.h"
#include "Transform.h"
#include "shader.h"
#include "DebugDiagnostic.h"
#include "texture.h"
#include "XMLSerialization.h"
#include "colladaReader.h"
#include "Graphics.h"
#include <fstream>
#include <iostream>
#include <sstream>

namespace Framework
{
const int MAX_INSTANCES = 1000;

//================================================
Mesh::Mesh()
{
	animated = true;
	vertex_count = 0;
}
//================================================
Mesh::~Mesh()
{
	Free();
}
//================================================
void Mesh::Free()
{
	S_RELEASE(vertex_buffer[0]);
	S_RELEASE(vertex_buffer[1]);
	MeshPartMapType::iterator mit;
	for ( mit = mesh_parts.begin(); mit != mesh_parts.end(); ++mit )
	{
		S_DELETE(mit->second);
	}
	mesh_parts.clear();
}
//================================================
void Mesh::Draw( ID3D10Device* device, Shader* static_shader, Shader* animated_shader )
{
	if ( instances.size() < 1 ) return;
	if ( animated ) DrawAnimated( device, animated_shader );
	else DrawStatic( device, static_shader );
}
//================================================
void Mesh::Update( float timeslice )
{
	if ( !animated ) return;
	InstanceListType::iterator it;
	for ( it = instances.begin(); it != instances.end(); ++it )
	{
		if ( !it->is_paused ) it->dt += timeslice;
	}
}
//================================================
void Mesh::RegisterInstance( MeshInstance* instance )
{
	instances.push_back( instance );
}
//================================================
void Mesh::RemoveInstance( MeshInstance* instance )
{
	instances.erase( instance );
}
//================================================
int Mesh::GetVertexCount()
{
	return vertex_count;
}
//================================================
int Mesh::GetInstanceCount()
{
	return instances.size(); /////
}
//================================================
void Mesh::BuildVertexBuffer( ID3D10Device* device, const char* filename, bool _animated  )
{
	animated = _animated;

	if ( animated ) BuildVBAnimated( device, filename );
	else BuildVBStatic( device, filename );

}
//================================================
void Mesh::BuildVBAnimated( ID3D10Device* device, const char* filename )
{
	D3D10_BUFFER_DESC vb_desc;
	D3D10_SUBRESOURCE_DATA vb_data;
	HRESULT hr;
	MeshVertex* vertices = NULL;

	LoadMeshFromMD5( filename, &vertices );
	if ( !vertices ) return;

	/*mesh vertex buffer*/
	ZeroMemory( &vb_desc, sizeof( vb_desc ) );
	vb_desc.Usage = D3D10_USAGE_DEFAULT;
	vb_desc.ByteWidth = sizeof( MeshVertex ) * vertex_count;
	vb_desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vb_desc.CPUAccessFlags = 0;
	vb_desc.MiscFlags = 0;
	ZeroMemory( &vb_data, sizeof( vb_data ) );
	vb_data.pSysMem = vertices;
	hr = device->CreateBuffer( &vb_desc, &vb_data, &vertex_buffer[0] );
	ErrorIf( FAILED(hr), "failed building vertex buffer" );

	///*instance buffer*/
	ZeroMemory( &vb_desc, sizeof( vb_desc ) );
	vb_desc.Usage = D3D10_USAGE_DYNAMIC;
	vb_desc.ByteWidth = sizeof( MeshInstanceData );
	vb_desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vb_desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	vb_desc.MiscFlags = 0;
	hr = device->CreateBuffer( &vb_desc, 0, &vertex_buffer[1] );
	ErrorIf( FAILED(hr), "failed building instance buffer" );

	delete [] vertices;
}
//================================================
void Mesh::BuildVBStatic( ID3D10Device* device, const char* filename )
{
	D3D10_BUFFER_DESC vb_desc;
	D3D10_SUBRESOURCE_DATA vb_data;
	HRESULT hr;
	MeshVertexStatic* vertices = NULL;
	LoadMeshFromCollada( filename, &vertices );
	if ( !vertices ) return;
	//for ( int i = 0; i < vertex_count; ++i )
	//{
	//	std::cout <<vertices[i].position.x <<" "<<vertices[i].position.y<<" "<<vertices[i].position.z <<std::endl;
	//}

	/*mesh vertex buffer*/
	ZeroMemory( &vb_desc, sizeof( vb_desc ) );
	vb_desc.Usage = D3D10_USAGE_DEFAULT;
	vb_desc.ByteWidth = sizeof( MeshVertexStatic ) * vertex_count;
	vb_desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vb_desc.CPUAccessFlags = 0;
	vb_desc.MiscFlags = 0;
	ZeroMemory( &vb_data, sizeof( vb_data ) );
	vb_data.pSysMem = vertices;
	hr = device->CreateBuffer( &vb_desc, &vb_data, &vertex_buffer[0] );
	ErrorIf( FAILED(hr), "failed building vertex buffer" );

	///*instance buffer*/
	ZeroMemory( &vb_desc, sizeof( vb_desc ) );
	vb_desc.Usage = D3D10_USAGE_DYNAMIC;
	vb_desc.ByteWidth = sizeof( MeshInstanceData ) * MAX_INSTANCES;
	vb_desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vb_desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	vb_desc.MiscFlags = 0;
	hr = device->CreateBuffer( &vb_desc, 0, &vertex_buffer[1] );
	ErrorIf( FAILED(hr), "failed building instance buffer" );

	delete [] vertices;
}
//================================================
void Mesh::DrawAnimated( ID3D10Device* device, Shader* shader )
{
	if ( instances.size() < 1 ) return;
	MeshInstanceData* instance_data;
	ID3D10EffectVectorVariable* orient_bones_ptr = shader->GetVariableByName("orient_bones")->AsVector();
	ID3D10EffectVectorVariable* pos_bones_ptr = shader->GetVariableByName("pos_bones")->AsVector();

	InstanceListType::iterator it;
	for ( it = instances.begin(); it != instances.end(); ++it )
	{
		Transform* transform = it->transform;
		if ( !GRAPHICS->FrustumTest( transform ) )  continue;
		vertex_buffer[1]->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void**)&instance_data );
		transform->BuildMatrix( instance_data[0].mat_world );
		instance_data[0].flag = it->flag;
		//D3DXMatrixTranslation( &instance_data[0].mat_world, pos->x, pos->y, pos->z );
		vertex_buffer[1]->Unmap();
		unsigned int stride[2] = { sizeof( MeshVertex ), sizeof( MeshInstanceData ) };
		unsigned int offset[2] = { 0, 0 };
		device->IASetVertexBuffers( 0, 2, vertex_buffer, stride, offset );

		Texture* texture = it->texture;
		if ( texture )
		{
			texture_ptr = shader->GetVariableByName("diffuse")->AsShaderResource();
			texture_ptr->SetResource( texture->GetDiffuse() );
		}

		int n_bones = 0;
		Vec4* bone_orients;
		Vec3* bone_positions;
		it->animation->GetJointAnimation( true, it->dt, n_bones, &bone_orients, &bone_positions );
		orient_bones_ptr->SetFloatVectorArray( (float*)bone_orients, 0, n_bones );
		pos_bones_ptr->SetFloatVectorArray( (float*)bone_positions, 0, n_bones );

		shader->Apply( device );
		device->DrawInstanced( vertex_count, 1, 0, 0 );
	}


}
//================================================
void Mesh::DrawStatic( ID3D10Device* device, Shader* shader )
{
	if ( instances.size() < 1 ) return;
	MeshInstanceData* instance_data;
	vertex_buffer[1]->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void**)&instance_data );


	InstanceListType::iterator it = instances.begin();
	unsigned int i = 0;
	for ( ; it != instances.end(); ++it )
	{
		Transform* transform = it->transform;
		if ( !GRAPHICS->FrustumTest( transform ) )  continue;
		transform->BuildMatrix( instance_data[i].mat_world );
		instance_data[i].flag = it->flag;
		++i;
	}
	vertex_buffer[1]->Unmap();
	unsigned int stride[2] = { sizeof( MeshVertexStatic ), sizeof( MeshInstanceData ) };
	unsigned int offset[2] = { 0, 0 };
	device->IASetVertexBuffers( 0, 2, vertex_buffer, stride, offset );
	
	shader->Apply( device );
	device->DrawInstanced( vertex_count, i, 0, 0 );
}
//============================================================
void Mesh::LoadMeshFromCollada( const char* filename, MeshVertexStatic** vertices )
{
	XMLReader reader;
	int polycount;
	int* vcount;
	int pcount = 0;
	int* p;
	DAESourceFloat* sources;
	Vec4* diffuses;
	
	std::string str;
	//reader = new XMLReader();
	reader.open( filename );
	//ErrorIf( !reader->open( filename ), "file %s not found.\n", filename );
	ISerializer* stream = reader.data().clone();
	/** load materials (only color here) **/
	ISerializer* effects = stream->clone();
	effects->readProperty( "library_effects", *effects );
	std::vector<ISerializer*> materials;
	effects->readProperty( "effect", materials );
	diffuses = new Vec4[materials.size()];
	for ( unsigned int i = 0; i < materials.size(); ++i )
	{
		ISerializer* diffuse = materials[i]->clone();
		materials[i]->readProperty("profile_COMMON", *diffuse);
		diffuse->readProperty("technique", *diffuse );
		diffuse->readProperty( "phong", *diffuse );
		diffuse->readProperty( "diffuse", *diffuse );
		float* color;
		ReadFloatArray( *diffuse, "color", 4, &color );
		diffuses[i] = Vec4( color[0], color[1], color[2], color[3] );
//		std::cout <<color[0]<<" "<<color[1]<<" "<<color[2]<<" "<<color[3]<<std::endl;
		delete [] color;
		delete diffuse;
		delete materials[i];
	}
	delete effects;
	/** start loading mesh **/
	ISerializer* geometries = stream->clone();
	stream->readProperty( "library_geometries", *geometries );
	geometries->readProperty( "geometry", *geometries );
	geometries->readProperty( "id", str );
	geometries->readProperty( "mesh", *geometries );

	std::vector<ISerializer*> source_streams;
	geometries->readProperty( "source", source_streams );
	// loading sources from mesh( must be float array)
	sources = new DAESourceFloat[ source_streams.size() ];
	for ( unsigned int i = 0; i < source_streams.size(); ++i )
	{
		sources[i].Serialize( *source_streams[i] );
		delete source_streams[i];
	}
	///** loading polylist
	std::vector<ISerializer*> polylists;
	geometries->readProperty( "polylist", polylists );
	vertex_count = 0;
	for ( unsigned int c = 0; c < polylists.size(); ++c )		//count vertices
	{
		//vcount
		polylists[c]->readProperty( "count", polycount );
		ReadIntArray( *(polylists[c]), "vcount", polycount, &vcount );
		for ( int i = 0; i < polycount; ++i )
		{
			int n_faces = vcount[i] - 2;
			vertex_count += n_faces;
		}
		delete [] vcount;
//		delete polylists[c];
	}

	vertex_count *= 3;
	*vertices = new MeshVertexStatic[ vertex_count ];
	int cur_vertex = 0;
	int nsources = source_streams.size();
	/// write into vertex format
	for ( unsigned int c = 0; c < polylists.size(); ++c )		//for each material
	{
		//vcount
		polylists[c]->readProperty( "count", polycount );
		ReadIntArray( *(polylists[c]), "vcount", polycount, &vcount );
		//p
		for ( int i = 0; i < polycount; ++i ) pcount += vcount[i];
		pcount *= nsources;
		ReadIntArray( *(polylists[c]), "p", pcount, &p );
		int index = 0;
		for ( int i = 0; i < polycount; ++i )
		{
			for ( int j = 0; j < vcount[i] - 2; ++j )
			{
				float* vposition;
				float* vnormal;
				vposition = sources[0].GetUnit(p[index]);
				vnormal = sources[1].GetUnit(p[index+1]);
				(*vertices)[cur_vertex].position = Vec3( vposition[0], vposition[2], vposition[1] );
				(*vertices)[cur_vertex].normal = Vec3( vnormal[0], vnormal[2], vnormal[1] );
				(*vertices)[cur_vertex].color = diffuses[c];
				++cur_vertex;
				vposition = sources[0].GetUnit(p[index+nsources*(j+1)] );
				vnormal = sources[1].GetUnit(p[index+nsources*(j+1)+1]);
				(*vertices)[cur_vertex].position = Vec3( vposition[0], vposition[2], vposition[1] );
				(*vertices)[cur_vertex].normal = Vec3( vnormal[0], vnormal[2], vnormal[1] );
				(*vertices)[cur_vertex].color = diffuses[c];
				++cur_vertex;
				vposition = sources[0].GetUnit(p[index+nsources*(j+2)] );
				vnormal = sources[1].GetUnit(p[index+nsources*(j+2)+1]);
				(*vertices)[cur_vertex].position = Vec3( vposition[0], vposition[2], vposition[1] );
				(*vertices)[cur_vertex].normal = Vec3( vnormal[0], vnormal[2], vnormal[1] );
				(*vertices)[cur_vertex].color = diffuses[c];
				++cur_vertex;
			}
			index += nsources * vcount[i];
		}
		delete [] p;
		delete [] vcount;
		delete polylists[c];
	}
	delete [] diffuses;
	delete [] sources;
	delete geometries;
	delete stream;
//	delete reader;
}
//================================================
void Mesh::LoadMeshFromMD5( const char* filename, MeshVertex** vertices )
{
	MD5Mesh* meshes;
	std::ifstream fin(filename);
	ErrorIf( !fin, "cannot open mesh file %s.\n", filename );
	char line[256];
	std::stringstream ss;
	std::string str;
	int num_meshes;
	int temp;
	char c;

	//  now parsing :
		//	numJoints xx
		//	numMeshes xx
	fin.getline( line, 255 );
	fin.getline( line, 255 );
	fin.getline( line, 255 );
	fin.getline( line, 255 );
	ss.clear();
	ss.str(line);
	ss >>str >>num_joints;
	fin.getline( line, 255 );
	ss.clear(); ss.str(line);
	ss >>str >>num_meshes;
	// now parsing:
	//joints {
	//   "name" parent ( pos.x pos.y pos.z ) ( orient.x orient.y orient.z )
	//	...
	//	}
	fin.getline( line, 255 );
	Joint* joints = new Joint [num_joints];
	fin.getline( line, 255 );
	for ( int i = 0; i < num_joints; ++i )
	{
		int parent;
		fin.getline( line, 255 );
		ss.clear(); ss.str( line );
		ss >>joints[i].name;
		ss >>parent;
		//AddOffspringToJoint( parent, i );
		ss >>c >>joints[i].joint_pos.x >>joints[i].joint_pos.y >>joints[i].joint_pos.z >>c;
		ss >>c >>joints[i].joint_orient.x >>joints[i].joint_orient.y >>joints[i].joint_orient.z >>c;
		Vec4* q = &(joints[i].joint_orient);
		float t = 1.0f - (q->x * q->x) - (q->y * q->y) - (q->z * q->z);
		if (t < 0.0f) q->w = 0.0f;
		else q->w = -sqrt (t);
	}
	fin.getline( line, 255 );
	fin.getline( line, 255 );

	vertex_count = 0;

	meshes = new MD5Mesh[num_meshes];
	bool is_face = false;
	for ( int i = 0; i < num_meshes; ++i )
	{
		// now parsing:
		//mesh {
		//	shader " <id> <color> "
		fin.getline( line, 255 );
		fin.getline( line, 255 );
		ss.clear(); ss.str(line);
		ss >>str >>c;
		MeshPartData* part = new MeshPartData;
		std::string id;
		ss >>id >>part->color.x >>part->color.y >>part->color.z >>part->color.w;
		mesh_parts[id] = part;
		meshes[i].part = part;
		id.pop_back();
		if ( id == "face" ) is_face = true;
		else is_face = false;
		fin.getline( line, 255 );
		// now parsing
		//	numverts <int>
		//	vert vertIndex ( s t ) startWeight countWeight
		//	vert ...
		fin.getline( line, 255 );
		ss.clear(); ss.str(line);
		ss >>str >>meshes[i].num_verts;
//		vertex_count += meshes[i].num_verts;
		meshes[i].verts = new MD5Vert[meshes[i].num_verts];
		MD5Vert* verts = meshes[i].verts;
		for ( int v = 0; v < meshes[i].num_verts; ++v )
		{
			fin.getline( line, 255 );
			ss.clear(); ss.str(line);
			ss >>str >>temp >>c >>verts[v].texcoord.x >>verts[v].texcoord.y >>c >>verts[v].start_weight >>verts[v].count_weight;
			if ( !is_face ) verts[v].texcoord.x = verts[v].texcoord.y = 0.0f;
		}
		fin.getline(line, 255);
		// now parsing
		//	numtris <int>
		//	tri triIndex vertIndex[0] vertIndex[1] vertIndex[2]
		//	tri ...
		fin.getline( line, 255 );
		ss.clear(); ss.str(line);
		ss >>str >>meshes[i].num_tris;		
		vertex_count += 3 * meshes[i].num_tris;
		meshes[i].tris = new MD5Tri[meshes[i].num_tris];
		MD5Tri* tris = meshes[i].tris;
		for ( int t = 0; t < meshes[i].num_tris; ++t )
		{
			fin.getline( line, 255 );
			ss.clear(); ss.str(line);
			ss >>str >>temp >>tris[t].v0 >>tris[t].v1 >>tris[t].v2;
//			std::cout <<t <<" " <<tris[t].v0 <<" "<<tris[t].v1 <<" "<<tris[t].v2 <<std::endl;
		}
		fin.getline( line, 255 );
		// now parsing
		//	numweights <int>
		//	weight weightIndex joint bias ( pos.x pos.y pos.z )
		//	weight ...
		//}
		fin.getline( line, 255 );
		ss.clear(); ss.str(line);
		ss >>str >>meshes[i].num_weights;
		meshes[i].weights = new MD5Weight[meshes[i].num_weights];
		MD5Weight* weights = meshes[i].weights;
		for ( int w = 0; w < meshes[i].num_weights; ++w )
		{
			fin.getline( line, 255 );
			ss.clear(); ss.str(line);
			ss >>str >>temp >>weights[w].joint >>weights[w].bias >>c >>weights[w].pos.x >>weights[w].pos.y >>weights[w].pos.z;
//			std::cout <<w <<" " <<weights[w].joint<<c<<weights[w].bias <<c <<weights[w].pos.x <<c<<weights[w].pos.y <<c<<weights[w].pos.z<<std::endl;
		}
		fin.getline( line, 255 );
		fin.getline( line, 255 );
	}
	fin.close();

	// finished reading file, generate vertex position, normal
	*vertices = new MeshVertex[ vertex_count ];
	int cur_vertex = 0;
	for ( int m = 0; m < num_meshes; ++m )
	{
		Vec3 average(0.0f,0.0f,0.0f);
		for ( int v = 0; v < meshes[m].num_verts; ++v )
		{
			MD5Vert* vert = &(meshes[m].verts[v]);
			int* vbones = vert->bones;
			float* vweights = vert->weights;
			Vec3 bind_pose( 0.0f, 0.0f, 0.0f );
			int b_index = 0;
			for ( int w = vert->start_weight; w < vert->start_weight + vert->count_weight; ++w )
			{
				Vec3 pos;
				pos = meshes[m].weights[w].pos;
				QuatRotatePoint( joints[meshes[m].weights[w].joint].joint_orient, pos, pos );
				pos += joints[meshes[m].weights[w].joint].joint_pos;
				pos *= meshes[m].weights[w].bias;
				bind_pose += pos;
				if ( animated )   //animated mesh, only store the weight information
				{
					if ( meshes[m].weights[w].bias > 0.9f )
					{
						//std::cout <<"x";
						vert->position[0] = meshes[m].weights[w].pos;
						vbones[0] = meshes[m].weights[w].joint;
						vweights[0] = 1.0f;
						//++b_index;
					}
				}
			}
			if ( !animated ) vert->position[0] = bind_pose;
			bind_pose /= (float)meshes[m].num_verts;
			average += bind_pose;
		}
		meshes[m].part->center = average;
//		std::cout <<average.x <<" "<<average.y <<" "<<average.z <<std::endl;
		for ( int t = 0; t < meshes[m].num_tris; ++t )
		{
			MD5Tri* tri = &(meshes[m].tris[t]);
			for ( int b = 0; b < 4; ++b ) 
			{
				(*vertices)[cur_vertex].position[b] = meshes[m].verts[tri->v0].position[b];
				(*vertices)[cur_vertex].bones[b] = meshes[m].verts[tri->v0].bones[b];
				(*vertices)[cur_vertex].weights[b] = meshes[m].verts[tri->v0].weights[b];
			} 
			(*vertices)[cur_vertex].texcoord = meshes[m].verts[tri->v0].texcoord;
			(*vertices)[cur_vertex++].color = meshes[m].part->color;

			for ( int b = 0; b < 4; ++b ) 
			{
				(*vertices)[cur_vertex].position[b] = meshes[m].verts[tri->v1].position[b];
				(*vertices)[cur_vertex].bones[b] = meshes[m].verts[tri->v1].bones[b];
				(*vertices)[cur_vertex].weights[b] = meshes[m].verts[tri->v1].weights[b];
			} 
			(*vertices)[cur_vertex].texcoord = meshes[m].verts[tri->v1].texcoord;
			(*vertices)[cur_vertex++].color = meshes[m].part->color;

			for ( int b = 0; b < 4; ++b ) 
			{
				(*vertices)[cur_vertex].position[b] = meshes[m].verts[tri->v2].position[b];
				(*vertices)[cur_vertex].bones[b] = meshes[m].verts[tri->v2].bones[b];
				(*vertices)[cur_vertex].weights[b] = meshes[m].verts[tri->v2].weights[b];
			} 
			(*vertices)[cur_vertex].texcoord = meshes[m].verts[tri->v2].texcoord;
			(*vertices)[cur_vertex++].color = meshes[m].part->color;
		}
	}

	delete [] meshes;
	delete [] joints;
}


}//end namespace
