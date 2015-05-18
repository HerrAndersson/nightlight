#include "RenderModule.h"
#include <fstream>
#include <string>
using namespace std;


RenderModule::RenderModule(HWND hwnd, int screenWidth, int screenHeight, bool fullscreen)
{
	vertexShader = NULL;
	pixelShader = NULL;
	skeletalVertexShader = NULL;
	blendVertexShader = NULL;
	sampleStateWrap = NULL;
	sampleStateClamp = NULL;
	matrixBufferPerObject = NULL;
	matrixBufferPerWeightedObject = NULL;
	matrixBufferPerBlendObject = NULL;
	matrixBufferPerFrame = NULL;
	lightBuffer = NULL;
	this->hwnd = hwnd;

	d3d = new D3DManager(hwnd, screenWidth, screenHeight, fullscreen);
	//initialize shadowmap
	shadowMap = new ShadowMap(d3d->GetDevice(), 512, L"depthVertexShader.hlsl");

	bool result;

	//initializing shader files
	result = InitializeShader(L"Assets/Shaders/vertexShader.hlsl", L"Assets/Shaders/pixelShader.hlsl");
	result = InitializeSkeletalShader(L"skeletalVertexShader.hlsl", L"Assets/Shaders/pixelShader.hlsl");
	result = InitializeBlendShader(L"blendVertexShader.hlsl", L"Assets/Shaders/pixelShader.hlsl");
}

RenderModule::~RenderModule()
{
	delete d3d;
	delete shadowMap;
	layoutPosUvNorm->Release();
	layoutPosUvNormIdxWei->Release();
	layoutPosUvNorm3PosNorm->Release();

	matrixBufferPerObject->Release();
	matrixBufferPerWeightedObject->Release();
	matrixBufferPerBlendObject->Release();

	matrixBufferPerFrame->Release();
	pixelShader->Release();
	sampleStateClamp->Release();
	sampleStateWrap->Release();
	vertexShader->Release();
	skeletalVertexShader->Release();
	blendVertexShader->Release();
}

bool RenderModule::InitializeShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc, lightBufferDesc;

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	ID3D11Device* device = d3d->GetDevice();

	/////////////////////////////////////////////////////////////////////////// Shaders ///////////////////////////////////////////////////////////////////////////

	//vertex shader
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "vertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
			throw runtime_error(string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
		else
			throw std::runtime_error("\nVertexshader not found");;

		return false;
	}
	if (SUCCEEDED(result))
		OutputDebugString("\nVertexshader created");

	//pixel shader
	result = D3DCompileFromFile(psFilename, NULL, NULL, "pixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
			throw runtime_error(string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
		else
			throw std::runtime_error("\nPixelshader not found");

		return false;
	}

	if (SUCCEEDED(result))
		OutputDebugString("\nPixelshader created");

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader);
	if (FAILED(result))
		return false;

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);
	if (FAILED(result))
		return false;

	/////////////////////////////////////////////////////////////////////// Input layout /////////////////////////////////////////////////////////////////////////
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

	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &layoutPosUvNorm);
	if (FAILED(result)){ return false; }

	//we no longer need the shader buffers, so release them
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	/////////////////////////////////////////////////////////////////////// Samplers /////////////////////////////////////////////////////////////////////////
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
	result = device->CreateSamplerState(&samplerDesc, &sampleStateWrap);
	if (FAILED(result))
	{
		return false;
	}

	//Create a CLAMP texture sampler state description.
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	//Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &sampleStateClamp);
	if (FAILED(result))
		return false;

	/////////////////////////////////////////////////////////////////////// Other /////////////////////////////////////////////////////////////////////////
	//CONSTANT BUFFER DESCRIPTIONS:
	//this is the dynamic matrix constant buffer that is in the VERTEX SHADER
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	//create a pointer to constant buffer, so we can acess the vertex shader constant buffer within this class
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferPerObject);
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBufferPerObject);

	if (FAILED(result))
		throw std::runtime_error("\nFailed to create matrix buffer");

	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferPerFrame);
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBufferPerFrame);

	if (FAILED(result))
		throw std::runtime_error("\nFailed to create matrix buffer");

	//this is the light dynamic constant buffer in the PIXEL SHADER
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	lightBufferDesc.ByteWidth = sizeof(LightBuffer);
	result = device->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);

	if (FAILED(result))
		throw std::runtime_error("\nFailed to create light buffer");
	
	return true;
}

