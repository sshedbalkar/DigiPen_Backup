#pragma once

namespace Framework
{

class Effect
{
	enum ShaderType
	{
		VS = 0,
		PS = 1,
		GS = 2,
		HS = 3,
		DS = 4
	};
private:
	ID3D11VertexShader* vs;
	ID3D11HullShader* hs;
	ID3D11DomainShader* ds;
	ID3D11GeometryShader* gs;
	ID3D11PixelShader* ps;
	ID3D11InputLayout* layout;
	D3D11_PRIMITIVE_TOPOLOGY topology;
	std::string entrypoints[5];
	std::string binfile;
	D3D11_INPUT_ELEMENT_DESC* elements;
	unsigned int num_elements;

	bool so;
	D3D11_SO_DECLARATION_ENTRY* soelements;
	unsigned int sostride;

	void ParseLayoutFormat( const char* layout );
	void WriteToBinary( const char* filename, SYSTEMTIME modtime, ID3D10Blob** buffers );

	void LoadFromBinary( ID3D11Device* device, std::ifstream& fin, int num_elements, D3D11_INPUT_ELEMENT_DESC* elements );
	void LoadFromAscii( ID3D11Device* device, const char* path, ID3D10Blob* shader_buffers[5] );
	HRESULT CreateGeometryShader( ID3D11Device* device, void* data, unsigned int size );
public:
	Effect();
	void Release();
	void LoadEffect( ID3D11Device* device, const char* filename, const char* str_layout, int topology, const char* entrypts[5], bool _so );
	void Apply( ID3D11DeviceContext* context );
	void ReloadFromAscii( ID3D11Device* device, const char* path );
	
	static void DXCompile( const char* filename, const char* entrypoint, const char* smlevel, ID3D10Blob** buffer );
	static void AddMacro( const char* tag, const char* value );
	static void ApplyMacros();
	static std::map<std::string, std::string> macro_strings;
	static std::vector<D3D10_SHADER_MACRO> macros;

};

}