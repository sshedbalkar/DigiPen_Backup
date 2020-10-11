////////////////////////////////////////////////////////////////////////////////
// Filename: FXAA.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Precompiled.h"
#include "FXAA.h"
#include "Graphics.h"

namespace Framework {

FXAA::FXAA()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	FXAABuffer = 0;
	m_sampleStateAniso = 0;
}


FXAA::FXAA(const FXAA& other)
{
}


FXAA::~FXAA()
{
}


bool FXAA::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;


	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, "Shaders//FXAA.hlsl", "Shaders//FXAA.hlsl");
	if(!result)
	{
		return false;
	}

	return true;
}


void FXAA::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}


bool FXAA::Render(ID3D11DeviceContext* deviceContext, int indexCount, const D3DXMATRIX& worldMatrix, const D3DXMATRIX& viewMatrix, const
			      D3DXMATRIX& projectionMatrix, ID3D11ShaderResourceView** textureArray)
{
	bool result;


	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, textureArray);
	if(!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}


bool FXAA::InitializeShader(ID3D11Device* device, HWND hwnd, char* vsFilename, char* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
	unsigned int numElements;
	D3D11_BUFFER_DESC FXAABufferDesc;
    D3D11_SAMPLER_DESC samplerDesc;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

    // Compile the vertex shader code.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "FxaaVS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 
								   0, NULL, &vertexShaderBuffer, &errorMessage, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, vsFilename, "Missing Shader File", MB_OK);
		}

		return false;
	}

    // Compile the pixel shader code.
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "FxaaPS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 
								   0, NULL, &pixelShaderBuffer, &errorMessage, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// If there was  nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, psFilename, "Missing Shader File", MB_OK);
		}

		return false;
	}

    // Create the vertex shader from the buffer.
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, 
										&m_vertexShader);
	if(FAILED(result))
	{
		return false;
	}

    // Create the vertex shader from the buffer.
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, 
									   &m_pixelShader);
	if(FAILED(result))
	{
		return false;
	}

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClassTangent and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;
	
	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	polygonLayout[3].SemanticName = "TANGENT";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	polygonLayout[4].SemanticName = "BINORMAL";
	polygonLayout[4].SemanticIndex = 0;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[4].InputSlot = 0;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), 
									   vertexShaderBuffer->GetBufferSize(), &m_layout);
	if(FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	ZeroMemory( &samplerDesc, sizeof(samplerDesc) );
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = samplerDesc.BorderColor[2] = samplerDesc.BorderColor[3] = 0;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    samplerDesc.AddressU = samplerDesc.AddressV = samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.MaxAnisotropy = 4;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.MaxLOD = 0.0f;
	samplerDesc.MinLOD = 0.0f;
	// Create the texture sampler state.
    result = device->CreateSamplerState(&samplerDesc, &m_sampleStateAniso);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the scale dynamic constant buffer that is in the vertex shader.
	FXAABufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	FXAABufferDesc.ByteWidth = sizeof(CB_FXAA);
	FXAABufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	FXAABufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	FXAABufferDesc.MiscFlags = 0;
	FXAABufferDesc.StructureByteStride = 0;
	// Create the scale constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&FXAABufferDesc, NULL, &FXAABuffer);
	if(FAILED(result)){return false;}
	

	return true;
}


void FXAA::ShutdownShader()
{
	// Release the scale constant buffer.
	if(FXAABuffer)
	{
		FXAABuffer->Release();
		FXAABuffer = 0;
	}

	// Release the sampler states.
	if(m_sampleStateAniso)
	{
		m_sampleStateAniso->Release();
		m_sampleStateAniso = 0;
	}

	// Release the layout.
	if(m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if(m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if(m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}


void FXAA::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, char* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for(i=0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, "Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}


bool FXAA::SetShaderParameters(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** textureArray)
{
	HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int VSBufferNumber = 0;
	unsigned int PSBufferNumber = 0;
	CB_FXAA *dataPtr;

	
	// Lock the scale constant buffer so it can be written to.
	result = deviceContext->Map(FXAABuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result)){return false;}
	// Get a pointer to the data in the constant buffer.
	dataPtr = (CB_FXAA*)mappedResource.pData;
	// Copy the scale position into the constant buffer.
	dataPtr->m_FXAA = D3DXVECTOR4(1.0f/(GRAPHICS->screen_width), 1.0f/(GRAPHICS->screen_height), 0.0f, 0.0f);
	// Unlock the scale constant buffer.
	deviceContext->Unmap(FXAABuffer, 0);
	// Now set the scale constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(VSBufferNumber, 1, &FXAABuffer);
	deviceContext->PSSetConstantBuffers(PSBufferNumber, 1, &FXAABuffer);
	++VSBufferNumber;
	
	// Set shader texture array resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, textureArray);
	//deviceContext->PSSetShaderResources(2, MAX_NUM_LIGHTS, depthTextures);
	
	return true;
}


void FXAA::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

    // Set the vertex and pixel shaders that will be used to render this triangle.
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);
	deviceContext->DSSetShader(NULL, NULL, 0);
    deviceContext->HSSetShader(NULL, NULL, 0);
    deviceContext->GSSetShader(NULL, NULL, 0);
	deviceContext->CSSetShader(NULL, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleStateAniso);

	// Render the triangles.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
}