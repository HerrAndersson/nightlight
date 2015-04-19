#include "RenderModule.h"


RenderModule::RenderModule(HWND hwnd)
{
	m_vertexShader = NULL;
	m_pixelShader = NULL;
	m_sampleStateWrap = NULL;
	m_sampleStateClamp = NULL;
	m_matrixBuffer = NULL;
	m_hwnd = hwnd;
}


RenderModule::~RenderModule()
{
}


bool RenderModule::Initialize(ID3D11Device* device)
{
	bool result;

		//initializing shader files
	result = InitializeShader(device, L"vertexShader.vs", L"pixelShader.ps");
	if (!result)
	{
		return false;
	}

	return true;
}

bool RenderModule::Render(ID3D11DeviceContext* deviceContext, 
	XMMATRIX& worldMatrix,
	XMMATRIX& viewMatrix,
	XMMATRIX& projectionMatrix)
{
	bool result;

	//Set shader parameters, preparing them for render.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	//Now render the prepared buffers with the shader.
	RenderShader(deviceContext);

	return true;
}

bool RenderModule::InitializeShader(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	//vertex shader
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "vertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		//if shader failed to compile
		if (errorMessage)
		{
			OutputDebugString("Vertexshader failed to compile");
		}
		// if it couldn't find the shaderfile
		else
		{
			OutputDebugString("Vertexshader not found");;
		}

		return false;
	}
	if (SUCCEEDED(result))
		OutputDebugString("Vertexshader created");

	//pixel shader
	result = D3DCompileFromFile(psFilename, NULL, NULL, "pixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		//if shader failed to compile
		
		if (errorMessage)
		{
			OutputDebugString("Pixelshader failed to compile");
		}
		
		//if it couldn't find the shaderfile
		else
		{
			OutputDebugString("Pixelshader not found");;
		}
	
		if (SUCCEEDED(result))
			OutputDebugString("Pixelshader created");

		return false;
	}

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}


	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	//INPUT LAYOUT:
	// Create the layout description for input into the vertex shader.
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

	//count of elements in the layout
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result)){ return false; }

	//we no longer need the shader buffers, so release them
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	
	//Create a WRAP texture sampler state description
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//Create the texture sampler state
	result = device->CreateSamplerState(&samplerDesc, &m_sampleStateWrap);
	if (FAILED(result))
	{
		return false;
	}

	//Create a CLAMP texture sampler state description.
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	//Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleStateClamp);
	if (FAILED(result))
	{
		return false;
	}

	//CONSTANT BUFFER DESCRIPTIONS:
	//this is the dynamic matrix constant buffer that is in the VERTEX SHADER
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(matrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	//create a pointer to constant buffer, so we can acess the vertex shader constant buffer within this class
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result)) { return false; }

	

	return true;

}

void RenderModule::ShutdownShader()
{
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}


	if (m_sampleStateClamp)
	{
		m_sampleStateClamp->Release();
		m_sampleStateClamp = 0;
	}

	if (m_sampleStateWrap)
	{
		m_sampleStateWrap->Release();
		m_sampleStateWrap = 0;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	
	return;
}

bool RenderModule::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX& worldMatrix, XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNr;
	matrixBuffer* dataPtr;
	

	XMMATRIX worldMatrixC, viewMatrixC, projectionMatrixC;

	//transposing the matrices
	worldMatrixC = XMMatrixTranspose(worldMatrix);
	viewMatrixC = XMMatrixTranspose(viewMatrix);
	projectionMatrixC = XMMatrixTranspose(projectionMatrix);
	
	//lock the constant buffer for writing
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }

	//find pointer to data in the matrix constant buffer
	dataPtr = (matrixBuffer*)mappedResource.pData;

	//fill in the matrix buffer with the information from the transposed WVP etc matrices
	dataPtr->world = worldMatrixC;
	dataPtr->view = viewMatrixC;
	dataPtr->projection = projectionMatrixC;
	
	//unlock the constant buffer again
	deviceContext->Unmap(m_matrixBuffer, 0);

	//set the position of the constant buffer in the vertexshader
	bufferNr = 0;

	//setting matrix constant buffer in the VS with its new and updated values
	deviceContext->VSSetConstantBuffers(bufferNr, 1, &m_matrixBuffer);

	
	//setting the sent in shader texture resource in the pixel shader //No textures for the moment
	//deviceContext->PSSetShaderResources(0, 3, texture);

	
	return true;

}

void RenderModule::RenderShader(ID3D11DeviceContext* deviceContext)
{
	deviceContext->IASetInputLayout(m_layout);

	//set which vertex pixel geometry shaders that will be used for this triangle

	deviceContext->VSSetShader(m_vertexShader, NULL, 0);

	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//set the clamp and wrap sampler states and which register they belong to
	deviceContext->PSSetSamplers(0, 1, &m_sampleStateClamp);
	deviceContext->PSSetSamplers(1, 1, &m_sampleStateWrap);

	//and render!
	deviceContext->DrawIndexed(NULL, 0, 0);

	return;
}