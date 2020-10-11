#include "texture.h"
#include "VMath.h"
#include "DebugDiagnostic.h"

namespace Framework
{
//===============================================
Texture::Texture() : diffuse(NULL), mask(NULL), normal(NULL)
{
}
//===============================================
Texture::~Texture()
{
	Free();
}
//===============================================
void Texture::Free()
{
	S_RELEASE(diffuse);
	S_RELEASE(mask);
	S_RELEASE(normal);
}
//===============================================
void Texture::LoadDiffuse( ID3D10Device* device, const char* filename )
{
	HRESULT hr;
	D3DX10CreateShaderResourceViewFromFile( device, filename, NULL, NULL, &diffuse, &hr );
	ErrorIf( FAILED(hr), "failed loading texture %s.\n", filename );
}
//===============================================
void Texture::LoadMask( ID3D10Device* device, const char* filename )
{
	HRESULT hr;
	D3DX10CreateShaderResourceViewFromFile( device, filename, NULL, NULL, &mask, &hr );
	ErrorIf( FAILED(hr), "failed loading texture %s.\n", filename );
}
//===============================================
void Texture::LoadNormal( ID3D10Device* device, const char* filename )
{
	HRESULT hr;
	D3DX10CreateShaderResourceViewFromFile( device, filename, NULL, NULL, &normal, &hr );
	ErrorIf( FAILED(hr), "failed loading texture %s.\n", filename );
}

}