bool RenderModule::InitializeSkeletalShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc, lightBufferDesc;

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	ID3D11Device* device = d3d->GetDevice();

	/////////////////////////////////////////////////////////////////////////// Shaders ///////////////////////////////////////////////////////////////////////////

	//vertex shader
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "skeletalVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
			throw runtime_error(string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
		else
			throw std::runtime_error("\nVertexshader not found");;

		return false;
	}
	if (SUCCEEDED(result))
		OutputDebugString("\nVertexshader created");

	//pixel shader
	result = D3DCompileFromFile(psFilename, NULL, NULL, "pixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
			throw runtime_error(string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
		else
			throw std::runtime_error("\nPixelshader not found");

		return false;
	}

	if (SUCCEEDED(result))
		OutputDebugString("\nPixelshader created");

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &skeletalVertexShader);
	if (FAILED(result))
		return false;

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);
	if (FAILED(result))
		return false;

	/////////////////////////////////////////////////////////////////////// Input layout /////////////////////////////////////////////////////////////////////////
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

	polygonLayout[3].SemanticName = "INDICES";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32A32_SINT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	polygonLayout[4].SemanticName = "WEIGHTS";
	polygonLayout[4].SemanticIndex = 0;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[4].InputSlot = 0;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;

	//count of elements in the layout
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &layoutPosUvNormIdxWei);
	if (FAILED(result)){ return false; }

	//we no longer need the shader buffers, so release them
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	/////////////////////////////////////////////////////////////////////// Samplers /////////////////////////////////////////////////////////////////////////
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
	result = device->CreateSamplerState(&samplerDesc, &sampleStateWrap);
	if (FAILED(result))
	{
		return false;
	}

	//Create a CLAMP texture sampler state description.
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	//Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &sampleStateClamp);
	if (FAILED(result))
		return false;

	/////////////////////////////////////////////////////////////////////// Other /////////////////////////////////////////////////////////////////////////
	//CONSTANT BUFFER DESCRIPTIONS:
	//this is the dynamic matrix constant buffer that is in the VERTEX SHADER
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	//create a pointer to constant buffer, so we can acess the vertex shader constant buffer within this class
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferPerWeightedObject);
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBufferPerWeightedObject);

	if (FAILED(result))
		throw std::runtime_error("\nFailed to create matrix buffer");

	//this is the light dynamic constant buffer in the PIXEL SHADER
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	lightBufferDesc.ByteWidth = sizeof(LightBuffer);
	result = device->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);

	if (FAILED(result))
		throw std::runtime_error("\nFailed to create light buffer");

	return true;

}

bool RenderModule::InitializeBlendShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[9];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc, lightBufferDesc;

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	ID3D11Device* device = d3d->GetDevice();

	/////////////////////////////////////////////////////////////////////////// Shaders ///////////////////////////////////////////////////////////////////////////

	//vertex shader
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "blendVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
			throw runtime_error(string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
		else
			throw std::runtime_error("\nBlendVertexshader not found");;

		return false;
	}
	if (SUCCEEDED(result))
		OutputDebugString("\nVertexshader created");

	//pixel shader
	result = D3DCompileFromFile(psFilename, NULL, NULL, "pixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
			throw runtime_error(string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
		else
			throw std::runtime_error("\nPixelshader not found");

		return false;
	}

	if (SUCCEEDED(result))
		OutputDebugString("\nPixelshader created");

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &blendVertexShader);
	if (FAILED(result))
		return false;

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);
	if (FAILED(result))
		return false;

	/////////////////////////////////////////////////////////////////////// Input layout /////////////////////////////////////////////////////////////////////////
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

	polygonLayout[3].SemanticName = "POSITION";
	polygonLayout[3].SemanticIndex = 1;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	polygonLayout[4].SemanticName = "POSITION";
	polygonLayout[4].SemanticIndex = 2;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[4].InputSlot = 0;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;

	polygonLayout[5].SemanticName = "POSITION";
	polygonLayout[5].SemanticIndex = 3;
	polygonLayout[5].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[5].InputSlot = 0;
	polygonLayout[5].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[5].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[5].InstanceDataStepRate = 0;

	polygonLayout[6].SemanticName = "NORMAL";
	polygonLayout[6].SemanticIndex = 1;
	polygonLayout[6].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[6].InputSlot = 0;
	polygonLayout[6].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[6].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[6].InstanceDataStepRate = 0;

	polygonLayout[7].SemanticName = "NORMAL";
	polygonLayout[7].SemanticIndex = 2;
	polygonLayout[7].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[7].InputSlot = 0;
	polygonLayout[7].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[7].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[7].InstanceDataStepRate = 0;

	polygonLayout[8].SemanticName = "NORMAL";
	polygonLayout[8].SemanticIndex = 3;
	polygonLayout[8].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[8].InputSlot = 0;
	polygonLayout[8].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[8].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[8].InstanceDataStepRate = 0;

	//count of elements in the layout
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &layoutPosUvNorm3PosNorm);
	if (FAILED(result)){ return false; }

	//we no longer need the shader buffers, so release them
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	/////////////////////////////////////////////////////////////////////// Samplers /////////////////////////////////////////////////////////////////////////
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
	result = device->CreateSamplerState(&samplerDesc, &sampleStateWrap);
	if (FAILED(result))
	{
		return false;
	}

	//Create a CLAMP texture sampler state description.
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	//Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &sampleStateClamp);
	if (FAILED(result))
		return false;

	/////////////////////////////////////////////////////////////////////// Other /////////////////////////////////////////////////////////////////////////
	//CONSTANT BUFFER DESCRIPTIONS:
	//this is the dynamic matrix constant buffer that is in the VERTEX SHADER
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	//create a pointer to constant buffer, so we can acess the vertex shader constant buffer within this class
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferPerBlendObject);
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBufferPerBlendObject);

	if (FAILED(result))
		throw std::runtime_error("\nFailed to create matrix buffer");

	//this is the light dynamic constant buffer in the PIXEL SHADER
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	lightBufferDesc.ByteWidth = sizeof(LightBuffer);
	result = device->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);

	if (FAILED(result))
		throw std::runtime_error("\nFailed to create light buffer");

	return true;

}

