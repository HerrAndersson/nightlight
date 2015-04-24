#pragma once
#include <vector>
#include <string>
#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>
#include <fstream>
#include "WICTextureLoader\WICTextureLoader.h"

using namespace DirectX;

struct MainHeader{
	int meshCount, matCount, camCount, ambientLightSize, areaLightSize, dirLightSize, pointLightSize, spotLightSize;
};

struct MeshHeader
{
	int nameLength, numberPoints, numberNormals, numberCoords, numberFaces;
};

struct MatHeader{
	int diffuseNameLength, ambientNameLength, specularNameLength, transparencyNameLength, glowNameLength;
};

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT2 uv;
	XMFLOAT3 normal;
};

struct ambientLightStruct
{
	double intensity;
	XMFLOAT3 color;
	XMFLOAT3 pos;
};

struct areaLightStruct
{
	double intensity;
	XMFLOAT3 color;
	XMFLOAT3 pos;
};

struct directionalLightStruct
{
	double intensity;
	XMFLOAT3 color;
	XMFLOAT3 dir;
	XMFLOAT3 pos;
};

struct pointLightStruct
{
	double intensity;
	XMFLOAT3 color;
	XMFLOAT3 pos;
};

struct spotLightStruct
{
	double intensity;
	XMFLOAT3 color;
	XMFLOAT3 dir;
	XMFLOAT3 pos;

	double coneAngle;
	double penumbraAngle;
	double dropoff;
};

struct Model
{
	~Model()
	{
		vertexBuffer->Release();
		pointLights.clear();
	}
	ID3D11Buffer* vertexBuffer;
	int vertexBufferSize;
	std::vector<pointLightStruct> pointLights;
	spotLightStruct spotLight;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
};

struct RenderObject
{
	Model* model;
	ID3D11ShaderResourceView* diffuseTexture = nullptr;
	ID3D11ShaderResourceView* specularTexture = nullptr;
};

struct lightData
{
	std::vector<pointLightStruct> pointLights;
	std::vector<ambientLightStruct> ambientLights;
	std::vector<areaLightStruct> areaLights;
	std::vector<directionalLightStruct> dirLights;
	std::vector<spotLightStruct> spotLights;
};