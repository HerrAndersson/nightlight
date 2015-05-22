#pragma once
#include "AssetUtil.h"

using namespace std;
using namespace assetUtility;

class AssetManager
{

public:

	AssetManager();
	~AssetManager();
	RenderObject* GetRenderObject(int id);
	void LoadModel(string file_path, Model& model, MaterialData& material);
private:

	ID3D11Device* device;
	vector<Model*> models;
	vector<ID3D11ShaderResourceView*> textures;
	vector<RenderObject*> renderObjects;


	void CreateRenderObject(int modelID, int diffuseID, int specularID);
	void LoadTexture(string file_path);

	ID3D11Buffer* CreateVertexBuffer(vector<WeightedPoint> *points, vector<Point> *purePoints, vector<XMFLOAT3> *normals, vector<XMFLOAT2> *UVs, vector<XMINT3> *vertexIndices, bool hasSkeleton, vector<BlendShape> *blendShapes);
};