bool RenderModule::SetDataPerObject(XMMATRIX& worldMatrix, RenderObject* renderObject, bool isSelected)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNr;
	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();

	XMMATRIX worldMatrixC;

	worldMatrixC = XMMatrixTranspose(worldMatrix);

	//lock the constant buffer for writing

	bufferNr = 0;

	//setting the sent in shader texture resource in the pixel shader
	UINT32 vertexSize;
	vertexSize = sizeof(Vertex);

	UINT32 offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &renderObject->model->vertexBuffer, &vertexSize, &offset);
	deviceContext->PSSetShaderResources(0, 1, &renderObject->diffuseTexture);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	worldMatrixC = XMMatrixTranspose(worldMatrix);
	bufferNr = 0;
	result = deviceContext->Map(matrixBufferPerObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }

	MatrixBufferPerObject* dataPtr = (MatrixBufferPerObject*)mappedResource.pData;

	dataPtr->world = worldMatrixC;
	dataPtr->isSelected = isSelected;

	deviceContext->Unmap(matrixBufferPerObject, 0);

	deviceContext->VSSetConstantBuffers(bufferNr, 1, &matrixBufferPerObject);
	return true;
}

bool RenderModule::SetDataPerBlendObject(XMMATRIX& worldMatrix, RenderObject* renderObject, bool isSelected, XMFLOAT4 weights)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNr;
	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();

	XMMATRIX worldMatrixC;

	worldMatrixC = XMMatrixTranspose(worldMatrix);

	//lock the constant buffer for writing

	bufferNr = 0;

	//setting the sent in shader texture resource in the pixel shader
	UINT32 vertexSize;
	vertexSize = sizeof(BlendVertex);

	UINT32 offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &renderObject->model->vertexBuffer, &vertexSize, &offset);
	deviceContext->PSSetShaderResources(0, 1, &renderObject->diffuseTexture);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	worldMatrixC = XMMatrixTranspose(worldMatrix);
	bufferNr = 0;

	result = deviceContext->Map(matrixBufferPerBlendObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }

	MatrixBufferPerBlendObject* dataPtr = (MatrixBufferPerBlendObject*)mappedResource.pData;

	dataPtr->world = worldMatrixC;
	dataPtr->weight[0] = weights.x;
	dataPtr->weight[1] = weights.y;
	dataPtr->weight[2] = weights.z;
	dataPtr->weight[3] = weights.w;

	deviceContext->Unmap(matrixBufferPerBlendObject, 0);

	deviceContext->VSSetConstantBuffers(bufferNr, 1, &matrixBufferPerBlendObject);
	return true;
}

