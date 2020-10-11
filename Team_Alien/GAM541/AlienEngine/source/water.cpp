#include "Water.h"
#include "shader.h"
#include "Transform.h"
#include "Graphics.h"
#include "texture.h"
#include <iostream>

const int MAX_WAVE_UNITS		= 1000;
#define PIECE_WIDTH		0.1f
#define SPEED			1.0f

namespace Framework
{

//====================================================
Water::Water()
{
}
//====================================================
Water::~Water()
{
	Free();
}
//====================================================
void Water::Initialize( ID3D10Device* device )
{
	D3D10_BUFFER_DESC vb_desc;
	ZeroMemory( &vb_desc, sizeof( vb_desc ) );
	vb_desc.Usage = D3D10_USAGE_DYNAMIC;
	vb_desc.ByteWidth = sizeof( UnitVertex ) * MAX_WAVE_UNITS;
	vb_desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vb_desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	vb_desc.MiscFlags = 0;

	device->CreateBuffer( &vb_desc, NULL, &vb );

	diffuse_map_ptr = shader->GetVariableByName("diffuse_map")->AsShaderResource();
	t_ptr = shader->GetVariableByName("t")->AsScalar();
	ripple_x_ptr = shader->GetVariableByName("ripple_x")->AsScalar();
	ripple_height_ptr = shader->GetVariableByName("ripple_height")->AsScalar();
	ripple_initial_t_ptr = shader->GetVariableByName("ripple_initial_t")->AsScalar();

	Texture* texture = GRAPHICS->GetTexture("WaterTexture");
	diffuse_map_ptr->SetResource( texture->GetDiffuse() );
	t = 0.0f;
	ripple_height = 0.0f;
	ripple_x = 0.0f;
	ripple_initial_t = 0.0f;
}
//====================================================
void Water::Update( float timeslice )
{
	t += timeslice;
}
//====================================================
void Water::Draw( ID3D10Device* device, Mat4& mat_view, Mat4& mat_proj, float* reflect_altitude )
{
	unsigned int stride = sizeof( UnitVertex ), offset = 0;
	UnitVertex* mapped;

	shader->SetTransformationConstants( device, mat_view, mat_proj );
	device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	vb->Map( D3D10_MAP_WRITE_DISCARD, 0, (void**)&mapped );
	int count = 0;
	ClusterListType::iterator cit;
	for ( cit = cluster_list.begin(); cit != cluster_list.end(); ++cit )
	{
		if ( count + cit->count > MAX_WAVE_UNITS ) { printf("water waves overflowed\n"); exit(0); }
		memcpy( mapped + count, cit->pieces, cit->count * sizeof( UnitVertex ) );
		count += cit->count;
	}
	vb->Unmap();
	t_ptr->SetFloat( t );
	ripple_height_ptr->SetFloat( ripple_height );
	ripple_x_ptr->SetFloat( ripple_x );
	ripple_initial_t_ptr->SetFloat( ripple_initial_t );
	device->IASetVertexBuffers(0,1,&vb,&stride,&offset );
	shader->Apply( device );
	device->Draw( count, 0 );
}
//====================================================
void Water::Unload()
{
	ClusterListType::iterator cit;
	for ( cit = cluster_list.begin(); cit != cluster_list.end(); ++cit )
	{
		delete [] cit->pieces;
	}
	cluster_list.clear();
}
//====================================================
void Water::Free()
{
	Unload();
	S_RELEASE(vb);
}
//====================================================
void Water::RegisterVolume( Transform* transform, int flag )
{
	ClusterData cluster;
	cluster.transform = transform;
	cluster.pieces = NULL;
	cluster_list.push_back( cluster );
	PopulateSurface( cluster_list.back(), 0.2f, flag );
}
//====================================================
void Water::PopulateSurface( ClusterData& cluster, float density, int flag )
{
	float surface_width = cluster.transform->Scale.x;
	int side_x = (int)(surface_width / PIECE_WIDTH);
	if ( side_x < 1 ) side_x = 1;
	cluster.count = side_x;
	surface_width = (float)side_x * PIECE_WIDTH;
	cluster.pieces = new UnitVertex[cluster.count];
	UnitVertex* pieces = cluster.pieces;

	float x = cluster.transform->Position.x;
	x -= surface_width * 0.5f;
	x += 0.5f * PIECE_WIDTH;
	float y = cluster.transform->Position.y;
	float height = cluster.transform->Scale.y;
	for ( int i = 0; i < side_x; ++i )
	{
		pieces[i].position = Vec3( x, y, height );
		x += PIECE_WIDTH;
	}
}

}
