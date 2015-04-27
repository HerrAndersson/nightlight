#pragma once
#include <windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>
#include <d3dcompiler.h>
#include <string>

using namespace DirectX;

//Before rendering to shadow map, don't forget to activate FRONT-FACE culling in d3d manager

class ShadowMap
{
private:

	float dimensions;

	ID3D11Texture2D*				shadowMap;
	ID3D11DepthStencilView*			shadowDepthView;
	ID3D11ShaderResourceView*		shadowResourceView;

	ID3D11DepthStencilState*        shadowDepthState;

	D3D11_VIEWPORT					shadowViewport;

	ID3D11InputLayout*              shadowInputLayout;
	ID3D11VertexShader*             shadowVS;

public:

	ShadowMap(ID3D11Device* device, float dimensions, LPCWSTR vsFilename);
	virtual ~ShadowMap();

	ID3D11ShaderResourceView* GetShadowSRV();

	void ActivateShadowRendering(ID3D11DeviceContext* deviceContext);
};