bool RenderModule::SetDataPerSkeletalObject(XMMATRIX& worldMatrix, RenderObject* renderObject, bool isSelected, float frame)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNr;
	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();

	XMMATRIX worldMatrixC;

	worldMatrixC = XMMatrixTranspose(worldMatrix);

	//lock the constant buffer for writing

	bufferNr = 0;

	//setting the sent in shader texture resource in the pixel shader
	UINT32 vertexSize;
	vertexSize = sizeof(WeightedVertex);

	UINT32 offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &renderObject->model->vertexBuffer, &vertexSize, &offset);
	deviceContext->PSSetShaderResources(0, 1, &renderObject->diffuseTexture);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	worldMatrixC = XMMatrixTranspose(worldMatrix);

		int currentFrame = (int)frame, finalFrame = renderObject->model->skeleton[0].frames.size();
		float interpolation = frame - (float)currentFrame;

		std::vector<XMFLOAT4X4> boneLocalMatrices;
		std::vector<XMFLOAT4X4> boneGlobalMatrices;
		std::vector<Bone>* bones = &renderObject->model->skeleton;
		boneGlobalMatrices.resize(renderObject->model->skeleton.size());
		boneLocalMatrices.resize(renderObject->model->skeleton.size());
		XMFLOAT3 idnt(1, 1, 1);
		XMVECTOR S = XMLoadFloat3(&idnt);
		XMVECTOR P = XMLoadFloat3(&bones->at(0).frames[currentFrame].trans);
		XMVECTOR Q = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&bones->at(0).frames[currentFrame].rot));

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&boneLocalMatrices[0], XMMatrixAffineTransformation(S, zero, Q, P));

		XMStoreFloat4x4(&boneLocalMatrices[0], XMMatrixRotationQuaternion(Q)*XMMatrixTranslationFromVector(P));

		boneGlobalMatrices[0] = boneLocalMatrices[0];
		for (int i = 1; i < (signed)boneGlobalMatrices.size(); i++)
		{
			if (currentFrame < finalFrame){

				XMVECTOR s0 = XMLoadFloat3(&idnt);
				XMVECTOR trans = XMLoadFloat3(&bones->at(i).frames[currentFrame].trans);
				XMVECTOR rot = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&bones->at(i).frames[currentFrame].rot));

				if (currentFrame + 1 < finalFrame){

					XMVECTOR s1 = XMLoadFloat3(&idnt);
					XMVECTOR trans2 = XMLoadFloat3(&bones->at(i).frames[currentFrame + 1].trans);
					XMVECTOR rot2 = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&bones->at(i).frames[currentFrame + 1].rot));

					S = XMVectorLerp(s0, s1, interpolation);
					P = XMVectorLerp(trans, trans2, interpolation);
					Q = XMQuaternionSlerp(rot, rot2, interpolation);

					XMStoreFloat4x4(&boneLocalMatrices[i], XMMatrixAffineTransformation(S, zero, Q, P));
				}
				else
				{
					XMStoreFloat4x4(&boneLocalMatrices[i], XMMatrixAffineTransformation(S, zero, rot, trans));
				}
			}
			XMMATRIX local = XMLoadFloat4x4(&boneLocalMatrices[i]);
			XMMATRIX global = XMLoadFloat4x4(&boneGlobalMatrices[bones->at(i).parent]);
			XMMATRIX toRoot = XMMatrixMultiply(local, global);

			XMStoreFloat4x4(&boneGlobalMatrices[i], toRoot);
		}

		result = deviceContext->Map(matrixBufferPerWeightedObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result)) { return false; }
		MatrixBufferPerWeightedObject* dataPtr = (MatrixBufferPerWeightedObject*)mappedResource.pData;
		dataPtr->world = worldMatrixC;

		std::vector<XMFLOAT4X4> finalTransforms;
		finalTransforms.resize(boneGlobalMatrices.size());
		for (int i = 0; i < (signed)boneGlobalMatrices.size(); i++)
		{
			XMMATRIX offset = XMLoadFloat4x4(&bones->at(i).invBindPose);
			XMMATRIX toRoot = XMLoadFloat4x4(&boneGlobalMatrices[i]);

			XMStoreFloat4x4(&finalTransforms[i], XMMatrixMultiply(offset, toRoot));

			dataPtr->bones[i] = finalTransforms[i];
			//XMStoreFloat4x4(&dataPtr->bones[i], XMMatrixTranslation(0, 0, 0));
		}

		deviceContext->Unmap(matrixBufferPerWeightedObject, 0);
		deviceContext->VSSetConstantBuffers(bufferNr, 1, &matrixBufferPerWeightedObject);
	
	return true;
}

