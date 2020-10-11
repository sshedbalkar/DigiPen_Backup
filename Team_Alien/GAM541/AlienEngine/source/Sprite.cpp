///////////////////////////////////////////////////////////////////////////////////////
//
//	Sprite.cpp
//	
//	Authors: , 
//	Copyright 2010, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////

#include "Sprite.h"
#include "Graphics.h"

namespace Framework
{

//======================================================
Sprite::Sprite() : texture(NULL),transform(NULL)
{
}
//======================================================
Sprite::~Sprite()
{
	if ( is_hud_sprite ) 
		GRAPHICS->RemoveSpriteHUD( this );
	else 
		GRAPHICS->RemoveSprite3D( this );
}
//======================================================
void Sprite::Initialize()
{ 
	transform = GetOwner()->has( Transform );
	if ( is_hud_sprite ) 
		GRAPHICS->RegisterSpriteHUD( this );
	else 
		GRAPHICS->RegisterSprite3D( this );
}
//======================================================
void Sprite::Serialize(ISerializer& stream)
{
	std::string str;
	stream.readValue("Texture", str);
	texture = GRAPHICS->GetTexture( str.c_str() );
	stream.readValue("Masked",str);
	if ( str == "true" ) masked = true;
	else masked = false;
	if ( !stream.readValue("FilterFlag", flag ) ) flag = 0;
	stream.readValue("Type",str);
	if ( str == "HUD" ) is_hud_sprite = true;
	else is_hud_sprite = false;
}

}