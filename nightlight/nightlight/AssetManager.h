#pragma once
#include "AssetUtil.h"
#include <fstream>
struct RenderObject{
	ID3D11Buffer* vertexBuffer;
	Material* material;
	Texture* diffuseTexture;
	Texture* specularTexture;
};

class AssetManager
{
public:
	RenderObject asset; //single test asset
	AssetManager(ID3D11Device* device);
	RenderObject* LoadRenderObject(std::string file_path);
	
	ID3D11Buffer* getVertexBuffer();

	~AssetManager();
private:
	ID3D11Buffer* CreateVertexBuffer(std::vector<XMFLOAT3> *points, std::vector<XMFLOAT3> *normals, std::vector<XMFLOAT2> *UVs, std::vector<XMINT3> *faceIndices);
	void setUpBuffers(ID3D11DeviceContext*);
	
	std::vector<RenderObject> renderObjects;
};