bool RenderModule::SetDataPerFrame(XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix, XMFLOAT3& camPos, LightObject * spotlight, LevelStates* levelstate)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNr;
	MatrixBufferPerFrame* dataPtr;
	LightBuffer* lightPtr;
	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();

	XMMATRIX viewMatrixC, projectionMatrixC;

	//transposing the matrices
	viewMatrixC = XMMatrixTranspose(viewMatrix);
	projectionMatrixC = XMMatrixTranspose(projectionMatrix);

	//lock the constant buffer for writing
	result = deviceContext->Map(matrixBufferPerFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }

	dataPtr = (MatrixBufferPerFrame*)mappedResource.pData;

	dataPtr->viewMatrix = viewMatrixC;
	dataPtr->projectionMatrix = projectionMatrixC;
	dataPtr->camPos = camPos;

	deviceContext->Unmap(matrixBufferPerFrame, 0);

	bufferNr = 1;

	//setting matrix constant buffer in the VS with its new and updated values
	deviceContext->VSSetConstantBuffers(bufferNr, 1, &matrixBufferPerFrame);
	
	//lock the constant buffer for writing
	result = deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }
	
	lightPtr = (LightBuffer*)mappedResource.pData;


	lightPtr->lightPosSpot = spotlight->getPosition();
	lightPtr->lightDirSpot = spotlight->getDirection();
	lightPtr->lightRangeSpot = spotlight->getRange();
	lightPtr->lightConeSpot = spotlight->getCone();
	lightPtr->lightAttSpot = XMFLOAT3(0.3f, 0.03f, 0.0f);
	lightPtr->lightAmbientSpot = spotlight->getAmbientColor();
	lightPtr->lightDiffuseSpot = spotlight->getDiffuseColor();

	//if (levelstate->currentLevel->getEndDoor())
	//{
	//Point light which only want to exist at Enddoor, therefore needs to get enddoor position (probably without Y to put it at a good height manually):
	lightPtr->lightDiffusePoint1 = XMFLOAT4(0.95f, 0.1f, 0.2f, 1.0f);
	lightPtr->lightPosPoint1 = XMFLOAT3(-1.0f, -2.4f, -2.0f);
	//}

	if (levelstate->currentLevelNr == levelstate->menuLevel->GetLevelNr())
	{
		////Point light which won't look good but need a bool to only exist in Menu Level:
		lightPtr->lightDiffusePoint2 = XMFLOAT4(0.55f, 0.45f, 0.2f, 1.0f);
		lightPtr->lightPosPoint2 = XMFLOAT3(-7.0f, -2.0f, -5.5f);
	}
	else
	{
	}

	deviceContext->Unmap(lightBuffer, 0);

	bufferNr = 0;

	//setting matrix constant buffer in the VS with its new and updated values
	deviceContext->PSSetConstantBuffers(bufferNr, 1, &lightBuffer);

	return true;
}

void RenderModule::UseDefaultShader()
{
	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();

	deviceContext->IASetInputLayout(layoutPosUvNorm);

	d3d->SetCullingState(3);

	//Set shaders
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	deviceContext->PSSetSamplers(0, 1, &sampleStateClamp);
	deviceContext->PSSetSamplers(1, 1, &sampleStateWrap);
}

void RenderModule::ActivateShadowRendering(XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix)
{
	d3d->SetCullingState(2);
	shadowMap->ActivateShadowRendering(d3d->GetDeviceContext());
	XMMATRIX view, proj;
	//spotLight->getViewMatrix(view);
	//spotLight->getProjMatrix(proj);
	shadowMap->SetBufferPerFrame(d3d->GetDeviceContext(), viewMatrix, projectionMatrix);
}

