#include "RenderModule.h"
#include <fstream>
#include <string>
using namespace std;


RenderModule::RenderModule(HWND hwnd, int screenWidth, int screenHeight, bool fullscreen)
{
	vertexShader = NULL;
	pixelShader = NULL;
	sampleStateWrap = NULL;
	sampleStateClamp = NULL;
	matrixBufferPerObject = NULL;
	matrixBufferPerFrame = NULL;
	hwnd = hwnd;

	d3d = new D3DManager(hwnd, screenWidth, screenHeight, fullscreen);

	bool result;

	//initializing shader files
	result = InitializeShader(L"Assets/Shaders/vertexShader.hlsl", L"Assets/Shaders/pixelShader.hlsl");
}


RenderModule::~RenderModule()
{
	delete d3d;

	layoutPosUvNorm->Release();
	matrixBufferPerObject->Release();
	matrixBufferPerFrame->Release();
	pixelShader->Release();
	sampleStateClamp->Release();
	sampleStateWrap->Release();
	vertexShader->Release();

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
	D3D11_BUFFER_DESC matrixBufferDesc;
	
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
			/*OutputDebugString(string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize());*/
			throw runtime_error(string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
		else
			OutputDebugString("\nVertexshader not found");;

		return false;
	}
	if (SUCCEEDED(result))
		OutputDebugString("\nVertexshader created");

	//pixel shader
	result = D3DCompileFromFile(psFilename, NULL, NULL, "pixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
			OutputDebugString("\nPixelshader failed to compile");
		else
			OutputDebugString("\nPixelshader not found");

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
		OutputDebugString("Failed to create matrix buffer");

	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferPerFrame);
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBufferPerFrame);

	if (FAILED(result))
		OutputDebugString("Failed to create matrix buffer");

	return true;
}

bool RenderModule::SetDataPerObject(XMMATRIX& worldMatrix, ID3D11ShaderResourceView* texture, ID3D11Buffer* vertexBuffer)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNr;
	MatrixBufferPerObject* dataPtr;
	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();
	
	XMMATRIX worldMatrixC;

	//transposing the matrices
	worldMatrixC = XMMatrixTranspose(worldMatrix);
	
	//lock the constant buffer for writing
	result = deviceContext->Map(matrixBufferPerObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }

	dataPtr = (MatrixBufferPerObject*)mappedResource.pData;

	dataPtr->world = worldMatrixC;
	
	deviceContext->Unmap(matrixBufferPerObject, 0);

	bufferNr = 0;

	//setting matrix constant buffer in the VS with its new and updated values
	deviceContext->VSSetConstantBuffers(bufferNr, 1, &matrixBufferPerObject);


	//setting the sent in shader texture resource in the pixel shader //No textures for the moment
	UINT32 vertexSize = sizeof(Vertex);
	UINT32 offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

bool RenderModule::SetDataPerFrame(XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNr;
	MatrixBufferPerFrame* dataPtr;
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

	deviceContext->Unmap(matrixBufferPerFrame, 0);

	bufferNr = 1;

	//setting matrix constant buffer in the VS with its new and updated values
	deviceContext->VSSetConstantBuffers(bufferNr, 1, &matrixBufferPerFrame);

	return true;
}

void RenderModule::UseDefaultShader()
{
	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();

	deviceContext->IASetInputLayout(layoutPosUvNorm);

	//Set shaders
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	deviceContext->PSSetSamplers(0, 1, &sampleStateClamp);
	deviceContext->PSSetSamplers(1, 1, &sampleStateWrap);
}


bool RenderModule::Render(GameObject* gameObject)
{
	bool result = true;

	//Set shader parameters, preparing them for render.
	RenderObject* renderObject = gameObject->GetRenderObject();
	//result = SetDataPerObject(*gameObject->GetWorldMatrix(), renderObject->diffuseTexture->texturePointer, renderObject->vertexBuffer);
	result = SetDataPerObject(*gameObject->GetWorldMatrix(), nullptr, renderObject->vertexBuffer);
	if (!result)
		return false;

	//Now render the prepared buffers with the shader.
	d3d->GetDeviceContext()->Draw(renderObject->vertexBufferSize, 0);

	return result;
}

void RenderModule::BeginScene(float red, float green, float blue, float alpha, XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix)
{
	
	bool result = SetDataPerFrame(viewMatrix, projectionMatrix);
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