#pragma once

#include <D3D10.h>

namespace Framework
{

class Texture
{
	ID3D10ShaderResourceView* diffuse;
	ID3D10ShaderResourceView* mask;
	ID3D10ShaderResourceView* normal;

public:
	Texture();
	~Texture();

	void Free();
	void LoadDiffuse( ID3D10Device* device, const char* filename );
	void LoadMask( ID3D10Device* device, const char* filename );
	void LoadNormal( ID3D10Device* device, const char* filename );

	ID3D10ShaderResourceView* GetDiffuse() { return diffuse; }
	ID3D10ShaderResourceView* GetMask() { return mask; }
	ID3D10ShaderResourceView* GetNormal() { return normal; }

};

}
