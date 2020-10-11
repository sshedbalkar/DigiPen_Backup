#pragma once

namespace Framework
{
#define MAX_BIND_VERTEX_BUFFERS			4
#define MAX_BIND_SHADER_RESOURCES		8
#define MAX_BIND_CONSTANT_BUFFERS		6
#define MAX_BIND_SAMPLERS				6

class Effect;
class DXObject;

class Renderer
{
	/////////////////////////////////////////////////////////////// effect
	Effect* effect;
	/////////////////////////////////////////////////////////////// vertex buffers
	ID3D11Buffer* vertex_buffers[MAX_BIND_VERTEX_BUFFERS];
	unsigned int strides[MAX_BIND_VERTEX_BUFFERS];
	unsigned int offsets[MAX_BIND_VERTEX_BUFFERS];
	unsigned int num_vertex_buffers;
	/////////////////////////////////////////////////////////////// index buffer
	ID3D11Buffer* index_buffer;
	/////////////////////////////////////////////////////////////// constant buffers
	ID3D11Buffer* vs_constants[MAX_BIND_CONSTANT_BUFFERS];
	unsigned int num_vs_constants;
	ID3D11Buffer* ps_constants[MAX_BIND_CONSTANT_BUFFERS];
	unsigned int num_ps_constants;
	ID3D11Buffer* gs_constants[MAX_BIND_CONSTANT_BUFFERS];
	unsigned int num_gs_constants;
	ID3D11Buffer* hs_constants[MAX_BIND_CONSTANT_BUFFERS];
	unsigned int num_hs_constants;
	ID3D11Buffer* ds_constants[MAX_BIND_CONSTANT_BUFFERS];
	unsigned int num_ds_constants;
	////////////////////////////////////////////////////////////// shader resources
	DXObject* vs_srvs[MAX_BIND_SHADER_RESOURCES];
	unsigned int num_vs_srvs;
	DXObject* ps_srvs[MAX_BIND_SHADER_RESOURCES];
	unsigned int num_ps_srvs;
	DXObject* gs_srvs[MAX_BIND_SHADER_RESOURCES];
	unsigned int num_gs_srvs;
	DXObject* hs_srvs[MAX_BIND_SHADER_RESOURCES];
	unsigned int num_hs_srvs;
	DXObject* ds_srvs[MAX_BIND_SHADER_RESOURCES];
	unsigned int num_ds_srvs;
	////////////////////////////////////////////////////////////// sampler states
	ID3D11SamplerState* vs_samplers[MAX_BIND_SAMPLERS];
	unsigned int num_vs_samplers;
	ID3D11SamplerState* ps_samplers[MAX_BIND_SAMPLERS];
	unsigned int num_ps_samplers;
	ID3D11SamplerState* gs_samplers[MAX_BIND_SAMPLERS];
	unsigned int num_gs_samplers;
	ID3D11SamplerState* hs_samplers[MAX_BIND_SAMPLERS];
	unsigned int num_hs_samplers;
	ID3D11SamplerState* ds_samplers[MAX_BIND_SAMPLERS];
	unsigned int num_ds_samplers;
	////////////////////////////////////////////////////////////// raster state
	ID3D11RasterizerState* raster_state;
	ID3D11RasterizerState* raster_state_prev;
	////////////////////////////////////////////////////////////// blend state
	ID3D11BlendState* blend_state;
	ID3D11BlendState* blend_state_prev;
	float blend_factor[4];
	float blend_factor_prev[4];
	unsigned int sample_mask_prev;
	////////////////////////////////////////////////////////////// depth stencil state
	ID3D11DepthStencilState* depth_stencil_state;
	ID3D11DepthStencilState* depth_stencil_state_prev;
	unsigned int stencil_ref_prev;
public:
	Renderer();
	void Release();

	void Apply( ID3D11DeviceContext* context );
	void Restore( ID3D11DeviceContext* context );

	void SetEffect( Effect* effect );
	void SetVertexBuffer( ID3D11Buffer* vbuffer, unsigned int stride, unsigned int offset );
	void SetVertexBuffer( ID3D11Buffer* vbuffer, unsigned int stride, unsigned int offset, unsigned int slot );
	void SetIndexBuffer( ID3D11Buffer* ibuffer );
	void SetConstantBuffer( const char* stage, ID3D11Buffer* cbuffer );
	void SetShaderResource( const char* stage, DXObject* srv );
	void SetShaderResource( const char* stage, DXObject* srv, unsigned int slot );
	void SetSamplerState( const char* stage, ID3D11SamplerState* sstate );
	void SetRasterState( ID3D11RasterizerState* rstate );
	void SetBlendState( ID3D11BlendState* bstate );
	void SetDepthStencilState( ID3D11DepthStencilState* dsstate );

	void ClearVertexBuffer();
	void ClearConstantBuffer( const char* stage );
	void ClearShaderResource( const char* stage );
	void ClearSamplerState( const char* stage );

private:
	~Renderer();
	void DXObjects2SRVs( DXObject** srvobjects, unsigned int num_srvs, ID3D11ShaderResourceView** srvs );
};

}