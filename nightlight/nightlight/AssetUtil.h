#pragma once
#include <vector>
#include <string>
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

struct MainHeader{
	int meshCount, matCount, camCount, lightCount;
};

struct MeshHeader
{
	int nameLength, numberPoints, numberNormals, numberCoords, numberFaces;
};

struct MatHeader{
	int diffuseNameLength, ambientNameLength, specularNameLength, transparencyNameLength, glowNameLength;
};

struct Texture
{
	std::string textureName = "";
	ID3D11ShaderResourceView* texturePointer = nullptr;
};

struct Material
{
	std::string materialName = "";
	XMFLOAT3	diffuse;
	XMFLOAT3	specular;
	float		shine;
};

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT2 uv;
	XMFLOAT3 normal;
};