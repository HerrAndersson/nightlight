#pragma once
#include "AssetUtil.h"

using std::vector;

class AssetManager
{
public:
	AssetManager(ID3D11Device* device_);
	~AssetManager();
private:
	ID3D11Device* device;
	std::vector<RenderObject*> renderObjects;
	void CreateRenderObject(int modelID, int diffuseID, int specularID);

	void LoadModel(std::string file_path);
	void CreateRenderObject(int modelID, int diffuseID, int specularID);
	void LoadTexture(std::string file_path);

	std::vector<ID3D11ShaderResourceView*> textures;
	void LoadTexture(std::string file_path);
	ID3D11Buffer* CreateVertexBuffer(vector<XMFLOAT3> *points, vector<XMFLOAT3> *normals, vector<XMFLOAT2> *UVs, vector<XMINT3> *faceIndices);
	void FileToStrings(std::string file_path, std::vector<std::string> &output);
	std::vector<int> StringToIntArray(std::string input);

/*
	RenderObject* GetRenderObject(); 
	ID3D11Buffer* getVertexBuffer();
	void setUpBuffers(ID3D11DeviceContext*);
*/	 
};

