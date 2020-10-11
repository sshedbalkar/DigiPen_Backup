#pragma once

#include <Windows.h>
#include <D3D10.h>

namespace Framework
{

class D3DManager
{
	ID3D10Device* device;
	IDXGISwapChain* swap_chain;
	ID3D10RenderTargetView* render_target_screen;
	ID3D10ShaderResourceView* srv_screen;
	ID3D10Texture2D* depth_stencil_buffer;
	ID3D10DepthStencilView* depth_stencil_view;
	D3D10_VIEWPORT viewport;
	D3D10_VIEWPORT viewport2;
	D3D10_VIEWPORT viewport4;
	D3D10_VIEWPORT viewport8;
	ID3D10DepthStencilState* depth_enable_state;
	ID3D10DepthStencilState* depth_disable_state;
	ID3D10RasterizerState* rasterizer_state;
	ID3D10RasterizerState* wireframe_state;
//	ID3D10BlendState* blend_disable_state;
	ID3D10BlendState* blend_add_diffuse_state;
	ID3D10BlendState* blend_sub_diffuse_state;

public:
	D3DManager();
	~D3DManager();

	void Initialize( HWND hwnd, bool full_screen, unsigned int w, unsigned int h );
	void Shutdown();

	void SetBlendAddDiffuse();
	void SetBlendSubDiffuse();
//	void SetBlendDisable();
	void SetDepthEnable( bool enable = true );
	void SetRenderWireFrame( bool enable = true );
	ID3D10Device* GetDevice() { return device; }
	ID3D10RenderTargetView* GetRenderTargetScreen() { return render_target_screen; }
	ID3D10ShaderResourceView* GetShaderResourceScreen() { return srv_screen; }
	ID3D10DepthStencilView* GetDepthStencilView() { return depth_stencil_view; }
	IDXGISwapChain* GetSwapChain() { return swap_chain; }
	D3D10_VIEWPORT* GetViewport() { return &viewport; }
	void SetViewport();
	void SetViewport2();
	void SetViewport4();
	void SetViewport8();
private:
	
	void GetRefreshRate();
	void CreateSwapChainAndDevice( HWND hwnd, bool full_screen, unsigned int w, unsigned int h );
	void CreateViewport( unsigned int w, unsigned int h );
	void CreateRenderTargetsAndDepthBuffer( unsigned int w, unsigned int h );
	void CreateRasterizerStates();
	void CreateBlendStates();
};


}