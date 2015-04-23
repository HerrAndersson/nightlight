#pragma once
#include "AssetUtil.h"

using std::vector;

class AssetManager
{
public:
	RenderObject asset; //single test asset
	std::vector<RenderObject> assets;
	AssetManager(ID3D11Device* device_);
	RenderObject* LoadRenderObject(std::string file_path);
	void FileToStrings(std::string file_path, std::vector<std::string> &output);
	
	ID3D11Buffer* getVertexBuffer();
	 
	~AssetManager();

	RenderObject* GetRenderObject(); 

private:
	ID3D11Device* device;
	ID3D11Buffer* CreateVertexBuffer(vector<XMFLOAT3> *points, vector<XMFLOAT3> *normals, vector<XMFLOAT2> *UVs, vector<XMINT3> *faceIndices);
	void setUpBuffers(ID3D11DeviceContext*);
	vector<RenderObject> renderObjects;
};

