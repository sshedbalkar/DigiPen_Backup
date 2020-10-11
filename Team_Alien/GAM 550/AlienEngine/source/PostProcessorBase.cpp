////////////////////////////////////////////////////////////////////////////////
// Filename: postprocessor.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Precompiled.h"
#include "postprocessorbase.h"
#include "ShaderCompilation.h"
//#include "d3d9.h"

namespace Framework
{

PostProcessorBase::PostProcessorBase() : device(NULL), context(NULL)
{
}

PostProcessorBase::~PostProcessorBase()
{
    ClearTempRenderTargetCache();
}

void PostProcessorBase::Initialize(ID3D11Device* device, HWND hwnd)
{
    this->device = device;

    // Create resources for the full-screen quad

    // Load the shaders
    ID3D10Blob* vsBuffer;
    quadVS=(CompileVSFromFile(device, L"Shaders//Quad.hlsl", "QuadVS", "vs_4_0", NULL, NULL, &vsBuffer));


    // Create the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }		
    };

	if (FAILED((device->CreateInputLayout(layout, 2, vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), &quadInputLayout))))
		__debugbreak();

    // Create and initialize the vertex and index buffers
    QuadVertex verts[4] =
    {
        { XMFLOAT4(1, 1, 1, 1), XMFLOAT2(1, 0) },
        { XMFLOAT4(1, -1, 1, 1), XMFLOAT2(1, 1) },
        { XMFLOAT4(-1, -1, 1, 1), XMFLOAT2(0, 1) },
        { XMFLOAT4(-1, 1, 1, 1), XMFLOAT2(0, 0) }
    };

    D3D11_BUFFER_DESC desc;
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.ByteWidth = sizeof(verts);
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = verts;
    initData.SysMemPitch = 0;
    initData.SysMemSlicePitch = 0;
	if (FAILED((device->CreateBuffer(&desc, &initData, &quadVB))))
		__debugbreak();

    unsigned short indices[6] = { 0, 1, 2, 2, 3, 0 };

    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.ByteWidth = sizeof(indices);
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    desc.CPUAccessFlags = 0;
    initData.pSysMem = indices;
	if (FAILED((device->CreateBuffer(&desc, &initData, &quadIB))))
		__debugbreak();

    // Create the constant buffer
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.ByteWidth = CBSize(sizeof(PSConstants));
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	if (FAILED((device->CreateBuffer(&desc, NULL, &psConstants))))
		__debugbreak();

    // Create a depth-stencil state
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    dsDesc.DepthEnable = false;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    dsDesc.StencilEnable = false;
    dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    dsDesc.BackFace = dsDesc.FrontFace;
	if (FAILED((device->CreateDepthStencilState(&dsDesc, &dsState))))
		__debugbreak();

    // Create a blend state
    D3D11_BLEND_DESC blendDesc;
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.IndependentBlendEnable = false;
    for (UINT i = 0; i < 8; ++i)
    {
        blendDesc.RenderTarget[i].BlendEnable = false;
        blendDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[i].DestBlend = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        blendDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
    }
	if (FAILED((device->CreateBlendState(&blendDesc, &blendState))))
		__debugbreak();

    // Create a rasterizer state
    D3D11_RASTERIZER_DESC rastDesc;
    rastDesc.AntialiasedLineEnable = FALSE;
    rastDesc.CullMode = D3D11_CULL_NONE;
    rastDesc.DepthBias = 0;
    rastDesc.DepthBiasClamp = 0.0f;
    rastDesc.DepthClipEnable = TRUE;
    rastDesc.FillMode = D3D11_FILL_SOLID;
    rastDesc.FrontCounterClockwise = false;
    rastDesc.MultisampleEnable = true;
    rastDesc.ScissorEnable = false;
    rastDesc.SlopeScaledDepthBias = 0;
	if (FAILED((device->CreateRasterizerState(&rastDesc, &rastState))))
		__debugbreak();

    // Create sampler states
    D3D11_SAMPLER_DESC sampDesc;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.BorderColor[0] = 0;
    sampDesc.BorderColor[1] = 0;
    sampDesc.BorderColor[2] = 0;
    sampDesc.BorderColor[3] = 0;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.MaxAnisotropy = 1;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    sampDesc.MinLOD = 0;
    sampDesc.MipLODBias = 0;
	if (FAILED((device->CreateSamplerState(&sampDesc, &linearSamplerState))))
		__debugbreak();

    sampDesc.AddressU = sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	if (FAILED((device->CreateSamplerState(&sampDesc, &linearWrapSamplerState))))
		__debugbreak();

    sampDesc.AddressU = sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	if (FAILED((device->CreateSamplerState(&sampDesc, &linearBorderSamplerState))))
		__debugbreak();

    sampDesc.AddressU = sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	if (FAILED((device->CreateSamplerState(&sampDesc, &pointSamplerState))))
		__debugbreak();

    sampDesc.AddressU = sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	if (FAILED((device->CreateSamplerState(&sampDesc, &pointBorderSamplerState))))
		__debugbreak();
}

