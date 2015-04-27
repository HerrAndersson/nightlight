#pragma once
#include "AssetUtil.h"

using namespace std;

class AssetManager
{

public:

	AssetManager(ID3D11Device* device);
	~AssetManager();
	RenderObject* GetRenderObject(int id);

private:

	ID3D11Device* device;
	vector<Model*> models;
	vector<ID3D11ShaderResourceView*> textures;
	vector<RenderObject*> renderObjects;

	void LoadModel(string file_path);
	void CreateRenderObject(int modelID, int diffuseID, int specularID);
	void LoadTexture(string file_path);

	ID3D11Buffer* CreateVertexBuffer(vector<XMFLOAT3> *points, vector<XMFLOAT3> *normals, vector<XMFLOAT2> *UVs, vector<XMINT3> *faceIndices);
	void FileToStrings(string file_path, vector<string> &output);
	vector<int> StringToIntArray(string input);

};

