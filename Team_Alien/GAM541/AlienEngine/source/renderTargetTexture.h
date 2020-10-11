#pragma once

#include <D3D10.h>

namespace Framework
{

class RenderTargetTexture
{
	ID3D10RenderTargetView* rtv;
	ID3D10ShaderResourceView* srv;
	ID3D10Texture2D* texture;

public:
	RenderTargetTexture();
	~RenderTargetTexture();

	void Initialize( ID3D10Device* device, int width, int height );
	void Release();
	ID3D10RenderTargetView* GetRTV() { return rtv; }
	ID3D10ShaderResourceView* GetSRV(){ return srv; }
};

}