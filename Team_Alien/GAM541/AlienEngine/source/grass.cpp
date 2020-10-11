#include "grass.h"
#include "shader.h"
#include "Transform.h"
#include <iostream>

const int MAX_GRASS_BLADES =		1000;
const float COLOR_FALLOFF  =		0.02f;
const float WAVING_ACCELERATION =	0.4f;
const float GRASS_BLADE_WIDTH =		0.15f;
const float GRASS_BLADE_HEIGHT =	0.5f;

namespace Framework
{
//==============================================
Grass::Grass()
{
}
//==============================================
Grass::~Grass()
{
	Free();
}
//==============================================
void Grass::Initialize( ID3D10Device* device )
{
	D3D10_BUFFER_DESC vb_desc;
	ZeroMemory( &vb_desc, sizeof( vb_desc ) );
	vb_desc.Usage = D3D10_USAGE_DYNAMIC;
	vb_desc.ByteWidth = sizeof( GrassBladeData ) * MAX_GRASS_BLADES;
	vb_desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vb_desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	vb_desc.MiscFlags = 0;

	device->CreateBuffer( &vb_desc, NULL, &vb );
	mat_world_ptr = shader->GetVariableByName("mat_world")->AsMatrix();
}
//==============================================
void Grass::Update( float timeslice )
{
	ClusterListType::iterator cit;
	for ( cit = cluster_list.begin(); cit != cluster_list.end(); ++cit )
	{
		if ( cit->bend < 0 ) cit->waving_speed += WAVING_ACCELERATION * timeslice;
		else cit->waving_speed -= WAVING_ACCELERATION * timeslice;
		float db = cit->waving_speed * timeslice;
		cit->bend += db;
		for ( int i = 0; i < cit->count; ++i )
		{
			cit->blades[i].extra.z += db;
		}
	}
}
//==============================================
void Grass::Draw( ID3D10Device* device, Mat4& mat_view, Mat4& mat_proj, float* reflect_altitude )
{
	unsigned int stride = sizeof( GrassBladeData ), offset = 0;
	GrassBladeData* mapped;
	Mat4 mat_world;

	shader->SetTransformationConstants( device, mat_view, mat_proj );
	device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	ClusterListType::iterator cit;
	for ( cit = cluster_list.begin(); cit != cluster_list.end(); ++cit )
	{
		vb->Map( D3D10_MAP_WRITE_DISCARD, 0, (void**)&mapped );
		int count = cit->count;
		if ( count > MAX_GRASS_BLADES ) {printf("grass blades overflowed."); count = MAX_GRASS_BLADES; }
		memcpy( mapped, cit->blades, count * sizeof( GrassBladeData ) );
		vb->Unmap();
		cit->transform->BuildMatrix( mat_world );
		mat_world_ptr->SetMatrix( (float*)&mat_world );
		device->IASetVertexBuffers(0,1,&vb,&stride,&offset );
		shader->Apply( device );
		device->Draw( count, 0 );
	}

}
//==============================================
void Grass::Unload()
{
	ClusterListType::iterator cit;
	for ( cit = cluster_list.begin(); cit != cluster_list.end(); ++cit )
	{
		delete [] cit->blades;
	}
	cluster_list.clear();
}
//==============================================
void Grass::Free()
{
	Unload();
	S_RELEASE(vb);
}
//==============================================
void Grass::RegisterVolume( Transform* transform, int flag )
{
	ClusterData cluster;
	cluster.transform = transform;
	cluster.blades = NULL;
	cluster_list.push_back( cluster );
	PopulateSurface( cluster_list.back(), 3.0f, flag );
}
//==============================================
void Grass::PopulateSurface( ClusterData& cluster, float density, int flag )
{
	Transform* transform = cluster.transform;
	GrassBladeData** pblades = &cluster.blades;
	int count = static_cast<int>(( transform->Scale.x ) * density);
	cluster.count = count;
	cluster.waving_speed = 0.0f;
	cluster.bend = 0.1f;
	*pblades = new GrassBladeData[count];
	GrassBladeData* blades = *pblades;

	float step = GRASS_BLADE_WIDTH * 0.8f;
	step /= transform->Scale.x;
	float x;
	float y = 0.5f;
	float z = 0.5f;
	float bend;
	float step_bend;
	float height;
	float step_height;
//	float step_step_height;
	int bunch = 0;

	for ( int i = 0; i < count; ++i )
	{
		if ( bunch == 0 )
		{
			bunch = (int)Rand(7.0f,9.0f);
			x = Rand( -0.5f + GRASS_BLADE_WIDTH * 7.0f / transform->Scale.x, 0.5f - GRASS_BLADE_WIDTH * 7.0f / transform->Scale.x );
			bend = Rand( -0.6f, -0.5f );
			step_bend = 1.0f / (float)bunch;
			height = 0.7f;
			step_height = 0.15f;
		}
		float f = GaussianRand( 0.0f, 0.2f );
		blades[i].position = Vec3( x, y, z );
		blades[i].extra = Vec3( GRASS_BLADE_WIDTH, GaussianRand(GRASS_BLADE_HEIGHT * height, GRASS_BLADE_HEIGHT * height * 0.1f ), cluster.bend + bend );
		blades[i].color = Vec4( 0.4f, Rand(0.8f,0.9f),0.2f,1.0f );

		x += step;
		--bunch;
		bend += step_bend;
		height += step_height;
		step_height -= 0.05f;
	}
}

}//end namespace