void PostProcessorBase::AfterReset(UINT width, UINT height)
{
    // Clear all of the temp render targets
    ClearTempRenderTargetCache();

    inputWidth = width;
    inputHeight = height;
}

void PostProcessorBase::Render(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* input, ID3D11RenderTargetView* output)
{
    context = deviceContext;

    // Set device states
    float blendFactor[4] = {1, 1, 1, 1};
    context->RSSetState(rastState);
    context->OMSetBlendState(blendState, blendFactor, 0xFFFFFFFF);
    context->OMSetDepthStencilState(dsState, 0);

    // Set the sampler states
    ID3D11SamplerState* samplerStates[5] = { pointSamplerState, linearSamplerState,
                                             linearWrapSamplerState, linearBorderSamplerState,
                                             pointBorderSamplerState };
    context->PSSetSamplers(0, 5, samplerStates);

    // Set the input layout
    context->IASetInputLayout(quadInputLayout);

    // Set the vertex buffer
    UINT stride = sizeof(QuadVertex);
    UINT offset = 0;
    ID3D11Buffer* vertexBuffers[1] = { quadVB };
    context->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);

    // Set the index buffer
    context->IASetIndexBuffer(quadIB, DXGI_FORMAT_R16_UINT, 0);

    // Set primitive topology
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Set the shaders
    context->VSSetShader(quadVS, NULL, 0);
    context->DSSetShader(NULL, NULL, 0);
    context->HSSetShader(NULL, NULL, 0);
    context->GSSetShader(NULL, NULL, 0);
	context->CSSetShader(NULL, NULL, 0);
}

void PostProcessorBase::PostProcess(ID3D11ShaderResourceView* input, ID3D11RenderTargetView* output, ID3D11PixelShader* pixelShader, const WCHAR* name)
{
    inputs.push_back(input);
    outputs.push_back(output);
    PostProcess(pixelShader, name);
}

void PostProcessorBase::PostProcess(ID3D11PixelShader* pixelShader, const WCHAR* name)
{
    _ASSERT(context);

    _ASSERT(inputs.size() <= MaxInputs);

   // D3DPERF_BeginEvent(0xFFFFFFFF, name);

    // Set the outputs
    ID3D11RenderTargetView** renderTargets = reinterpret_cast<ID3D11RenderTargetView**>(&outputs[0]);
    UINT numRTs = static_cast<UINT>(outputs.size());
    if(uaViews.size() == 0)
        context->OMSetRenderTargets(numRTs, renderTargets, NULL);
    else
    {
        ID3D11UnorderedAccessView** uavs = reinterpret_cast<ID3D11UnorderedAccessView**>(&uaViews[0]);
        UINT numUAVs = static_cast<UINT>(uaViews.size());
        UINT initialCounts[D3D11_PS_CS_UAV_REGISTER_COUNT] = { 0 };
        context->OMSetRenderTargetsAndUnorderedAccessViews(numRTs, renderTargets, NULL, numRTs, numUAVs, uavs, initialCounts);
    }

    // Set the input textures
    ID3D11ShaderResourceView** textures = reinterpret_cast<ID3D11ShaderResourceView**>(&inputs[0]);
    context->PSSetShaderResources(0, static_cast<UINT>(inputs.size()), textures);

    // Set the constants
    D3D11_MAPPED_SUBRESOURCE mapped;
    if ((FAILED(context->Map(psConstants, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))))
		__debugbreak();

    PSConstants* constants = reinterpret_cast<PSConstants*>(mapped.pData);

    for (size_t i = 0; i < inputs.size(); ++i)
    {
        if(inputs[i] == NULL)
        {
            constants->InputSize[i].x = 0.0f;
            constants->InputSize[i].y = 0.0f;
            continue;
        }

        ID3D11Resource* resource;
        ID3D11Texture2D* texture;
        D3D11_TEXTURE2D_DESC desc;
        D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
        inputs[i]->GetDesc(&srDesc);
        UINT mipLevel = srDesc.Texture2D.MostDetailedMip;
        inputs[i]->GetResource(&resource);
        texture=(reinterpret_cast<ID3D11Texture2D*>(resource));
        texture->GetDesc(&desc);
        constants->InputSize[i].x = (max(float(desc.Width / (1 << mipLevel)), 1.0f));
        constants->InputSize[i].y = (max(float(desc.Height / (1 << mipLevel)), 1.0f));
    }

    ID3D11Resource* resource;
    ID3D11Texture2D* texture;
    D3D11_TEXTURE2D_DESC desc;
    D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
    outputs[0]->GetResource(&resource);
    outputs[0]->GetDesc(&rtDesc);
    UINT mipLevel = rtDesc.Texture2D.MipSlice;
    texture=(reinterpret_cast<ID3D11Texture2D*>(resource));
    texture->GetDesc(&desc);
    constants->OutputSize.x = (max(float(desc.Width / (1 << mipLevel)), 1.0f));
    constants->OutputSize.y = (max(float(desc.Height / (1 << mipLevel)), 1.0f));

    context->Unmap(psConstants, 0);

    ID3D11Buffer* constantBuffers[1] = { psConstants };
    context->PSSetConstantBuffers(0, 1, constantBuffers);

    // Set the viewports
    D3D11_VIEWPORT viewports[16];
    for (UINT_PTR i = 0; i < 16; ++i)
    {
        viewports[i].Width = (max(float(desc.Width / (1 << mipLevel)), 1.0f));
        viewports[i].Height = (max(float(desc.Height / (1 << mipLevel)), 1.0f));
        viewports[i].TopLeftX = 0;
        viewports[i].TopLeftY = 0;
        viewports[i].MinDepth = 0.0f;
        viewports[i].MaxDepth = 1.0f;
    }
    context->RSSetViewports(static_cast<UINT>(outputs.size()), viewports);

    // Set the pixel shader
    context->PSSetShader(pixelShader, NULL, 0);
	context->DSSetShader(NULL, NULL, 0);
    context->HSSetShader(NULL, NULL, 0);
    context->GSSetShader(NULL, NULL, 0);
	context->CSSetShader(NULL, NULL, 0);

    // Draw the quad
    context->DrawIndexed(6, 0, 0);

    // Clear the SRV's and RT's
    ID3D11ShaderResourceView* srViews[16] = { NULL };
    context->PSSetShaderResources(0, static_cast<UINT>(inputs.size()), srViews);

    ID3D11RenderTargetView* rtViews[16] = { NULL };
    context->OMSetRenderTargets(static_cast<UINT>(outputs.size() + uaViews.size()), rtViews, NULL);

    inputs.clear();
    outputs.clear();
    uaViews.clear();

    texture = NULL;
    //D3DPERF_EndEvent();
}

