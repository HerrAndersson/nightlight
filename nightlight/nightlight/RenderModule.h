#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <fstream>
#include "D3DManager.h"
#include "AssetUtil.h"
#include "GameObject.h"
#include "lightObject.h"
#include "ShadowMap.h"

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

		XMFLOAT3 lightPosPoint2;
		float pad3;
		XMFLOAT4 lightDiffusePoint2;

		//spotlight
		XMFLOAT3 lightPosSpot2;
		float  lightRangeSpot2;
		XMFLOAT3 lightDirSpot2;
		float lightConeSpot2;
		XMFLOAT3 lightAttSpot2;
		float pad4;
		XMFLOAT4 lightAmbientSpot2;
		XMFLOAT4 lightDiffuseSpot2;
		
	};

	struct MatrixBufferPerObject
	{
		XMMATRIX world;
		int isSelected;
	};

	struct MatrixBufferPerWeightedObject
	{
		XMMATRIX world;
		XMFLOAT4X4 bones[30];
	};

	struct MatrixBufferPerBlendObject
	{
		XMMATRIX world;
		int isSelected;
		float weight[4];
	};


	struct MatrixBufferPerFrame
	{
		XMMATRIX viewMatrix;
		XMMATRIX projectionMatrix;
		XMFLOAT3 camPos;
	};

	D3DManager*				d3d;
	ShadowMap*				shadowMap;

	//Vertex shaders
	ID3D11VertexShader*		vertexShader;
	ID3D11VertexShader*		skeletalVertexShader;
	ID3D11VertexShader*		blendVertexShader;

	//Pixel shaders
	ID3D11PixelShader*		pixelShader;

	//Sampler states
	ID3D11SamplerState*		sampleStateWrap;
	ID3D11SamplerState*		sampleStateClamp;

	//Other
	ID3D11InputLayout*		layoutPosUvNorm;
	ID3D11InputLayout*		layoutPosUvNormIdxWei;
	ID3D11InputLayout*		layoutPosUvNorm3PosNorm;
	ID3D11Buffer*			matrixBufferPerObject;
	ID3D11Buffer*			matrixBufferPerWeightedObject;
	ID3D11Buffer*			matrixBufferPerBlendObject;
	ID3D11Buffer*			matrixBufferPerFrame;
	ID3D11Buffer*			lightBuffer;
	HWND					hwnd;

public:
	bool SetDataPerFrame(XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix, XMFLOAT3& camPos, LightObject * spotlight);

	RenderModule(HWND hwnd, int screenWidth, int screenHeight, bool fullscreen);
	~RenderModule();

	bool InitializeShader(WCHAR* vsFilename, WCHAR* psFilename);
	bool InitializeSkeletalShader(WCHAR* vsFilename, WCHAR* psFilename);
	bool InitializeBlendShader(WCHAR* vsFilename, WCHAR* psFilename);

	bool SetDataPerObject(XMMATRIX& worldMatrix, RenderObject* renderObject, bool isSelected);
	bool SetDataPerBlendObject(XMMATRIX& worldMatrix, RenderObject* renderObject, bool isSelected, float weights[4]);
	bool SetDataPerSkeletalObject(XMMATRIX& worldMatrix, RenderObject* renderObject, bool isSelected, float frame);

	void UseDefaultShader();
	void ActivateShadowRendering(XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix);
	void UseSkeletalShader();
	void UseBlendShader();
	//void Usesomeothershader() etc.

	void BeginScene(float red, float green, float blue, float alpha);

	bool Render(GameObject* gameObject);
	bool Render(GameObject* gameObject, float frame);
	bool Render(GameObject* gameObject, float weights[4]);
	bool RenderShadow(GameObject* gameObject);

	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
};



