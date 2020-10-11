#include "renderTargetTexture.h"
#include "DebugDiagnostic.h"

namespace Framework
{
	
//=======================================================
RenderTargetTexture::RenderTargetTexture() : texture(NULL), srv(NULL), rtv(NULL)
{
}
//=======================================================
RenderTargetTexture::~RenderTargetTexture()
{
	Release();
}
//=======================================================
void RenderTargetTexture::Initialize( ID3D10Device* device, int width, int height )
{
	D3D10_TEXTURE2D_DESC texture_desc;
	D3D10_RENDER_TARGET_VIEW_DESC rtv_desc;
	D3D10_SHADER_RESOURCE_VIEW_DESC srv_desc;
	HRESULT hr;

	ZeroMemory( &texture_desc, sizeof( texture_desc ) );
	texture_desc.Width = width;
	texture_desc.Height = height;
	texture_desc.MipLevels = 3;
	texture_desc.ArraySize = 1;
	texture_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texture_desc.SampleDesc.Count = 1;
	texture_desc.Usage = D3D10_USAGE_DEFAULT;
	texture_desc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
	texture_desc.CPUAccessFlags = 0;
	texture_desc.MiscFlags = 0;

	hr = device->CreateTexture2D( &texture_desc, NULL, &texture );
	ErrorIf( FAILED(hr), "failed creating texture as render target.\n" );

	rtv_desc.Format = texture_desc.Format;
	rtv_desc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
	rtv_desc.Texture2D.MipSlice = 0;

	hr = device->CreateRenderTargetView( texture, &rtv_desc, &rtv );
	ErrorIf( FAILED(hr), "failed creating render target view.\n" );

	srv_desc.Format = texture_desc.Format;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MostDetailedMip = 0;
	srv_desc.Texture2D.MipLevels = 1;

	hr = device->CreateShaderResourceView( texture, &srv_desc, &srv );
	ErrorIf( FAILED(hr), "failed creating shader resource view.\n" );

	texture->Release();
	texture = NULL;
}
//=======================================================
void RenderTargetTexture::Release()
{
	if ( rtv ) {rtv->Release(); rtv = 0;}
	if ( srv ) {srv->Release(); srv = 0;}
	if ( texture ) {texture->Release(); texture = 0; }
}

}//end namespace