#include <D3DX11async.h>
#include <fstream>
#include <iostream>
#include "Containers.h"
#include "DebugDiagnostic.h"
#include "Utility.h"
#include "PathHelper.h"
#include "effect.h"

namespace Framework
{
	
const char* sm_level [] = { "vs_4_0", "ps_4_0", "gs_4_0", "hs_5_0", "ds_5_0" };
const int num_vertex_element_types = 3;
const char* semantics [] = { "FLOAT", "INTEGAR", "MATRIX" };
const DXGI_FORMAT vertex_element_formats[4][num_vertex_element_types] = {
					{ DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32_SINT, DXGI_FORMAT_R32G32B32A32_FLOAT },
					{ DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32_SINT, DXGI_FORMAT_R32G32B32A32_FLOAT },
					{ DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32_SINT, DXGI_FORMAT_R32G32B32A32_FLOAT },
					{ DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_SINT, DXGI_FORMAT_R32G32B32A32_FLOAT }
					};

std::map<std::string, std::string> Effect::macro_strings;
std::vector<D3D10_SHADER_MACRO> Effect::macros;

//=======================================================================
void Effect::AddMacro( const char* tag, const char* value )
{
	macro_strings.insert( std::pair<std::string, std::string>( tag, value ) );
	
}
//=======================================================================
void Effect::ApplyMacros()
{
	macros.resize( macro_strings.size() + 1 );
	unsigned int i = 0;
	std::map<std::string, std::string>::iterator it;
	for ( it = macro_strings.begin(); it != macro_strings.end(); ++it )
	{
		macros[i].Name = it->first.c_str();
		macros[i].Definition = it->second.c_str();
		++i;
	}
	macros[i].Name = 0;
	macros[i].Definition = 0;
}
//=======================================================================
void Effect::DXCompile( const char* filename, const char* entrypoint, const char* smlevel, ID3D10Blob** buffer )
{
	ID3D10Blob* err_buffer = NULL;
	if ( strlen( entrypoint ) == 0 ) return;
	HRESULT hr = D3DX11CompileFromFile( filename, &macros[0], NULL, entrypoint, smlevel, 
										D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
										buffer, &err_buffer, NULL);
	if(FAILED(hr))
	{
		if(err_buffer)
		{	
			char* error_msg = (char*)(err_buffer->GetBufferPointer());
			int buffer_size = err_buffer->GetBufferSize();
			std::cout <<"	##shader compiling message:";
			for( int j = 0; j < buffer_size - 2; ++j )
			{
				std::cout << error_msg[j];
			}
			std::cout <<std::endl;
			err_buffer->Release();
			err_buffer = 0;
		}
	}
}
//=======================================================================
Effect::Effect() : vs(NULL),hs(NULL),ds(NULL),gs(NULL),ps(NULL), layout(NULL), elements(NULL), soelements(NULL)
{
}
//=======================================================================
void Effect::Release()
{
	if ( elements ) delete [] elements; elements = NULL;
	if ( soelements ) delete [] soelements; soelements = NULL;
	if ( vs ) vs->Release();
	if ( ps ) ps->Release();
	if ( gs ) gs->Release();
	if ( ds ) ds->Release();
	if ( hs ) hs->Release();
	if ( layout ) layout->Release();

	delete this;
}
//=======================================================================
void Effect::LoadEffect( ID3D11Device* device, const char* path, const char* str_layout, int _topology, const char* _entrypts[5], bool _so )
{
	so = _so;
	////////////////////////// workout the binary file name ///////////////////////////////////////////////////////
	HRESULT hr;
	ID3D10Blob* shader_buffers[5] = {NULL};
	std::string filename;
	PathHelper::PathGetFilename( path, filename );
	for ( int i = 0; i < 5; ++i ) filename += _entrypts[i];
	binfile = "shaders\\binary\\";
	binfile += filename;
	binfile += ".bin";
	////////////////////////// set up the peripherals /////////////////////////////////////////////////////////////
	ParseLayoutFormat( str_layout );
	ErrorIf( num_elements < 0, "error parsing input layout for effect %s.\n", filename.c_str() );
	topology = static_cast<D3D11_PRIMITIVE_TOPOLOGY>(_topology);
	for ( int i = 0; i < 5; ++i ) entrypoints[i] = _entrypts[i];
	////////////////////////// check if it need a recompile ///////////////////////////////////////////////////////
	bool need4recompile = true;
	WIN32_FILE_ATTRIBUTE_DATA filedata;
	SYSTEMTIME modtime, lastmodtime;

	GetFileAttributesEx( path, GetFileExInfoStandard, &filedata );
	FileTimeToSystemTime( &filedata.ftLastWriteTime, &modtime );
	
	std::ifstream fin;
	fin.open( binfile.c_str(), std::ios::in | std::ios::binary );
	if ( !fin ) need4recompile = true;
	else
	{
		fin.read( (char*)&lastmodtime, sizeof( SYSTEMTIME ) );
		if ( lastmodtime == modtime ) need4recompile = false;
		else need4recompile = true;
	}	
	//////////////////////// compile from ascii fx file //////////////////////////////////////////////////////////
	if ( need4recompile )
	{
		fin.close();
		ID3D10Blob* shader_buffers[5] = {NULL};
		LoadFromAscii( device, path, shader_buffers );
		
		DXCall( device->CreateInputLayout(elements, num_elements, 
				shader_buffers[0]->GetBufferPointer(), 
				shader_buffers[0]->GetBufferSize(), 
				&layout ) );

		WriteToBinary( binfile.c_str(), modtime, shader_buffers );

		for ( int i = 0; i < 5; ++i )
		{
			if ( shader_buffers[i] ) { shader_buffers[i]->Release(); shader_buffers[i] = NULL; }
		}
		printf("CreateEffectFromAscii: succeeded in creating effect %s.\n", filename.c_str() );
	}
	///////////////////////// read from binary //////////////////////////////////////////////////////////////
	else
	{
		LoadFromBinary( device, fin, num_elements, elements );
		printf("CreateEffectFromBinary: succeeded in creating effect %s.\n", filename.c_str() );
	}
}
//=======================================================================
void Effect::LoadFromAscii( ID3D11Device* device, const char* path, ID3D10Blob* shader_buffers[5] )
{
	for ( int i = 0; i < 5; ++i )
		Effect::DXCompile( path, entrypoints[i].c_str(), sm_level[i], &shader_buffers[i] );

	if ( shader_buffers[0] ) device->CreateVertexShader( shader_buffers[0]->GetBufferPointer(), shader_buffers[0]->GetBufferSize(), 0, &vs );
	if ( shader_buffers[1] ) device->CreatePixelShader( shader_buffers[1]->GetBufferPointer(), shader_buffers[1]->GetBufferSize(), 0, &ps );
	if ( shader_buffers[2] ) CreateGeometryShader( device, shader_buffers[2]->GetBufferPointer(), shader_buffers[2]->GetBufferSize() );
	if ( shader_buffers[3] ) device->CreateHullShader( shader_buffers[3]->GetBufferPointer(), shader_buffers[3]->GetBufferSize(), 0, &hs );
	if ( shader_buffers[4] ) device->CreateDomainShader( shader_buffers[4]->GetBufferPointer(), shader_buffers[4]->GetBufferSize(), 0, &ds );
}
//=======================================================================
void Effect::LoadFromBinary( ID3D11Device* device, std::ifstream& fin, int num_elements, D3D11_INPUT_ELEMENT_DESC* elements )
{
	HRESULT hr;
	int mark;
	while( fin.read( (char*)&mark, sizeof( int ) ) )
	{
		unsigned int size;
		void* data;

		fin.read( (char*)&size, sizeof(unsigned int) );
		data = new BYTE[size];
		fin.read( (char*)data, size );
		switch( mark )
		{
		case VS:
			DXCall( device->CreateVertexShader( data, size, 0, &vs ) );
			DXCall( device->CreateInputLayout( elements, num_elements, data, size, &layout ) );
			break;
		case PS:
			DXCall( device->CreatePixelShader( data, size, 0, &ps ) );
			break;
		case GS:
			DXCall( CreateGeometryShader( device, data, size ) );
			break;
		case HS:
			DXCall( device->CreateHullShader( data, size, 0, &hs ) );
			break;
		case DS:
			DXCall( device->CreateDomainShader( data, size, 0, &ds ) );
			break;
		}
		delete [] data;
	}
}
//=======================================================================
void Effect::ReloadFromAscii( ID3D11Device* device, const char* path )
{
	ID3D10Blob* shader_buffers[5] = {NULL};
	for ( int i = 0; i < 5; ++i )
		Effect::DXCompile( path, entrypoints[i].c_str(), sm_level[i], &shader_buffers[i] );

	if ( shader_buffers[0] )
	{
		if ( vs ) { vs->Release(); vs = NULL; }
		if ( ps ) { ps->Release(); ps = NULL; }
		if ( gs ) { gs->Release(); gs = NULL; }
		if ( hs ) { hs->Release(); hs = NULL; }
		if ( ds ) { ds->Release(); ds = NULL; }
		if ( shader_buffers[0] ) device->CreateVertexShader( shader_buffers[0]->GetBufferPointer(), shader_buffers[0]->GetBufferSize(), 0, &vs );
		if ( shader_buffers[1] ) device->CreatePixelShader( shader_buffers[1]->GetBufferPointer(), shader_buffers[1]->GetBufferSize(), 0, &ps );
		if ( shader_buffers[2] ) CreateGeometryShader( device, shader_buffers[2]->GetBufferPointer(), shader_buffers[2]->GetBufferSize() );
		if ( shader_buffers[3] ) device->CreateHullShader( shader_buffers[3]->GetBufferPointer(), shader_buffers[3]->GetBufferSize(), 0, &hs );
		if ( shader_buffers[4] ) device->CreateDomainShader( shader_buffers[4]->GetBufferPointer(), shader_buffers[4]->GetBufferSize(), 0, &ds );
	
		WIN32_FILE_ATTRIBUTE_DATA filedata;
		SYSTEMTIME modtime;
		GetFileAttributesEx( path, GetFileExInfoStandard, &filedata );
		FileTimeToSystemTime( &filedata.ftLastWriteTime, &modtime );

		WriteToBinary( binfile.c_str(), modtime, shader_buffers );
		
		printf( "ReloadEffect: succeeded with %s\n", binfile.c_str() );
	}
	else
		printf( "ReloadEffect: need at least a correct vertex buffer.\n" );

	for ( int i = 0; i < 5; ++i )
	{
		if ( shader_buffers[i] ) { shader_buffers[i]->Release(); shader_buffers[i] = NULL; }
	}
}
//=======================================================================
HRESULT Effect::CreateGeometryShader( ID3D11Device* device, void* data, unsigned int size )
{
	if ( so )
	{
		return device->CreateGeometryShaderWithStreamOutput( data,
														  size,
														  soelements,
														  num_elements,
														  &sostride,
														  1,
														  D3D11_SO_NO_RASTERIZED_STREAM,
														  NULL,
														  &gs );
	}
	else return device->CreateGeometryShader( data, size, NULL, &gs );
}
//=======================================================================
void Effect::Apply( ID3D11DeviceContext* context )
{
	context->IASetPrimitiveTopology( topology );
	context->IASetInputLayout( layout );
	context->VSSetShader( vs, NULL, 0 );
	context->HSSetShader( hs, NULL, 0 );
	context->DSSetShader( ds, NULL, 0 );
	context->GSSetShader( gs, NULL, 0 );
	context->PSSetShader( ps, NULL, 0 );
}
//=======================================================================
void Effect::ParseLayoutFormat( const char* str )
{
	int i = 0, j, repeat;
	int element_count[3] = {0, 0, 0};
	int type, num_values;
	bool per_instance;
	int l = strlen(str);
	//if ( l % 3 != 0 ) return -1;

	num_elements = 0;
	for ( int el = 0; el < l; el += 3 )
	{
		if ( str[el] == 'm' ) num_elements += 4;
		else ++num_elements;
	}
	elements = new D3D11_INPUT_ELEMENT_DESC [ num_elements ];
	D3D11_INPUT_ELEMENT_DESC* desc = elements;
	D3D11_SO_DECLARATION_ENTRY* sodesc = NULL;
	if ( so )
	{
		soelements = new D3D11_SO_DECLARATION_ENTRY[ num_elements ];
		sodesc = soelements;
		sostride = 0;
	}

	while ( i < l )
	{
		if ( str[i] == 'f' ) type = 0;
		else if ( str[i] == 'i' ) type = 1;
		else if ( str[i] == 'm' ) type = 2;

		++i;
		num_values = str[i] - '0';

		++i;
		if ( str[i] == 'I' ) per_instance = true;
		else per_instance = false;

		++i;

		if ( type == 2 ) repeat = 4; else repeat = 1;
		for ( j = 0; j < repeat; ++j )
		{
			desc->SemanticName = semantics[type];
			desc->SemanticIndex = element_count[type]; ++element_count[type];
			desc->Format = vertex_element_formats[num_values-1][type];
			desc->AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			desc->InputSlot = per_instance ? 1 : 0;
			desc->InputSlotClass = per_instance ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
			desc->InstanceDataStepRate = per_instance ? 1 : 0;
			
			if ( so )
			{
				sodesc->Stream = 0;
				sodesc->SemanticName = semantics[type];
				sodesc->SemanticIndex = desc->SemanticIndex;
				sodesc->StartComponent = 0;
				sodesc->ComponentCount = num_values;
				sodesc->OutputSlot = 0;
				++sodesc;
				sostride += num_values * sizeof(float);
			}

			++desc;
		}
	}
}
//=======================================================================
void Effect::WriteToBinary( const char* path, SYSTEMTIME modtime, ID3D10Blob** buffers )
{
	std::ofstream fout;
	fout.open( path, std::ios::out | std::ios::binary );

	fout.write( (const char*)&modtime, sizeof(modtime) );

	int mark = VS;
	for ( int i = 0; i < 5; ++i )
	{
		if ( buffers[i] )
		{
			unsigned int size = buffers[i]->GetBufferSize();
			fout.write( (const char*)&mark, sizeof(int) );
			fout.write( (const char*)&size, sizeof(unsigned int) );
			fout.write( static_cast<const char*>(buffers[i]->GetBufferPointer()), buffers[i]->GetBufferSize() );
		}
		++mark;
	}

	fout.close();
}

}