#pragma once

#include "VMath.h"
#include "DirectXIncludes.h"

#define VERTEX_ELEMENT_POSITION				1
#define VERTEX_ELEMENT_NORMAL				2
#define VERTEX_ELEMENT_COLOR				4
#define VERTEX_ELEMENT_TEXCOORD0			8
#define VERTEX_ELEMENT_TEXCOORD1			16
#define VERTEX_ELEMENT_BONE					32
#define VERTEX_ELEMENT_TRANSFORM			64

namespace Framework
{

class Shader
{
	ID3D10Effect* effect;
	ID3D10EffectTechnique* technique;
	ID3D10InputLayout* input_layout;

public:
	Shader();
	~Shader();

	void Initialize( ID3D10Device* device, const char* filename, unsigned int vertex_format );
	void Free();

	void SetUsingShader( ID3D10Device* device );
	void SetTransformationConstants( ID3D10Device* device, Mat4& mat_view, Mat4& mat_proj );

	ID3D10EffectConstantBuffer* GetConstantBufferByName( const char* name ) { return effect->GetConstantBufferByName(name); }
	ID3D10EffectVariable* GetVariableByName( const char* name ) { return effect->GetVariableByName(name); }

private:
	void CompileEffectFromFile( ID3D10Device*, const char* filename );
	ID3D10EffectMatrixVariable* mat_view_ptr;
	ID3D10EffectMatrixVariable* mat_proj_ptr;
};

}
