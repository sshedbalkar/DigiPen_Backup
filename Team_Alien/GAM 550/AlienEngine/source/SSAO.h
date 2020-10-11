////////////////////////////////////////////////////////////////////////////////
// Filename: Ssao.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SSAO_H_
#define _SSAO_H_

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;

namespace Framework {

////////////////////////////////////////////////////////////////////////////////
// Class name: SSAO
////////////////////////////////////////////////////////////////////////////////
class SSAO
{
private:
	struct CB_SSAO
	{
		__declspec(align(16)) D3DXVECTOR4 m_SSAO;
	};

public:
	SSAO();
	SSAO(const SSAO&);
	~SSAO();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, const D3DXMATRIX& worldMatrix, const D3DXMATRIX& viewMatrix, const
			      D3DXMATRIX& projectionMatrix, ID3D11ShaderResourceView** textureArray);

private:
	bool InitializeShader(ID3D11Device*, HWND, char*, char*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, char*);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** textureArray);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	
	ID3D11SamplerState* m_sampleStateAniso;
	
	ID3D11Buffer* SSAOBuffer;
};
}
#endif