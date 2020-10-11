#include "Skyline.h"
#include "shader.h"
#include "Transform.h"
#include "Graphics.h"
#include "texture.h"
#include <iostream>

const int MAX_SKYLINE_UNITS		= 1000;

namespace Framework
{

//====================================================
Skyline::Skyline()
{
}
//====================================================
Skyline::~Skyline()
{
	Free();
}
//====================================================
void Skyline::Initialize( ID3D10Device* device )
{
	D3D10_BUFFER_DESC vb_desc;
	ZeroMemory( &vb_desc, sizeof( vb_desc ) );
	vb_desc.Usage = D3D10_USAGE_DYNAMIC;
	vb_desc.ByteWidth = sizeof( UnitVertex ) * MAX_SKYLINE_UNITS;
	vb_desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vb_desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	vb_desc.MiscFlags = 0;

	device->CreateBuffer( &vb_desc, NULL, &vb );

	normal_map_ptr = shader->GetVariableByName("normal_map")->AsShaderResource();
	diffuse_ptr = shader->GetVariableByName("diffuse")->AsShaderResource();
	mat_world_ptr = shader->GetVariableByName("mat_world")->AsMatrix();
	Texture* texture = GRAPHICS->GetTexture("Skyline");
	normal_map_ptr->SetResource( texture->GetNormal() );
	diffuse_ptr->SetResource( texture->GetDiffuse() );

}
//====================================================
void Skyline::Update( float timeslice )
{
}
//====================================================
void Skyline::Draw( ID3D10Device* device, Mat4& mat_view, Mat4& mat_proj, float* reflect_altitude )
{
	unsigned int stride = sizeof( UnitVertex ), offset = 0;
	UnitVertex* mapped;
	Mat4 mat_world;

	shader->SetTransformationConstants( device, mat_view, mat_proj );
	device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	ClusterListType::iterator cit;
	for ( cit = cluster_list.begin(); cit != cluster_list.end(); ++cit )
	{
		if ( !reflect_altitude || ( *reflect_altitude < cit->transform->Position.y ) )
		{
			vb->Map( D3D10_MAP_WRITE_DISCARD, 0, (void**)&mapped );
			int count = cit->count;
			memcpy( mapped, cit->pieces, count * sizeof( UnitVertex ) );
			vb->Unmap();
			cit->transform->BuildMatrix( mat_world );
			mat_world_ptr->SetMatrix( (float*)&mat_world );
			device->IASetVertexBuffers(0,1,&vb,&stride,&offset );
			shader->Apply( device );
			device->Draw( count, 0 );// std::cout <<count <<std::endl;
		}
	}
}
//====================================================
void Skyline::Unload()
{
	ClusterListType::iterator cit;
	for ( cit = cluster_list.begin(); cit != cluster_list.end(); ++cit )
	{
		delete [] cit->pieces;
	}
	cluster_list.clear();
}
//====================================================
void Skyline::Free()
{
	Unload();
	S_RELEASE(vb);
}
//====================================================
void Skyline::RegisterVolume( Transform* transform, int flag )
{
	ClusterData cluster;
	cluster.transform = transform;
	cluster.pieces = NULL;
	cluster_list.push_back( cluster );
	PopulateSurface( cluster_list.back(), 0.1f, flag );
}
//====================================================
void Skyline::PopulateSurface( ClusterData& cluster, float density, int flag )
{
	float surface_width = cluster.transform->Scale.x;
	int side_x = static_cast<int>( density * surface_width);
	float piece_width = 1.0f / static_cast<float>(side_x);
//	piece_width -= seam_width;
	cluster.count = side_x;
	cluster.pieces = new UnitVertex[cluster.count];
	UnitVertex* pieces = cluster.pieces;
	float x = -0.5f + piece_width * 0.5f;

	for ( int i = 0; i < side_x; ++i )
	{
		float width = GaussianRand( piece_width, piece_width * 0.5f );
		x += width * 0.5f;
		float height = Rand( 1.5f, 2.0f );
		pieces[i].position = Vec3( x, width, height );
		pieces[i].tip.x = Rand( 0.97f, 0.99f );
		pieces[i].tip.y = GaussianRand( 0.2f, 0.2f );
		pieces[i].tip.z = 1.0f - 2 * pieces[i].tip.y;
		//pieces[i].tip.y = Rand( 0.0f, 1.0f );
		//pieces[i].tip.z = Rand( 0.0f, 1.0f - pieces[i].tip.y );
		x += width * 0.5f;
	}
}

}
