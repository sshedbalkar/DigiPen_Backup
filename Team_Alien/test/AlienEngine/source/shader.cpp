#include <iostream>
#include "shader.h"


namespace Framework
{

//================================================
Shader::Shader()
{
}
//================================================
Shader::~Shader()
{
}
//================================================
void Shader::Initialize( ID3D10Device* device, const char* filename, unsigned int vertex_format )
{
	unsigned int element_count = 0;
	D3D10_INPUT_ELEMENT_DESC input_elements[9];
	D3D10_PASS_DESC pass_desc;
	HRESULT hr;

	CompileEffectFromFile( device, filename );
	technique = effect->GetTechniqueByIndex( 0 );

	//////ErrorIf

	if ( ( vertex_format & VERTEX_ELEMENT_POSITION ) == VERTEX_ELEMENT_POSITION )
	{
		input_elements[element_count].SemanticName = "POSITION";
		input_elements[element_count].SemanticIndex = 0;
		input_elements[element_count].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		input_elements[element_count].InputSlot = 0;
		input_elements[element_count].AlignedByteOffset = 0;
		input_elements[element_count].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
		input_elements[element_count].InstanceDataStepRate = 0;
		++element_count;
	}
	if ( ( vertex_format & VERTEX_ELEMENT_NORMAL ) == VERTEX_ELEMENT_NORMAL )
	{
		input_elements[element_count].SemanticName = "NORMAL";
		input_elements[element_count].SemanticIndex = 0;
		input_elements[element_count].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		input_elements[element_count].InputSlot = 0;
		input_elements[element_count].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
		input_elements[element_count].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
		input_elements[element_count].InstanceDataStepRate = 0;
		++element_count;
	}
	if ( ( vertex_format & VERTEX_ELEMENT_COLOR ) == VERTEX_ELEMENT_COLOR )
	{
		input_elements[element_count].SemanticName = "COLOR";
		input_elements[element_count].SemanticIndex = 0;
		input_elements[element_count].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		input_elements[element_count].InputSlot = 0;
		input_elements[element_count].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
		input_elements[element_count].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
		input_elements[element_count].InstanceDataStepRate = 0;
		++element_count;
	}
	if ( ( vertex_format & VERTEX_ELEMENT_TEXCOORD0 ) == VERTEX_ELEMENT_TEXCOORD0 )
	{
		input_elements[element_count].SemanticName = "TEXCOORD";
		input_elements[element_count].SemanticIndex = 0;
		input_elements[element_count].Format = DXGI_FORMAT_R32G32_FLOAT;
		input_elements[element_count].InputSlot = 0;
		input_elements[element_count].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
		input_elements[element_count].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
		input_elements[element_count].InstanceDataStepRate = 0;
		++element_count;
	}
	if ( ( vertex_format & VERTEX_ELEMENT_TEXCOORD1 ) == VERTEX_ELEMENT_TEXCOORD1 )
	{
		input_elements[element_count].SemanticName = "TEXCOORD";
		input_elements[element_count].SemanticIndex = 1;
		input_elements[element_count].Format = DXGI_FORMAT_R32G32_FLOAT;
		input_elements[element_count].InputSlot = 0;
		input_elements[element_count].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
		input_elements[element_count].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
		input_elements[element_count].InstanceDataStepRate = 0;
		++element_count;
	}
	if ( ( vertex_format & VERTEX_ELEMENT_BONE ) == VERTEX_ELEMENT_BONE )
	{
		input_elements[element_count].SemanticName = "BONE";
		input_elements[element_count].SemanticIndex = 0;
		input_elements[element_count].Format = DXGI_FORMAT_R32_SINT;
		input_elements[element_count].InputSlot = 0;
		input_elements[element_count].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
		input_elements[element_count].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
		input_elements[element_count].InstanceDataStepRate = 0;
		++element_count;
	}
	if ( ( vertex_format & VERTEX_ELEMENT_TRANSFORM ) == VERTEX_ELEMENT_TRANSFORM )
	{
		for ( int i = 0; i < 4; ++i )
		{
			input_elements[element_count].SemanticName = "TRANSFORM";
			input_elements[element_count].SemanticIndex = i;
			input_elements[element_count].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			input_elements[element_count].InputSlot = 1;
			input_elements[element_count].AlignedByteOffset = i * 16;
			input_elements[element_count].InputSlotClass = D3D10_INPUT_PER_INSTANCE_DATA;
			input_elements[element_count].InstanceDataStepRate = 1;
			++element_count;
		}
	}

	technique->GetPassByIndex(0)->GetDesc( &pass_desc );
	hr = device->CreateInputLayout( input_elements, element_count, pass_desc.pIAInputSignature, 
									pass_desc.IAInputSignatureSize, &input_layout );
	if ( FAILED(hr) ) exit(0);/////ErrorIf

	mat_view_ptr = effect->GetVariableByName( "mat_view" )->AsMatrix();
	mat_proj_ptr = effect->GetVariableByName( "mat_proj" )->AsMatrix();
}
//================================================
void Shader::Free()
{
}
//================================================
void Shader::SetUsingShader( ID3D10Device* device )
{
	device->IASetInputLayout( input_layout );
	technique->GetPassByIndex(0)->Apply(0);
}
//================================================
void Shader::SetTransformationConstants( ID3D10Device* device, Mat4& mat_view, Mat4& mat_proj )
{
	mat_view_ptr->SetMatrix( (float*)&mat_view );
	mat_proj_ptr->SetMatrix( (float*)&mat_proj );
}
//==========================================================
void Shader::CompileEffectFromFile( ID3D10Device* device, const char* filename )
{
	HRESULT hr;
	ID3D10Blob* error_message;

	hr = D3DX10CreateEffectFromFile( filename, NULL, NULL, "fx_4_0", 
									D3D10_SHADER_ENABLE_STRICTNESS, 0, device, 
									NULL, NULL, &effect, &error_message, NULL );
	if ( FAILED(hr) )
	{
		if ( error_message )
		{
			char* errors;
			unsigned int size;
			errors = (char*)(error_message->GetBufferPointer() );
			size = error_message->GetBufferSize();

			for ( unsigned int i = 0; i < size; ++i ) std::cout<<errors[i];
			std::cout <<std::endl;

			error_message->Release();
		}
		exit(0);
	}
}

}