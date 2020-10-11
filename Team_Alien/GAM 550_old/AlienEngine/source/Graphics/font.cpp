#include <fstream>
#include "font.h"


namespace Framework
{
//=======================================================
void Font::Initialize( const char* path )
{
	
	std::ifstream fin( path, std::ios::in | std::ios::binary );
	unsigned short width[256];
	fin.read( (char*)width, 512 );
	fin.close();

	for ( int i = 0; i < 95; ++i )
	{
		float row = (float) (i / 16);
		float center = (float)( i % 16 ) * 32.0f + 16.0f;
		letters[i].size = (float)width[i];
		letters[i].left = center - (float)width[i] / 2.0f;
		letters[i].left *= 0.001953125f;
		letters[i].right = center + (float)width[i] / 2.0f;
		letters[i].right *= 0.001953125f;

		letters[i].y = row * 32.0f + 18.0f;
		letters[i].y *= 0.001953125f;
	}
}
//=======================================================
void Font::Apply( const char* sentence, float size, float aspect, void* vdata )
{
	float pos_x = 0.0f, pos_y = 0.0f;
	float width_per = size / 32.0f;

	pos_x = 0.0f;
	pos_y = 0.0f;

	float* data = static_cast<float*>(vdata);
	float h = 720.0f;
	float w = h * aspect;

	while ( *sentence != 0 )
	{
		LetterData* letter = &letters[(*sentence) - 32];
		
		// center position
		*data = pos_x + letter->size * width_per * 0.5f; *data /= w; ++data;
		*data = pos_y + size * 0.5f; *data /= h; ++data;
		// size
		*data = letter->size * width_per; *data /= w; ++data;
		*data = size; *data /= h; ++data;
		// tex center
		*data = 0.5f * ( letter->left + letter->right ); ++data;
		*data = letter->y; ++data;
		// tex size
		*data = letter->right - letter->left; ++data;
		*data = -0.0625f; ++data;
		
		++sentence;
		pos_x += letter->size * width_per;
		pos_x += 1.0f;
	}
}
//=======================================================
void Font::Release()
{
	delete this;
}

}