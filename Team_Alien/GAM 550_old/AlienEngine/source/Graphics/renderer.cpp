#include "Precompiled.h"
#include "renderer.h"
#include "effect.h"
#include "DXObject.h"

namespace Framework
{

//=============================================================================
Renderer::Renderer() :
		effect( NULL ),
		num_vertex_buffers( 0 ),
		index_buffer( NULL ),
		num_vs_constants( 0 ), num_ps_constants( 0 ), num_gs_constants( 0 ), num_hs_constants( 0 ), num_ds_constants( 0 ), 
		num_vs_srvs( 0 ), num_ps_srvs( 0 ), num_gs_srvs( 0 ), num_hs_srvs( 0 ), num_ds_srvs( 0 ), 
		num_vs_samplers( 0 ), num_ps_samplers( 0 ), num_gs_samplers( 0 ), num_hs_samplers( 0 ), num_ds_samplers( 0 ), 
		raster_state( NULL ), raster_state_prev( NULL ),
		blend_state( NULL ), blend_state_prev( NULL ),
		depth_stencil_state( NULL ), depth_stencil_state_prev( NULL )
{
	blend_factor[0] = blend_factor[1] = blend_factor[2] = blend_factor[3] = 0.0f;
}
//=============================================================================
Renderer::~Renderer() {}
//=============================================================================
void Renderer::Release()
{
	delete this;
}
//=============================================================================
void Renderer::Apply( ID3D11DeviceContext* context )
{
	//if ( raster_state ) context->RSGetState( &raster_state_prev );
	//if ( blend_state ) context->OMGetBlendState( &blend_state_prev, blend_factor_prev, &sample_mask_prev );
	//if ( depth_stencil_state ) context->OMGetDepthStencilState( &depth_stencil_state_prev, &stencil_ref_prev );

	context->IASetVertexBuffers( 0, num_vertex_buffers, vertex_buffers, strides, offsets );
	if ( index_buffer ) context->IASetIndexBuffer( index_buffer, DXGI_FORMAT_R32_UINT, 0 );
	
//	if ( raster_state ) context->RSSetState( raster_state );
	//if ( blend_state ) context->OMSetBlendState( blend_state, blend_factor, 0xffffffff );
	//if ( depth_stencil_state ) context->OMSetDepthStencilState( depth_stencil_state, 1 );

	if ( num_vs_samplers > 0 ) context->VSSetSamplers( 0, num_vs_samplers, vs_samplers );
	if ( num_ps_samplers > 0 ) context->PSSetSamplers( 0, num_ps_samplers, ps_samplers );
	if ( num_gs_samplers > 0 ) context->GSSetSamplers( 0, num_gs_samplers, gs_samplers );
	if ( num_hs_samplers > 0 ) context->HSSetSamplers( 0, num_hs_samplers, hs_samplers );
	if ( num_ds_samplers > 0 ) context->DSSetSamplers( 0, num_ds_samplers, ds_samplers );

	effect->Apply( context );

	if ( num_vs_constants > 0 ) context->VSSetConstantBuffers( 0, num_vs_constants, vs_constants );
	if ( num_ps_constants > 0 ) context->PSSetConstantBuffers( 0, num_ps_constants, ps_constants );
	if ( num_gs_constants > 0 ) context->GSSetConstantBuffers( 0, num_gs_constants, gs_constants );
	if ( num_hs_constants > 0 ) context->HSSetConstantBuffers( 0, num_hs_constants, hs_constants );
	if ( num_ds_constants > 0 ) context->DSSetConstantBuffers( 0, num_ds_constants, ds_constants );

	ID3D11ShaderResourceView* srvs[MAX_BIND_SHADER_RESOURCES];
	if ( num_vs_srvs > 0 ) {DXObjects2SRVs( vs_srvs, num_vs_srvs, srvs );context->VSSetShaderResources( 0, num_vs_srvs, srvs ); }
	if ( num_ps_srvs > 0 ) {DXObjects2SRVs( ps_srvs, num_ps_srvs, srvs );context->PSSetShaderResources( 0, num_ps_srvs, srvs ); }
	if ( num_gs_srvs > 0 ) {DXObjects2SRVs( gs_srvs, num_gs_srvs, srvs );context->GSSetShaderResources( 0, num_gs_srvs, srvs ); }
	if ( num_hs_srvs > 0 ) {DXObjects2SRVs( hs_srvs, num_hs_srvs, srvs );context->HSSetShaderResources( 0, num_hs_srvs, srvs ); }
	if ( num_ds_srvs > 0 ) {DXObjects2SRVs( ds_srvs, num_ds_srvs, srvs );context->DSSetShaderResources( 0, num_ds_srvs, srvs ); }

}
//=============================================================================
void Renderer::Restore( ID3D11DeviceContext* context )
{
	if ( raster_state_prev )
	{
		context->RSSetState( raster_state_prev );
		raster_state_prev->Release();
		raster_state_prev = NULL;
	}
	if ( blend_state_prev )
	{
		context->OMSetBlendState( blend_state_prev, blend_factor_prev, sample_mask_prev );
		blend_state_prev->Release();
		blend_state_prev = NULL;
	}
	if ( depth_stencil_state_prev )
	{
		context->OMSetDepthStencilState( depth_stencil_state_prev, stencil_ref_prev );
		depth_stencil_state_prev->Release();
		depth_stencil_state_prev = NULL;
	}
}
//=============================================================================
void Renderer::SetEffect( Effect* _effect )
{
	effect = _effect;
}
//=============================================================================
void Renderer::SetVertexBuffer( ID3D11Buffer* vbuffer, unsigned int stride, unsigned int offset )
{
	if ( num_vertex_buffers >= MAX_BIND_VERTEX_BUFFERS )
	{ printf("Renderer::SetVertexBuffer: too many vertex buffers.\n"); return; }
	vertex_buffers[num_vertex_buffers] = vbuffer;
	strides[num_vertex_buffers] = stride;
	offsets[num_vertex_buffers] = offset;
	++num_vertex_buffers;
}
//=============================================================================
void Renderer::SetVertexBuffer( ID3D11Buffer* vbuffer, unsigned int stride, unsigned int offset, unsigned int slot )
{
	if ( slot >= num_vertex_buffers )
	{ printf("Renderer::SetVertexBuffer: set to a non-existant slot.\n"); return; }
	vertex_buffers[slot] = vbuffer;
	strides[slot] = stride;
	offsets[slot] = offset;
}
//=============================================================================
void Renderer::SetIndexBuffer( ID3D11Buffer* ibuffer )
{
	index_buffer = ibuffer;
}
//=============================================================================
void Renderer::SetConstantBuffer( const char* stage, ID3D11Buffer* cbuffer )
{
	switch( stage[0] )
	{
	case 'V':
		if ( num_vs_constants >= MAX_BIND_CONSTANT_BUFFERS )
		{ printf("Renderer::SetConstantBuffer: too many constant buffers.\n"); return; }
		vs_constants[num_vs_constants] = cbuffer;
		++num_vs_constants;
		break;
	case 'P':
		if ( num_ps_constants >= MAX_BIND_CONSTANT_BUFFERS )
		{ printf("Renderer::SetConstantBuffer: too many constant buffers.\n"); return; }
		ps_constants[num_ps_constants] = cbuffer;
		++num_ps_constants;
		break;
	case 'G':
		if ( num_gs_constants >= MAX_BIND_CONSTANT_BUFFERS )
		{ printf("Renderer::SetConstantBuffer: too many constant buffers.\n"); return; }
		gs_constants[num_gs_constants] = cbuffer;
		++num_gs_constants;
		break;
	case 'H':
		if ( num_hs_constants >= MAX_BIND_CONSTANT_BUFFERS )
		{ printf("Renderer::SetConstantBuffer: too many constant buffers.\n"); return; }
		hs_constants[num_hs_constants] = cbuffer;
		++num_hs_constants;
		break;
	case 'D':
		if ( num_ds_constants >= MAX_BIND_CONSTANT_BUFFERS )
		{ printf("Renderer::SetConstantBuffer: too many constant buffers.\n"); return; }
		ds_constants[num_ds_constants] = cbuffer;
		++num_ds_constants;
		break;
	}
}
//=============================================================================
void Renderer::SetShaderResource( const char* stage, DXObject* srvobject )
{
	switch ( stage[0] )
	{
	case 'V':
		if ( num_vs_srvs >= MAX_BIND_SHADER_RESOURCES )
		{ printf("Renderer::SetShaderResource: too many srvs.\n"); return; }
		vs_srvs[num_vs_srvs] = srvobject;
		++num_vs_srvs;
		break;
	case 'P':
		if ( num_ps_srvs >= MAX_BIND_SHADER_RESOURCES )
		{ printf("Renderer::SetShaderResource: too many srvs.\n"); return; }
		ps_srvs[num_ps_srvs] = srvobject;
		++num_ps_srvs;
		break;
	case 'G':
		if ( num_gs_srvs >= MAX_BIND_SHADER_RESOURCES )
		{ printf("Renderer::SetShaderResource: too many srvs.\n"); return; }
		gs_srvs[num_gs_srvs] = srvobject;
		++num_gs_srvs;
		break;
	case 'H':
		if ( num_hs_srvs >= MAX_BIND_SHADER_RESOURCES )
		{ printf("Renderer::SetShaderResource: too many srvs.\n"); return; }
		hs_srvs[num_hs_srvs] = srvobject;
		++num_hs_srvs;
		break;
	case 'D':
		if ( num_ds_srvs >= MAX_BIND_SHADER_RESOURCES )
		{ printf("Renderer::SetShaderResource: too many srvs.\n"); return; }
		ds_srvs[num_ds_srvs] = srvobject;
		++num_ds_srvs;
		break;
	}
}
//=============================================================================
void Renderer::SetShaderResource( const char* stage, DXObject* srvobject, unsigned int slot )
{
	switch ( stage[0] )
	{
	case 'V':
		if ( slot >= num_vs_srvs )
		{ printf("Renderer::SetShaderResource: wrong slot.\n"); return; }
		vs_srvs[slot] = srvobject;
		break;
	case 'P':
		if ( slot >= num_ps_srvs )
		{ printf("Renderer::SetShaderResource: wrong slot.\n"); return; }
		ps_srvs[slot] = srvobject;
		break;
	case 'G':
		if ( slot >= num_gs_srvs )
		{ printf("Renderer::SetShaderResource: wrong slot.\n"); return; }
		gs_srvs[slot] = srvobject;
		break;
	case 'H':
		if ( slot >= num_hs_srvs )
		{ printf("Renderer::SetShaderResource: wrong slot.\n"); return; }
		hs_srvs[slot] = srvobject;
		break;
	case 'D':
		if ( slot >= num_ds_srvs )
		{ printf("Renderer::SetShaderResource: wrong slot.\n"); return; }
		ds_srvs[slot] = srvobject;
		break;
	}
}
//=============================================================================
void Renderer::SetSamplerState( const char* stage, ID3D11SamplerState* sstate )
{
	switch ( stage[0] )
	{
	case 'V':
		if ( num_vs_samplers >= MAX_BIND_SAMPLERS )
		{ printf("Renderer::SetSamplerState: too many samplers.\n"); return; }
		vs_samplers[num_vs_samplers] = sstate;
		++num_vs_samplers;
		break;
	case 'P':
		if ( num_ps_samplers >= MAX_BIND_SAMPLERS )
		{ printf("Renderer::SetSamplerState: too many samplers.\n"); return; }
		ps_samplers[num_ps_samplers] = sstate;
		++num_ps_samplers;
		break;
	case 'G':
		if ( num_gs_samplers >= MAX_BIND_SAMPLERS )
		{ printf("Renderer::SetSamplerState: too many samplers.\n"); return; }
		gs_samplers[num_gs_samplers] = sstate;
		++num_gs_samplers;
		break;
	case 'H':
		if ( num_hs_samplers >= MAX_BIND_SAMPLERS )
		{ printf("Renderer::SetSamplerState: too many samplers.\n"); return; }
		hs_samplers[num_hs_samplers] = sstate;
		++num_hs_samplers;
		break;
	case 'D':
		if ( num_ds_samplers >= MAX_BIND_SAMPLERS )
		{ printf("Renderer::SetSamplerState: too many samplers.\n"); return; }
		ds_samplers[num_ds_samplers] = sstate;
		++num_ds_samplers;
		break;
	}
}
//=============================================================================
void Renderer::SetRasterState( ID3D11RasterizerState* rstate )
{
	raster_state = rstate;
}
//=============================================================================
void Renderer::SetBlendState( ID3D11BlendState* bstate )
{
	blend_state = bstate;
}
//=============================================================================
void Renderer::SetDepthStencilState( ID3D11DepthStencilState* dsstate )
{
	depth_stencil_state = dsstate;
}
//=============================================================================
void Renderer::ClearVertexBuffer()
{
	num_vertex_buffers = 0;
}
//=============================================================================
void Renderer::ClearConstantBuffer( const char* stage )
{
	switch ( stage[0] )
	{
	case 'V':
		num_vs_constants = 0;
		break;
	case 'P':
		num_ps_constants = 0;
		break;
	case 'G':
		num_gs_constants = 0;
		break;
	case 'H':
		num_hs_constants = 0;
		break;
	case 'D':
		num_ds_constants = 0;
		break;
	}
}
//=============================================================================
void Renderer::ClearShaderResource( const char* stage )
{
	switch ( stage[0] )
	{
	case 'V':
		num_vs_srvs = 0;
		break;
	case 'P':
		num_ps_srvs = 0;
		break;
	case 'G':
		num_gs_srvs = 0;
		break;
	case 'H':
		num_hs_srvs = 0;
		break;
	case 'D':
		num_ds_srvs = 0;
		break;
	}
}
//=============================================================================
void Renderer::ClearSamplerState( const char* stage )
{
	switch ( stage[0] )
	{
	case 'V':
		num_vs_samplers = 0;
		break;
	case 'P':
		num_ps_samplers = 0;
		break;
	case 'G':
		num_gs_samplers = 0;
		break;
	case 'H':
		num_hs_samplers = 0;
		break;
	case 'D':
		num_ds_samplers = 0;
		break;
	}
}
//=============================================================================
void Renderer::DXObjects2SRVs( DXObject** srvobjects, unsigned int num_srvs, ID3D11ShaderResourceView** srvs )
{
	for ( unsigned int i = 0; i < num_srvs; ++i )
	{
		srvs[i] = srvobjects[i]->Object<ID3D11ShaderResourceView>();
	}
}

}