PostProcessorBase::TempRenderTarget* PostProcessorBase::GetTempRenderTarget(UINT width, UINT height, DXGI_FORMAT format,
                                                                            UINT msCount, UINT msQuality, UINT mipLevels,
                                                                            BOOL generateMipMaps, BOOL useAsUAV)
{
    // Look through existing render targets
    for (size_t i = 0; i < tempRenderTargets.size(); ++i)
    {
        TempRenderTarget* rt = tempRenderTargets[i];
        if (!rt->InUse && rt->Width == width && rt->Height == height && rt->Format == format
                && rt->MSCount == msCount && rt->MSQuality == msQuality && (rt->UAView != NULL) == useAsUAV)
        {
            rt->InUse = true;
            return rt;
        }
    }

    // Didn't find one, have to make one
    TempRenderTarget* rt = new TempRenderTarget();
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = width;
    desc.Height = height;
    desc.ArraySize = 1;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET;
    if(useAsUAV)
        desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
    desc.CPUAccessFlags = 0;
    desc.Format = format;
    desc.MipLevels = mipLevels;
    desc.MiscFlags = generateMipMaps ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
    desc.SampleDesc.Count = msCount;
    desc.SampleDesc.Quality = msQuality;
    desc.Usage = D3D11_USAGE_DEFAULT;
	if (FAILED((device->CreateTexture2D(&desc, NULL, &rt->Texture))))
		__debugbreak();

	if (FAILED((device->CreateRenderTargetView(rt->Texture, NULL, &rt->RTView))))
		__debugbreak();

	if (FAILED((device->CreateShaderResourceView(rt->Texture, NULL, &rt->SRView))))
		__debugbreak();


    if(useAsUAV)
        device->CreateUnorderedAccessView(rt->Texture, NULL, &rt->UAView);
    else
        rt->UAView = NULL;

    rt->Width = width;
    rt->Height = height;
    rt->MSCount = msCount;
    rt->MSQuality = msQuality;
    rt->Format = format;
    rt->InUse = true;
    tempRenderTargets.push_back(rt);

    return tempRenderTargets[tempRenderTargets.size() - 1];
}

void PostProcessorBase::ClearTempRenderTargetCache()
{
    for (size_t i = 0; i < tempRenderTargets.size(); ++i)
    {
        tempRenderTargets[i]->SRView->Release();
        tempRenderTargets[i]->RTView->Release();
        tempRenderTargets[i]->Texture->Release();
        if(tempRenderTargets[i]->UAView)
            tempRenderTargets[i]->UAView->Release();
        delete tempRenderTargets[i];
    }

    tempRenderTargets.erase(tempRenderTargets.begin(), tempRenderTargets.end());
}

}