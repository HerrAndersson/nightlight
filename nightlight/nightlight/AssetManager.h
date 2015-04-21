#pragma once
#include "AssetUtil.h"
#include <fstream>

using std::vector;

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

	RenderObject* GetRenderObject(); 

private:
	ID3D11Buffer* CreateVertexBuffer(vector<XMFLOAT3> *points, vector<XMFLOAT3> *normals, vector<XMFLOAT2> *UVs, vector<XMINT3> *faceIndices);
	void setUpBuffers(ID3D11DeviceContext*);
	
	vector<RenderObject> renderObjects;
};

