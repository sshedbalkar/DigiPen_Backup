#include "visualPlatform.h"
#include "Composition.h"
#include "Transform.h"
#include "Graphics.h"
#include "texturedbox.h"
#include <iostream>

namespace Framework
{

//===================================================
VisualPlatform::VisualPlatform() : texture( NULL )
{
}
//===================================================
VisualPlatform::~VisualPlatform()
{
}
//===================================================
void VisualPlatform::Serialize( ISerializer& stream )
{
	ISerializer* stream_platform = stream.clone();
	std::string str;
	stream_platform->readValue( "Box", str );
	if ( str != "none" ) texture = GRAPHICS->GetTexture( str.c_str() );
	stream_platform->readValue( "Top", str );
	top = Material::GetMaterialTypeFromString( str );
	stream_platform->readValue( "Bottom", str );
	bottom = Material::GetMaterialTypeFromString( str );
	if ( !stream_platform->readValue( "Shininess", shininess ) ) shininess = 20;
	if ( !stream_platform->readValue( "BloomIntensity", bloom_intensity ) ) bloom_intensity = 0.0f;
	if ( !stream_platform->readValue( "TextureMethod", tex_method ) ) tex_method = 1.0f;
	int i;
	if ( !stream_platform->readValue( "Flag", i ) ) i = 37;
	flag = (unsigned int)i;

	delete stream_platform;
}
//===================================================
void VisualPlatform::Initialize()
{
	transform = GetOwner()->has( Transform );
	if ( !transform ) return;
	
	TexturedBox* box_material = (TexturedBox*)GRAPHICS->GetMaterial( MAT_TEXTUREDBOX );
	if ( box_material )
	{
		if ( texture )
		{
			box_material->SetTexture( &texture, shininess, bloom_intensity, tex_method, flag );
			box_material->RegisterVolume( transform, 0 );
		}
	}

	Material* top_material = GRAPHICS->GetMaterial( top );
	if ( top_material )
		top_material->RegisterVolume( transform, 1 );
	
	Material* bottom_material = GRAPHICS->GetMaterial( bottom );
	if ( bottom_material )
		bottom_material->RegisterVolume( transform, -1 );
}
//===================================================
void VisualPlatform::Free()
{
}
//===================================================
void VisualPlatform::ChangeTexture( const char* id )
{
	Texture* t = GRAPHICS->GetTexture( id );
	if ( t ) texture = t;
}

}//end namespace