void RenderModule::UseSkeletalShader()
{
	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();

	deviceContext->IASetInputLayout(layoutPosUvNormIdxWei);

	d3d->SetCullingState(3);

	//Set shaders
	deviceContext->VSSetShader(skeletalVertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	deviceContext->PSSetSamplers(0, 1, &sampleStateClamp);
	deviceContext->PSSetSamplers(1, 1, &sampleStateWrap);
}

void RenderModule::UseBlendShader()
{
	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();

	deviceContext->IASetInputLayout(layoutPosUvNorm3PosNorm);

	d3d->SetCullingState(3);

	//Set shaders
	deviceContext->VSSetShader(blendVertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	deviceContext->PSSetSamplers(0, 1, &sampleStateClamp);
	deviceContext->PSSetSamplers(1, 1, &sampleStateWrap);
}

bool RenderModule::Render(GameObject* gameObject)
{
	bool result = true;

	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();
	RenderObject* renderObject = gameObject->GetRenderObject();

	if (renderObject->model->hasSkeleton)
		if (renderObject->model->hasBlendShapes)
		{
			//UseSkeletalBlendShader();
		}
		else
		{
			throw std::runtime_error(string("\nDetta objekt: " + gameObject->GetRenderObject()->model->name + "\nbehöver: GameObject* gameObject, float frame"));
		}
	else
		if (renderObject->model->hasBlendShapes)
		{
			throw std::runtime_error("\nDetta objekt: " + gameObject->GetRenderObject()->model->name + "\nbehöver: GameObject* gameObject, float weights[4]");
		}
		else
		{
			UseDefaultShader();
			result = SetDataPerObject(gameObject->GetWorldMatrix(), renderObject, gameObject->IsSelected());
		}

	//Set shader parameters, preparing them for render.
	if (!result)
		return false;

	//Now render the prepared buffers with the shader.
	deviceContext->Draw(renderObject->model->vertexBufferSize, 0);

	return result;
}

bool RenderModule::Render(GameObject* gameObject, float frame)
{
	bool result = true;

	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();
	RenderObject* renderObject = gameObject->GetRenderObject();

	if (renderObject->model->hasSkeleton)
		if (renderObject->model->hasBlendShapes)
		{
			//UseSkeletalBlendShader();
		}
		else
		{
			UseSkeletalShader();
			result = SetDataPerSkeletalObject(gameObject->GetWorldMatrix(), renderObject, gameObject->IsSelected(), frame);
		}
	else
		if (renderObject->model->hasBlendShapes)
		{
			throw std::runtime_error("\nDetta objekt: " + gameObject->GetRenderObject()->model->name + "\nbehöver: GameObject* gameObject, float weights[4]");
		}
		else
		{
			UseDefaultShader();
			result = SetDataPerObject(gameObject->GetWorldMatrix(), renderObject, gameObject->IsSelected());
		}

	//Set shader parameters, preparing them for render.
	if (!result)
		return false;

	//Now render the prepared buffers with the shader.
	deviceContext->Draw(renderObject->model->vertexBufferSize, 0);

	return result;
}

bool RenderModule::Render(GameObject* gameObject, XMFLOAT4 weights)
{
	bool result = true;

	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();
	RenderObject* renderObject = gameObject->GetRenderObject();

	if (renderObject->model->hasSkeleton)
	{
		if (renderObject->model->hasBlendShapes)
		{
			//UseSkeletalBlendShader();
		}
		else
		{
			throw std::runtime_error("\nDetta objekt: " + gameObject->GetRenderObject()->model->name + "\nbehöver: GameObject* gameObject, float frame");
		}
	}
	else
	{
		if (renderObject->model->hasBlendShapes)
		{
			UseBlendShader();
			result = SetDataPerBlendObject(gameObject->GetWorldMatrix(), renderObject, gameObject->IsSelected(), weights);
		}
		else
		{
			UseDefaultShader();
			result = SetDataPerObject(gameObject->GetWorldMatrix(), renderObject, gameObject->IsSelected());
		}
	}

	//Set shader parameters, preparing them for render.
	if (!result)
		return false;

	//Now render the prepared buffers with the shader.
	deviceContext->Draw(renderObject->model->vertexBufferSize, 0);

	return result;
}

bool RenderModule::RenderShadow(GameObject* gameObject)
{
	bool result = true;

	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();
	RenderObject* renderObject = gameObject->GetRenderObject();

	shadowMap->SetBufferPerObject(deviceContext, gameObject->GetWorldMatrix());

	//Now render the prepared buffers with the shader.
	deviceContext->Draw(renderObject->model->vertexBufferSize, 0);
	return result;

}

void RenderModule::BeginScene(float red, float green, float blue, float alpha)
{
	d3d->BeginScene(red, green, blue, alpha);
}
void RenderModule::EndScene()
{
	d3d->EndScene();
}

ID3D11Device* RenderModule::GetDevice()
{
	return d3d->GetDevice();
}
ID3D11DeviceContext* RenderModule::GetDeviceContext()
{
	return d3d->GetDeviceContext();
}