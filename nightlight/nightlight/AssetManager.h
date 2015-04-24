#pragma once
#include "AssetUtil.h"

using std::vector;

class AssetManager
{
public:
	RenderObject asset; //single test asset

	std::vector<RenderObject*> renderObjects;
	std::vector<Model*> models;
	std::vector<ID3D11ShaderResourceView*> textures;

	void LoadModel(std::string file_path);//todo
	void CreateRenderObject(int modelID, int diffuseID, int specularID);//todo
	void LoadTexture(std::string file_path);//todo


	void FileToStrings(std::string file_path, std::vector<std::string> &output);

	

	AssetManager(ID3D11Device* device_);
	~AssetManager();

	RenderObject* GetRenderObject(); 

private:
	ID3D11Device* device;
	ID3D11Buffer* CreateVertexBuffer(vector<XMFLOAT3> *points, vector<XMFLOAT3> *normals, vector<XMFLOAT2> *UVs, vector<XMINT3> *faceIndices);

	std::vector<int> StringToIntArray(std::string input);

/*
	void setUpBuffers(ID3D11DeviceContext*);
	ID3D11Buffer* getVertexBuffer();
*/	 
};

