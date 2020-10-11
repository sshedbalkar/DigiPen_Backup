////////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_
namespace Framework{

/////////////
// LINKING //
/////////////
//#pragma comment(lib, "dxgi.lib")
//#pragma comment(lib, "d3d11.lib")
//#pragma comment(lib, "d3dx11.lib")
//#pragma comment(lib, "d3dx10.lib")
//
//
////////////////
//// INCLUDES //
////////////////
//#include <dxgi.h>
//#include <d3dcommon.h>
//#include <d3d11.h>
//#include <d3dx10math.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: D3DClass
////////////////////////////////////////////////////////////////////////////////
class D3DClass
{
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	bool FULLSCREEN;
	bool VSYNC;

	void SetFullScreen(bool choice);
	void Resize(float w, float h, float n, float d);
	bool Initialize(int, int, HWND, float, float);
	void FreeD3DBeforeResize();
	void Shutdown();
	
	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(D3DXMATRIX&);
	void GetWorldMatrix(D3DXMATRIX&);
	void GetOrthoMatrix(D3DXMATRIX&);

	void GetVideoCardInfo(char*, int&);

	void TurnZBufferOn();
	void TurnZBufferOff();

	void TurnOnAlphaBlending();
	void TurnOffAlphaBlending();

	void TurnOnTransparentDSS();
	void TurnOffTransparentDSS();

	void ClearDB();

	void TurnWireFrameOn();
	void TurnWireFrameOff();
	void SetSMRasterStateOn();
	void SetSMRasterStateOff();

	ID3D11RenderTargetView* GetBackBuffer();
	void SetBackBuffer(ID3D11RenderTargetView*);

	ID3D11DepthStencilView* GetDepthStencilView();
	void SetBackBufferRenderTarget();
	void ResetViewport();
	void Reset();
	void Reset(ID3D11DepthStencilView* temp);
	void SetDSV(ID3D11DepthStencilView* temp);

	ID3D11DepthStencilState* GetDSS() {return m_depthStencilState;}
	ID3D11RasterizerState* GetRS() {return m_rasterState;}

	IDXGISwapChain* GetSwapChain() {return m_swapChain;}

private:
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilState* TransparentDSS;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterState;
	ID3D11RasterizerState* m_rasterStateWireFrame;
	ID3D11RasterizerState* m_rasterStateSM;
	D3DXMATRIX m_projectionMatrix;
	D3DXMATRIX m_worldMatrix;
	D3DXMATRIX m_orthoMatrix;
	ID3D11DepthStencilState* m_depthDisabledStencilState;
	ID3D11BlendState* m_alphaEnableBlendingState;
	ID3D11BlendState* m_alphaDisableBlendingState;	
	D3D11_VIEWPORT m_viewport;
};

}

#endif
