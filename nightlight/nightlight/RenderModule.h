#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <fstream>
#include "D3DManager.h"
#include "AssetUtil.h"

using DirectX::XMMATRIX;
using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;

class RenderModule
{

private:

	struct MatrixBuffer
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	D3DManager*				d3d;

	//Vertex shaders
	ID3D11VertexShader*		vertexShader;

	//Pixel shaders
	ID3D11PixelShader*		pixelShader;

	//Sampler states
	ID3D11SamplerState*		sampleStateWrap;
	ID3D11SamplerState*		sampleStateClamp;

	//Other
	ID3D11InputLayout*		layoutPosUvNorm;
	ID3D11Buffer*			matrixBuffer;

	HWND hwnd;

public:

	RenderModule(HWND hwnd, int screenWidth, int screenHeight, bool fullscreen);
	~RenderModule();

	bool InitializeShader(WCHAR* vsFilename, WCHAR* psFilename);

	bool SetShaderParameters(XMMATRIX& worldMatrix, XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11Buffer* vertexBuffer, int vertexCount);

	void UseDefaultShader();
	//void UseShadowShader();
	//void Usesomeothershader() etc.

	void BeginScene(float red, float green, float blue, float alpha);
	bool Render(XMMATRIX& worldMatrix, XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11Buffer* vertexBuffer, int vertexCount);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
};



