#pragma once

#include <D3D10.h>
#include "VMath.h"

namespace Framework
{

class Text;

class Font
{
	struct FontDataType
	{
		float size16;				//size in pixel
		float left16,right16;		//size in texcoords
		float size24;				//size24 in pixel
		float left24,right24;		//size24 in texcoords
		float y16;
		float y24;
	};

	struct TextVertex
	{
		Vec3 position0;
		Vec3 position1;
		Vec4 color;
		Vec2 tex0;
		Vec2 tex1;
	};
private:
	ID3D10ShaderResourceView* font_img;
	FontDataType* font_data;

public:
	Font();
	~Font();

	void Initialize( ID3D10Device* device, const char* bmp_file, const char* dat_file );
	void Free();

	static void BuildVertexBuffer( ID3D10Device* device, ID3D10Buffer** vb );
	void UpdateVertexBuffer( Text* text );

	int GetVertexTypeSize() { return sizeof(TextVertex); }
	ID3D10ShaderResourceView* GetFontTexture() { return font_img; }
};

}//end namespace