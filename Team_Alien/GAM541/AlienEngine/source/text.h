#pragma once

#include "VMath.h"
#include <D3D10.h>
#include <string>

namespace Framework
{

class Text
{
public:
	float x;
	float y;
	ID3D10Buffer* vertex_buffer;
	bool changed;
	std::string text;
	std::string font;
	Vec4 color;
	int height;
	float line_width;
	float text_width;

	Text* Prev;
	Text* Next;

public:
	Text() : text(""),color(0.0f,0.0f,0.0f,1.0f),changed(false),vertex_buffer(NULL) {}
	~Text() { if (vertex_buffer) vertex_buffer->Release(); vertex_buffer = 0; }
	void UpdateText( const char* _text, const char* _font, int font_size ){ text = _text; font = _font; height = font_size; changed = true; }
	void SetColor( Vec4& _color ) { color = _color; changed = true; }
	void SetPosition( float _x, float _y, float _line_width ){ x = _x; y = _y; line_width = _line_width;changed = true; }
};

}//end namespace
