#pragma once
#include <vector>
#include <string>
#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>
#include <fstream>
#include "WICTextureLoader\WICTextureLoader.h"

using namespace DirectX;

struct MainHeader
{
	int meshCount, matCount, camCount, ambientLightSize, areaLightSize, dirLightSize, pointLightSize, spotLightSize;
};

struct MeshHeader
{
	int nameLength, numberPoints, numberNormals, numberCoords, numberFaces;
};

struct MatHeader
{
	int diffuseNameLength, ambientNameLength, specularNameLength, transparencyNameLength, glowNameLength;
};

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT2 uv;
	XMFLOAT3 normal;
};

struct AmbientLightStruct
{
	double intensity;
	XMFLOAT3 color;
	XMFLOAT3 pos;
};

struct AreaLightStruct
{
	double intensity;
	XMFLOAT3 color;
	XMFLOAT3 pos;
};

struct DirectionalLightStruct
{
	double intensity;
	XMFLOAT3 color;
	XMFLOAT3 dir;
	XMFLOAT3 pos;
};

struct PointLightStruct
{
	double intensity;
	XMFLOAT3 color;
	XMFLOAT3 pos;
};

struct SpotLightStruct
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
	std::vector<PointLightStruct> pointLights;
	SpotLightStruct spotLight;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
};

struct RenderObject
{
	Model* model;
	ID3D11ShaderResourceView* diffuseTexture = nullptr;
	ID3D11ShaderResourceView* specularTexture = nullptr;
};

struct LightData
{
	std::vector<PointLightStruct> pointLights;
	std::vector<AmbientLightStruct> ambientLights;
	std::vector<AreaLightStruct> areaLights;
	std::vector<DirectionalLightStruct> dirLights;
	std::vector<SpotLightStruct> spotLights;
};