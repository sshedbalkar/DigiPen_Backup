#include <fstream>
#include <iostream>
#include "font.h"
#include "text.h"

namespace Framework
{

const int MAX_STRING_LENGTH = 80;

//===================================================
Font::Font()
{
}
//===================================================
Font::~Font()
{
}
//===================================================
void Font::Initialize( ID3D10Device* device, const char* bmp_file, const char* dat_file )
{
	font_data = new FontDataType[95];
	std::ifstream fin( dat_file, std::ios::in | std::ios::binary );
	WORD width[256];
	fin.read( (char*)width, 512 );
	fin.close();

	D3DX10CreateShaderResourceViewFromFile( device, bmp_file, NULL, NULL, &font_img, NULL );

	for ( int i = 0; i < 95; ++i )
	{
		float row = (float) (i / 16);
		float center = (float)( i % 16 ) * 32.0f + 17.0f;
		font_data[i].size24 = (float)width[i];
		font_data[i].left24 = center - (float)width[i] / 2.0f;
		font_data[i].left24 *= 0.001953125f;
		font_data[i].right24 = center + (float)width[i] / 2.0f;
		font_data[i].right24 *= 0.001953125f;

		font_data[i].size16 = (float)width[i+128];
		font_data[i].left16 = center - font_data[i].size16 / 2.0f;
		font_data[i].left16 *= 0.001953125f;
		font_data[i].right16 = center + font_data[i].size16 / 2.0f;
		font_data[i].right16 *= 0.001953125f;

		font_data[i].y24 = row * 32.0f + 18.0f;
		font_data[i].y24 *= 0.001953125f;
		font_data[i].y16 = row * 32.0f + 18.0f + 256.0f;
		font_data[i].y16 *= 0.001953125f;

//		std::cout <<(char)(i+32)<<" "<<font_data[i].left24 <<" "<<font_data[i].right24<<" "<<font_data[i].size24<<" "<<font_data[i].y24<<std::endl;
	}
}
//===================================================
void Font::Free()
{
}
//===================================================
void Font::BuildVertexBuffer( ID3D10Device* device, ID3D10Buffer** vb )
{
	D3D10_BUFFER_DESC vb_desc;
	HRESULT hr;
	ZeroMemory( &vb_desc, sizeof( vb_desc ) );
	vb_desc.Usage = D3D10_USAGE_DYNAMIC;
	vb_desc.ByteWidth = sizeof( TextVertex ) * MAX_STRING_LENGTH;
	vb_desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vb_desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	vb_desc.MiscFlags = 0;
	hr = device->CreateBuffer( &vb_desc, NULL, vb );
	if ( FAILED(hr) ) exit(0);/////ErrorIf

}
//===================================================
void Font::UpdateVertexBuffer( Text* text )
{
	TextVertex* mapped_data;
	text->vertex_buffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void**)&mapped_data );
	float pos_x = text->x, pos_y = text->y;
	float height = (float)text->height;
	float width_per = height / 32.0f;

	std::string::iterator it;
	it = text->text.begin();
	char ch = ' ';
	while ( it != text->text.end() )
	{
		FontDataType* letter = &font_data[(*it)-32];
		if ( pos_x + letter->size24 * width_per > text->x + text->line_width )
		{
			if (( ch >= 'a' && ch <='z') || ( ch >='A' && ch <= 'Z' ) )
				if (( (*it) >= 'a' && (*it) <='z') || ( (*it) >='A' && (*it) <= 'Z' ) )
						it = text->text.insert( it, '-' );
			pos_x = text->x;
		}

		pos_x += letter->size24 * width_per + 1.0f;
		ch = *it;
		++it;
	}
//	std::cout <<text->text;
	pos_x = text->x;
	pos_y = text->y;

	int i = 0;
	for ( it = text->text.begin(); it != text->text.end(); ++it )
	{
		FontDataType* letter = &font_data[(*it)-32];
		
		if (( (*it) >= 'a' && (*it) <='z') || ( (*it) >='A' && (*it) <= 'Z' ) )
		{
			if ( pos_x + letter->size24 * width_per > text->x + text->line_width ) 
			{ pos_x = text->x; pos_y -= height + 1.0f; }
		}
		else if ( pos_x > text->x + text->line_width ) { pos_x = text->x; pos_y -= height + 1.0f; }
		mapped_data[i].color = text->color;
		mapped_data[i].position0 = Vec3( pos_x, pos_y - height, 0.0f );
		mapped_data[i].position1 = Vec3( pos_x + letter->size24 * width_per, pos_y, 0.0f );
		mapped_data[i].tex0 = Vec2( 0.5f * ( letter->left24 + letter->right24 ), letter->y24 );
		mapped_data[i].tex1 = Vec2( 0.5f * ( letter->right24 - letter->left24 ),  0.03125f );
		++i;
		if ( i >= MAX_STRING_LENGTH ) { printf("string exceeded 255 characters. the extra part is truncated.\n"); break;}
		pos_x += letter->size24 * width_per;
		pos_x += 1.0f;

	}

	text->vertex_buffer->Unmap();
	text->changed = false;
}


}//end namespace