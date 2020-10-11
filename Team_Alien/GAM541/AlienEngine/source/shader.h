#pragma once

#include "VMath.h"
#include "DirectXIncludes.h"

#define VERTEX_ELEMENT_POSITION0			1
#define VERTEX_ELEMENT_POSITION1			2
#define VERTEX_ELEMENT_POSITION2			4
#define VERTEX_ELEMENT_POSITION3			8
#define VERTEX_ELEMENT_NORMAL				16
#define VERTEX_ELEMENT_COLOR				32
#define VERTEX_ELEMENT_TEXCOORD0			64
#define VERTEX_ELEMENT_TEXCOORD1			128
#define VERTEX_ELEMENT_BONES				256
#define VERTEX_ELEMENT_WEIGHTS				512
#define VERTEX_ELEMENT_PER_VERTEX_FLAG		1024
#define VERTEX_ELEMENT_TRANSFORM			2048
#define VERTEX_ELEMENT_PER_INSTANCE_FLAG	4096

namespace Framework
{

class Shader
{
	ID3D10Effect* effect;
	ID3D10EffectTechnique* technique;
	ID3D10InputLayout* input_layout;
	ID3D10EffectMatrixVariable* mat_view_ptr;
	ID3D10EffectMatrixVariable* mat_proj_ptr;

	unsigned int pass_count;
public:
	Shader();
	~Shader();

	void Initialize( ID3D10Device* device, const char* filename, unsigned int vertex_format );
	void Free();

//	void Draw( ID3D10Device* device, int vertex_count );
//	void DrawInstanced( ID3D10Device* device, int vertex_per_inst, int instance_count );
	void Apply( ID3D10Device* device );
	void SetTransformationConstants( ID3D10Device* device, Mat4& mat_view, Mat4& mat_proj );

	ID3D10EffectConstantBuffer* GetConstantBufferByName( const char* name ) { return effect->GetConstantBufferByName(name); }
	ID3D10EffectVariable* GetVariableByName( const char* name ) { return effect->GetVariableByName(name); }

private:
	void CompileEffectFromFile( ID3D10Device*, const char* filename );
};

}
