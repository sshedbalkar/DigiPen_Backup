#pragma once

namespace Framework
{


/////////////////////////////////////////////////////////////////////////////////////////
//  sampler states
/////////////////////////////////////////////////////////////////////////////////////////
class SamplerStates : public std::map< std::string, ID3D11SamplerState*>
{
	typedef std::pair<std::string, ID3D11SamplerState*> SamplerPair;
	D3D11_SAMPLER_DESC LinearWrapDesc()
	{
		D3D11_SAMPLER_DESC desc;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0.0f;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;
		return desc;
	}
	D3D11_SAMPLER_DESC LinearClampDesc()
	{
		D3D11_SAMPLER_DESC desc;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0.0f;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;
		return desc;
	}
	D3D11_SAMPLER_DESC PointWrapDesc()
	{
		D3D11_SAMPLER_DESC desc;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0.0f;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;
		return desc;
	}
	D3D11_SAMPLER_DESC PointClampDesc()
	{
		D3D11_SAMPLER_DESC desc;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0.0f;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;
		return desc;
	}
	D3D11_SAMPLER_DESC ShadowMapPCFDesc()
	{
		D3D11_SAMPLER_DESC desc;
		desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0.0f;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;
		return desc;
	}

public:
	~SamplerStates()
	{
		iterator it = this->begin();
		while ( it != this->end() )
		{
			S_RELEASE( it->second );
			++it;
		}
		this->clear();
	}
	void Initialize( ID3D11Device* device )
	{
		ID3D11SamplerState* sampler;
		HRESULT hr;
		DXCall( device->CreateSamplerState( &LinearWrapDesc(), &sampler ) );
		this->insert( SamplerPair( "LinearWrap", sampler ) );
		DXCall( device->CreateSamplerState( &LinearClampDesc(), &sampler ) );
		this->insert( SamplerPair( "LinearClamp", sampler ) );
		DXCall( device->CreateSamplerState( &PointWrapDesc(), &sampler ) );
		this->insert( SamplerPair( "PointWrap", sampler ) );
		DXCall( device->CreateSamplerState( &PointClampDesc(), &sampler ) );
		this->insert( SamplerPair( "PointClamp", sampler ) );
		DXCall( device->CreateSamplerState( &ShadowMapPCFDesc(), &sampler ) );
		this->insert( SamplerPair( "ShadowMapPCF", sampler ) );
	}
};
/////////////////////////////////////////////////////////////////////////////////////////
//  blend states
/////////////////////////////////////////////////////////////////////////////////////////
class BlendStates : public std::map<std::string, ID3D11BlendState*>
{
	typedef std::pair<std::string, ID3D11BlendState*> BlendPair;
	D3D11_BLEND_DESC BlendNoneDesc()
	{
		D3D11_BLEND_DESC desc;
		desc.AlphaToCoverageEnable = false;
		desc.IndependentBlendEnable = false;
		for (UINT i = 0; i < 8; ++i)
		{
			desc.RenderTarget[i].BlendEnable = false;
			desc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			desc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;
			desc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			desc.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			desc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
		}
		return desc;
	}
	D3D11_BLEND_DESC BlendAlphaDesc()
	{
		D3D11_BLEND_DESC desc;
		desc.AlphaToCoverageEnable = false;
		desc.IndependentBlendEnable = true;
		for (UINT i = 0; i < 8; ++i)
		{
			desc.RenderTarget[i].BlendEnable = true;
			desc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			desc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;
			desc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			desc.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			desc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
		}
		return desc;
	}
	D3D11_BLEND_DESC BlendAddDesc()
	{
		D3D11_BLEND_DESC desc;
		desc.AlphaToCoverageEnable = false;
		desc.IndependentBlendEnable = false;
		for (UINT i = 0; i < 8; ++i)
		{
			desc.RenderTarget[i].BlendEnable = true;
			desc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[i].DestBlend = D3D11_BLEND_ONE;
			desc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;
			desc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			desc.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
			desc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
		}
		return desc;
	}

public:
	~BlendStates()
	{
		iterator it = this->begin();
		while ( it != this->end() )
		{
			S_RELEASE( it->second );
			++it;
		}
		this->clear();
	}
	void Initialize( ID3D11Device* device )
	{
		ID3D11BlendState* blend;
		HRESULT hr;
		DXCall( device->CreateBlendState( &BlendNoneDesc(), &blend ) );
		this->insert( BlendPair( "BlendNone", blend ) );
		DXCall( device->CreateBlendState( &BlendAlphaDesc(), &blend ) );
		this->insert( BlendPair( "BlendAlpha", blend ) );
		DXCall( device->CreateBlendState( &BlendAddDesc(), &blend ) );
		this->insert( BlendPair( "BlendAdd", blend ) );

	}
};
/////////////////////////////////////////////////////////////////////////////////////////
//  raster states
/////////////////////////////////////////////////////////////////////////////////////////
class RasterStates : public std::map<std::string, ID3D11RasterizerState*>
{
	typedef std::pair<std::string, ID3D11RasterizerState*> RasterPair;
	D3D11_RASTERIZER_DESC CullNoneDesc()
	{
		D3D11_RASTERIZER_DESC desc;

		desc.AntialiasedLineEnable = FALSE;
		desc.CullMode = D3D11_CULL_NONE;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0.0f;
		desc.DepthClipEnable = TRUE;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.FrontCounterClockwise = false;
		desc.MultisampleEnable = true;
		desc.ScissorEnable = false;
		desc.SlopeScaledDepthBias = 0;
		return desc;
	}
	D3D11_RASTERIZER_DESC CullBackDesc()
	{
		D3D11_RASTERIZER_DESC desc;

		desc.AntialiasedLineEnable = FALSE;
		desc.CullMode = D3D11_CULL_BACK;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0.0f;
		desc.DepthClipEnable = TRUE;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.FrontCounterClockwise = false;
		desc.MultisampleEnable = true;
		desc.ScissorEnable = false;
		desc.SlopeScaledDepthBias = 0;
		return desc;
	}
	D3D11_RASTERIZER_DESC WireframeDesc()
	{
		D3D11_RASTERIZER_DESC desc;

		desc.AntialiasedLineEnable = FALSE;
		desc.CullMode = D3D11_CULL_NONE;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0.0f;
		desc.DepthClipEnable = TRUE;
		desc.FillMode = D3D11_FILL_WIREFRAME;
		desc.FrontCounterClockwise = false;
		desc.MultisampleEnable = true;
		desc.ScissorEnable = false;
		desc.SlopeScaledDepthBias = 0;
		return desc;
	}

public:
	~RasterStates()
	{
		iterator it = this->begin();
		while ( it != this->end() )
		{
			S_RELEASE( it->second );
			++it;
		}
		this->clear();
	}
	void Initialize( ID3D11Device* device )
	{
		ID3D11RasterizerState* raster;
		HRESULT hr;
		DXCall( device->CreateRasterizerState( &CullNoneDesc(), &raster ) );
		this->insert( RasterPair( "CullNone", raster ) );
		DXCall( device->CreateRasterizerState( &CullBackDesc(), &raster ) );
		this->insert( RasterPair( "CullBack", raster ) );
		DXCall( device->CreateRasterizerState( &WireframeDesc(), &raster ) );
		this->insert( RasterPair( "Wireframe", raster ) );

	}
};
/////////////////////////////////////////////////////////////////////////////////////////
//  depth stencil states
/////////////////////////////////////////////////////////////////////////////////////////
class DepthStencilStates : public std::map<std::string, ID3D11DepthStencilState*>
{
	typedef std::pair<std::string, ID3D11DepthStencilState*> DSPair;
	D3D11_DEPTH_STENCIL_DESC DepthEnableDesc()
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		desc.StencilEnable = false;
		desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		desc.BackFace = desc.FrontFace;

		return desc;
	}
	D3D11_DEPTH_STENCIL_DESC DepthDisableDesc()
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		desc.DepthEnable = false;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		desc.StencilEnable = false;
		desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		desc.BackFace = desc.FrontFace;

		return desc;
	}
	D3D11_DEPTH_STENCIL_DESC DepthReadOnlyDesc()
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		desc.StencilEnable = false;
		desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		desc.BackFace = desc.FrontFace;

		return desc;
	}

public:
	~DepthStencilStates()
	{
		iterator it = this->begin();
		while ( it != this->end() )
		{
			S_RELEASE( it->second );
			++it;
		}
		this->clear();
	}
	void Initialize( ID3D11Device* device )
	{
		ID3D11DepthStencilState* ds;
		HRESULT hr;
		DXCall( device->CreateDepthStencilState( &DepthEnableDesc(), &ds ) );
		this->insert( DSPair( "DepthEnable", ds ) );
		DXCall( device->CreateDepthStencilState( &DepthDisableDesc(), &ds ) );
		this->insert( DSPair( "DepthDisable", ds ) );
		DXCall( device->CreateDepthStencilState( &DepthReadOnlyDesc(), &ds ) );
		this->insert( DSPair( "DepthReadOnly", ds ) );

	}
};

}
