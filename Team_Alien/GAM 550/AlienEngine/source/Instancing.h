////////////////////////////////////////////////////////////////////////////////
// Filename: Instancing.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _Instancing_H_
#define _Instancing_H_

namespace Framework
{

class ModelComponent;

class Instancing{
public:
	Instancing() : m_instanceBuffer(0){ }
	~Instancing();
	bool InitializeInstancing( ID3D11Device* );
	static void drawInstancing( ModelComponent** pCubeModelThisFrameArray, unsigned int CubeRenderCount_thisLevel, 
		float dt, ID3D11DeviceContext* deviceContext, const D3DXMATRIX&, const D3DXMATRIX&, const D3DXMATRIX& );
private:
	ID3D11Buffer* m_instanceBuffer;
};


}

#endif
