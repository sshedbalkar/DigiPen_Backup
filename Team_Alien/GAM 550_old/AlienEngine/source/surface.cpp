#include "surface.h"
#include "material.h"
#include "Graphics.h"

namespace Framework
{
	
//=================================================
void Surface::Serialize(ISerializer& stream)
{
}
//=================================================
void Surface::TestingGrassSurface( Vec2& _span_x, Vec2& _span_z, float _density )
{
	span_x = _span_x;
	span_z = _span_z;
	density = _density;

	Material* grass = GRAPHICS->GetMaterialByName( "grass" );
	if ( grass ) grass->RegisterSurface( this );
	else printf("error");
}
//=================================================
void Surface::TestingTileSurface( Vec2& _span_x, Vec2& _span_z, float _density )
{
	span_x = _span_x;
	span_z = _span_z;
	density = _density;

	Material* tile = GRAPHICS->GetMaterialByName( "tile" );
	if ( tile ) tile->RegisterSurface( this );
	else printf("error");
}


}//end namespace