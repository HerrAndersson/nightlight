#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <fstream>
#include "D3DManager.h"
#include "AssetUtil.h"
#include "GameObject.h"
#include "lightObject.h"

using DirectX::XMMATRIX;
using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;

class RenderModule
{

private:
	
	
	struct LightBuffer
	{
		//spotlight
		XMFLOAT3 lightPosSpot;
		float  lightRangeSpot;
		XMFLOAT3 lightDirSpot;
		float lightConeSpot;
		XMFLOAT3 lightAttSpot;
		float pad;
		XMFLOAT4 lightAmbientSpot;
		XMFLOAT4 lightDiffuseSpot;

		XMFLOAT3 lightPosPoint1;
		float pad2;
		XMFLOAT4 lightDiffusePoint1;
		
	};

	struct MatrixBufferPerObject
	{
		XMMATRIX world;
	};

	struct MatrixBufferPerFrame
	{
		XMMATRIX viewMatrix;
		XMMATRIX projectionMatrix;
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
	ID3D11Buffer*			matrixBufferPerObject;
	ID3D11Buffer*			matrixBufferPerFrame;
	ID3D11Buffer*			lightBuffer;
	HWND					hwnd;

	bool SetDataPerFrame(XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix, LightObject * spotlight);

public:

	RenderModule(HWND hwnd, int screenWidth, int screenHeight, bool fullscreen);
	~RenderModule();

	bool InitializeShader(WCHAR* vsFilename, WCHAR* psFilename);

	bool SetDataPerObject(XMMATRIX& worldMatrix, ID3D11ShaderResourceView* texture, ID3D11Buffer* vertexBuffer);

	void UseDefaultShader();
	//void UseShadowShader();
	//void Usesomeothershader() etc.

	void BeginScene(float red, float green, float blue, float alpha, XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix, LightObject * spotlight);

	bool Render(GameObject* gameObject);

	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
};



