#include "texturedBox.h"
#include "Transform.h"
#include "shader.h"
#include "texture.h"

const int MAX_BOXES = 100;

namespace Framework
{

//===============================================
TexturedBox::TexturedBox() : texture( NULL )
{
}
//===============================================
TexturedBox::~TexturedBox()
{
	Free();
}
//===============================================
void TexturedBox::Initialize( ID3D10Device* device )
{
	D3D10_BUFFER_DESC vb_desc;
	ZeroMemory( &vb_desc, sizeof( vb_desc ) );
	vb_desc.Usage = D3D10_USAGE_DYNAMIC;
	vb_desc.ByteWidth = sizeof( BoxData ) * MAX_BOXES;
	vb_desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vb_desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	vb_desc.MiscFlags = 0;

	device->CreateBuffer( &vb_desc, NULL, &vb );

	diffuse_map_ptr = shader->GetVariableByName("diffuse_map")->AsShaderResource();
	normal_map_ptr = shader->GetVariableByName("normal_map")->AsShaderResource();
	mask_ptr = shader->GetVariableByName("mask")->AsShaderResource();
}
//===============================================
void TexturedBox::Update( float )
{
}
//===============================================
void TexturedBox::Draw( ID3D10Device* device, Mat4& mat_view, Mat4& mat_proj, float* reflect_altitude )
{
	
	int vcount = 0;
	unsigned int stride = sizeof( BoxData ), offset = 0;
	BoxData* mapped;

	shader->SetTransformationConstants( device, mat_view, mat_proj );
	device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	ClusterListType::iterator cit;
	for ( cit = cluster_list.begin(); cit != cluster_list.end(); ++cit )
	{
		ID3D10ShaderResourceView* tex;
		tex = (*cit->texture)->GetDiffuse();
		if ( tex ) diffuse_map_ptr->SetResource( tex );
		else diffuse_map_ptr->SetResource( NULL );

		tex = (*cit->texture)->GetNormal();
		if ( texture ) normal_map_ptr->SetResource( tex );
		else normal_map_ptr->SetResource( NULL );

		tex = (*cit->texture)->GetMask();
		if ( texture ) mask_ptr->SetResource( tex );
		else mask_ptr->SetResource( NULL );

		vb->Map( D3D10_MAP_WRITE_DISCARD, 0, (void**)&mapped );
		
		int vcount = 0;
		for ( unsigned int i = 0; i < (*cit).transforms.size(); ++i )
		{
			mapped[vcount].position = cit->transforms[i]->Position;
			mapped[vcount].size = cit->transforms[i]->Scale;
			mapped[vcount].extra = cit->extra[i];
			mapped[vcount].flag = cit->flags[i];
//				printf("%0.3f %0.3f\n", mapped[vcount].extra.x, mapped[vcount].extra.y );
			++vcount;
		}
		vb->Unmap();
		device->IASetVertexBuffers(0,1,&vb,&stride,&offset );
		shader->Apply( device );
		device->Draw( vcount, 0 );
	}
}
//===============================================
void TexturedBox::Unload()
{
	ClusterListType::iterator cit;
	for ( cit = cluster_list.begin(); cit != cluster_list.end(); ++cit )
	{
//		delete cit->box;
	}
	cluster_list.clear();
}
//===============================================
void TexturedBox::Free()
{
	Unload();
	S_RELEASE(vb);
}
//===============================================
void TexturedBox::RegisterVolume( Transform* transform, int flag )
{
	bool k = false;
	ClusterListType::iterator it;
	Vec3 extra( (float)shininess, bloom_intensity, tex_method );
	for ( it = cluster_list.begin(); it != cluster_list.end(); ++it )
	{
		if ((*it).texture == texture ) { k = true;break;}
	}
	if ( k )
	{
		it->transforms.push_back( transform );
		it->extra.push_back( extra );
		it->flags.push_back( filter_flag );
	}
	else
	{
		ClusterData cluster;
		cluster.transforms.push_back( transform );
		cluster.extra.push_back( extra );
		cluster.flags.push_back( filter_flag );
		cluster.texture = texture;
		cluster_list.push_back( cluster );
	}
}

}