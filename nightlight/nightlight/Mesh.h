#pragma once
#include "AssetUtil.h"

class Mesh
{
private:

	std::vector<Vertex> vertices;
	Material* material;
	Texture* diffuseTexture;
	Texture* specularTexture;

public:
	Mesh();
	virtual ~Mesh();
};