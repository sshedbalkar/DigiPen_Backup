////////////////////////////////////////////////////////////////////////////////
// Filename: depthshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _DEPTHSHADERCLASS_H_
#define _DEPTHSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;

namespace Framework {

class LightClass;

////////////////////////////////////////////////////////////////////////////////
// Class name: DepthShaderClass
////////////////////////////////////////////////////////////////////////////////
class DepthShaderClass
{
	
private:
	struct MatrixBufferType
	{
		D3DXMATRIX World;
		D3DXMATRIX WorldView;
		D3DXMATRIX WorldViewProjection;
		D3DXMATRIX WorldViewInverseTranspose;
		D3DXMATRIX WorldLightProjectionTexture[6];
	};

	struct CMVMBufferType
	{
		D3DXMATRIX viewM[6];
	};

public:
	DepthShaderClass();
	DepthShaderClass(const DepthShaderClass&);
	~DepthShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, const std::vector<LightClass>&, int);

private:
	bool InitializeShader(ID3D11Device*, HWND, char*, char*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, char*);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, const std::vector<LightClass>&, int);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11GeometryShader* m_geometryShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_CMVMBuffer;
};
}
#endif