#include "Spikes.h"
#include "shader.h"
#include "Transform.h"
#include "Graphics.h"
#include <iostream>

const int MAX_SPIKES_PIECES		= 100;
const float SEAM_WIDTH_RATE		= 0.2f;

namespace Framework
{

//====================================================
Spikes::Spikes()
{
}
//====================================================
Spikes::~Spikes()
{
	Free();
}
//====================================================
void Spikes::Initialize( ID3D10Device* device )
{
	D3D10_BUFFER_DESC vb_desc;
	ZeroMemory( &vb_desc, sizeof( vb_desc ) );
	vb_desc.Usage = D3D10_USAGE_DYNAMIC;
	vb_desc.ByteWidth = sizeof( SpikesPieceData ) * MAX_SPIKES_PIECES;
	vb_desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vb_desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	vb_desc.MiscFlags = 0;

	device->CreateBuffer( &vb_desc, NULL, &vb );

	mat_world_ptr = shader->GetVariableByName("mat_world")->AsMatrix();
}
//====================================================
void Spikes::Update( float timeslice )
{
	static_cast<void>(timeslice);
}
//====================================================
void Spikes::Draw( ID3D10Device* device, Mat4& mat_view, Mat4& mat_proj, float* reflect_altitude )
{
	unsigned int stride = sizeof( SpikesPieceData ), offset = 0;
	SpikesPieceData* mapped;
	Mat4 mat_world;

	shader->SetTransformationConstants( device, mat_view, mat_proj );
	device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	ClusterListType::iterator cit;
	for ( cit = cluster_list.begin(); cit != cluster_list.end(); ++cit )
	{
		vb->Map( D3D10_MAP_WRITE_DISCARD, 0, (void**)&mapped );
		int count = cit->count;
		memcpy( mapped, cit->pieces, count * sizeof( SpikesPieceData ) );
		vb->Unmap();
		cit->transform->BuildMatrix( mat_world );
		mat_world_ptr->SetMatrix( (float*)&mat_world );
		device->IASetVertexBuffers(0,1,&vb,&stride,&offset );
		shader->Apply( device );
		device->Draw( count, 0 );
	}
}
//====================================================
void Spikes::Unload()
{
	ClusterListType::iterator cit;
	for ( cit = cluster_list.begin(); cit != cluster_list.end(); ++cit )
	{
		delete [] cit->pieces;
	}
	cluster_list.clear();
}
//====================================================
void Spikes::Free()
{
	Unload();
	S_RELEASE(vb);
}
//====================================================
void Spikes::RegisterVolume( Transform* transform, int flag )
{
	ClusterData cluster;
	cluster.transform = transform;
	cluster.pieces = NULL;
	cluster_list.push_back( cluster );
	PopulateSurface( cluster_list.back(),5.0f, flag );
}
//====================================================
void Spikes::PopulateSurface( ClusterData& cluster, float density, int flag )
{
//	Surface* surface = cluster.surface;
	float surface_width = cluster.transform->Scale.x;
	int side_x = static_cast<int>( density * surface_width);
	float piece_width = surface_width / static_cast<float>(side_x);
	float step = piece_width / surface_width;
	float seam_width = piece_width * SEAM_WIDTH_RATE;
	piece_width -= seam_width;
	piece_width /= surface_width;
	cluster.count = side_x;
	cluster.pieces = new SpikesPieceData[cluster.count];
	SpikesPieceData* pieces = cluster.pieces;

	float x = -0.5f + step * 0.5f;

	for ( int i = 0; i < side_x; ++i )
	{
		pieces[i].position.x = x;
		pieces[i].position.y = piece_width;
		pieces[i].position.z = Rand( 0.5f, 0.8f );

		x += step;
	}
}

}
