#include "Mesh.h"
#include "Transform.h"
#include "binaryIO.h"

namespace Framework
{
const int max_instances = 1000;

//================================================
Mesh::Mesh()
{
	vertex_count = 0;
	filename = "Assets\\cube.mesh";
}
//================================================
Mesh::~Mesh()
{
}
//================================================
void Mesh::Serialize( ISerializer& )
{
}
//================================================
void Mesh::Initialize()
{
}
//================================================
void Mesh::BuildVertexBuffer( ID3D10Device* device )
{
	animated = false;////

	if ( animated ) BuildVertexBufferSkeletal( device );
	else BuildVertexBufferStatic( device );
}
//================================================
bool Mesh::SetVertexBuffer( ID3D10Device* device )
{
	if ( animated ) return SetVertexBufferSkeletal( device );
	else return SetVertexBufferStatic( device );
}
//================================================
void Mesh::SetPerMeshConstants( ID3D10Device* device )
{
}
//================================================
bool Mesh::SetPerInstanceConstants( ID3D10Device* device )
{
	return true;
}
//================================================
void Mesh::AddInstance( Transform* transform )
{
	transforms.push_back( transform );
}
//================================================
int Mesh::GetVertexCount()
{
	return vertex_count;
}
//================================================
int Mesh::GetInstanceCount()
{
	return transforms.size(); /////
}
//================================================
void Mesh::BuildVertexBufferSkeletal( ID3D10Device* device )
{
}
//================================================
void Mesh::BuildVertexBufferStatic( ID3D10Device* device )
{
	D3D10_BUFFER_DESC vb_desc;
	D3D10_SUBRESOURCE_DATA vb_data;
	HRESULT hr;
	SkeletalMeshVertex* imported_vertices;
	StaticMeshVertex* vertices;
	BinaryReader bin;
	
	bin.Open( filename.c_str() );
	bin( &imported_vertices, &vertex_count );
	bin.Close();
	vertices = new StaticMeshVertex[vertex_count];
	for ( int i = 0; i < vertex_count; ++i )
	{
		vertices[i].position = imported_vertices[i].position;
		vertices[i].normal = imported_vertices[i].normal;
		vertices[i].color = imported_vertices[i].color;
	}
	
	/*mesh vertex buffer*/
	ZeroMemory( &vb_desc, sizeof( vb_desc ) );
	vb_desc.Usage = D3D10_USAGE_DEFAULT;
	vb_desc.ByteWidth = sizeof( StaticMeshVertex ) * vertex_count;
	vb_desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vb_desc.CPUAccessFlags = 0;
	vb_desc.MiscFlags = 0;
	ZeroMemory( &vb_data, sizeof( vb_data ) );
	vb_data.pSysMem = vertices;
	hr = device->CreateBuffer( &vb_desc, &vb_data, &vertex_buffer[0] );
	if ( FAILED(hr) ) exit(0);/////ErrorIf

	/*instance buffer*/
	ZeroMemory( &vb_desc, sizeof( vb_desc ) );
	vb_desc.Usage = D3D10_USAGE_DYNAMIC;
	vb_desc.ByteWidth = sizeof( StaticMeshInstanceData ) * max_instances;
	vb_desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vb_desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	vb_desc.MiscFlags = 0;
	hr = device->CreateBuffer( &vb_desc, 0, &vertex_buffer[1] );
	if ( FAILED(hr) ) exit(0);/////ErrorIf
	
	delete [] imported_vertices;
	delete [] vertices;
}
//================================================
void Mesh::BuildVertexBufferTriangle( ID3D10Device* device )
{
	D3D10_BUFFER_DESC vb_desc;
	D3D10_SUBRESOURCE_DATA vb_data;
	HRESULT hr;
	StaticMeshVertex* vertices;
	//////for testing purpose, a triangle is built
	vertex_count = 3;
	vertices = new StaticMeshVertex[vertex_count];
	vertices[0].position = Vec3( -0.5f, -0.5f, 0.0f );
	vertices[1].position = Vec3(  0.5f, -0.5f, 0.0f );
	vertices[2].position = Vec3(  0.0f,  0.5f, 0.0f );

	/*mesh vertex buffer*/
	ZeroMemory( &vb_desc, sizeof( vb_desc ) );
	vb_desc.Usage = D3D10_USAGE_DEFAULT;
	vb_desc.ByteWidth = sizeof( StaticMeshVertex ) * 3;
	vb_desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vb_desc.CPUAccessFlags = 0;
	vb_desc.MiscFlags = 0;
	ZeroMemory( &vb_data, sizeof( vb_data ) );
	vb_data.pSysMem = vertices;
	hr = device->CreateBuffer( &vb_desc, &vb_data, &vertex_buffer[0] );
	if ( FAILED(hr) ) exit(0);/////ErrorIf

	/*instance buffer*/
	ZeroMemory( &vb_desc, sizeof( vb_desc ) );
	vb_desc.Usage = D3D10_USAGE_DYNAMIC;
	vb_desc.ByteWidth = sizeof( StaticMeshInstanceData ) * max_instances ;
	vb_desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vb_desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	vb_desc.MiscFlags = 0;
	hr = device->CreateBuffer( &vb_desc, 0, &vertex_buffer[1] );
	if ( FAILED(hr) ) exit(0);/////ErrorIf
	
}
//================================================
bool Mesh::SetVertexBufferSkeletal( ID3D10Device* device )
{
	return true;
}
//================================================
bool Mesh::SetVertexBufferStatic( ID3D10Device* device )
{
	if ( GetInstanceCount() < 1 ) return false;
	StaticMeshInstanceData* instance_data;
	vertex_buffer[1]->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void**)&instance_data );

	InstanceList::iterator it;
	unsigned int i = 0;
	for ( it = transforms.begin(); it != transforms.end(); ++it )
	{
		D3DXMatrixIdentity( &instance_data[i].mat_world );
		D3DXMatrixTranslation( &instance_data[i].mat_world, (*it)->Position.x, (*it)->Position.y, (*it)->Position.z );
		++i;
	}

	unsigned int stride[2] = { sizeof( StaticMeshVertex ), sizeof( StaticMeshInstanceData ) };
	unsigned int offset[2] = { 0, 0 };
	device->IASetVertexBuffers( 0, 2, vertex_buffer, stride, offset );
	return true;
}

}//end namespace
