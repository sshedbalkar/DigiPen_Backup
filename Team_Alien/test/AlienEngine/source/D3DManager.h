#pragma once

#include <Windows.h>
#include "DirectXIncludes.h"

namespace Framework
{

class D3DManager
{
	ID3D10Device* device;
	IDXGISwapChain* swap_chain;
	ID3D10RenderTargetView* render_target_screen;
	ID3D10Texture2D* depth_stencil_buffer;
	ID3D10DepthStencilView* depth_stencil_view;
	D3D10_VIEWPORT viewport;
	ID3D10DepthStencilState* depth_enable_state;
	ID3D10DepthStencilState* depth_disable_state;
	ID3D10RasterizerState* rasterizer_state;
	ID3D10RasterizerState* wireframe_state;

public:
	D3DManager();

	void Initialize( HWND hwnd, bool full_screen, unsigned int w, unsigned int h );
	void Shutdown();

	void SetDepthEnable( bool enable = true );
	void SetRenderWireFrame( bool enable = true );
	ID3D10Device* GetDevice() { return device; }
	ID3D10RenderTargetView* GetRenderTargetScreen() { return render_target_screen; }
	ID3D10DepthStencilView* GetDepthStencilView() { return depth_stencil_view; }
	IDXGISwapChain* GetSwapChain() { return swap_chain; }
private:
	
	void GetRefreshRate();
	void CreateSwapChainAndDevice( HWND hwnd, bool full_screen, unsigned int w, unsigned int h );
	void CreateViewport( unsigned int w, unsigned int h );
	void CreateRenderTargetsAndDepthBuffer( unsigned int w, unsigned int h );
	void CreateRasterizerStates();
};


}