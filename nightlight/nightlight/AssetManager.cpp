#include "AssetManager.h"

AssetManager::AssetManager(ID3D11Device* device)
{
	this->device = device;

	vector<string> modelNames;
	fileToStrings("Assets/models.txt", modelNames);
	for (int i = 0; i < (signed)modelNames.size(); i++)
		LoadModel("Assets/Models/" + modelNames[i]);

	vector<string> textureNames;
	fileToStrings("Assets/textures.txt", textureNames);
	for (int i = 0; i < (signed)textureNames.size(); i++)
		LoadTexture("Assets/Textures/" + textureNames[i]);

	vector<string> renderObjectIDs;
	fileToStrings("Assets/renderObjects.txt", renderObjectIDs);
	for (int i = 0; i < (signed)renderObjectIDs.size(); i++)
	{
		vector<int> IDs = stringToIntArray(renderObjectIDs[i]);
		CreateRenderObject(IDs[0], IDs[1], IDs[2]);
	}
};

AssetManager::~AssetManager()
{
	for (auto m : models) delete m;
	models.clear();

	for (auto t : textures) t->Release();
	textures.clear();

	for (auto ro : renderObjects) delete ro;
	renderObjects.clear();
};

void AssetManager::LoadModel(string file_path){
	Model* model = new Model();

	ifstream infile;
	infile.open(file_path.c_str(), ifstream::binary);
	if (!infile.is_open())
	{
		string outputstring = file_path + " not found.\n";
		throw runtime_error(outputstring.c_str());
		return;
	}
	MainHeader mainHeader;
	infile.read((char*)&mainHeader, sizeof(MainHeader));

	string name;
	vector<Point> points;
	vector<PurePoint> purePoints;
	vector<XMFLOAT3> normals;
	vector<XMFLOAT2> UVs;
	vector<XMINT3> vertexIndices;
	bool hasSkeleton;

	for (int i = 0; i < mainHeader.meshCount; i++){
		if (i == 0){
			MeshHeader meshHeader;
			infile.read((char*)&meshHeader, sizeof(MeshHeader));


			name.resize(meshHeader.nameLength);
			if (meshHeader.hasSkeleton)
				points.resize(meshHeader.numberPoints);
			else
				purePoints.resize(meshHeader.numberPoints);
			normals.resize(meshHeader.numberNormals);
			UVs.resize(meshHeader.numberCoords);
			vertexIndices.resize(meshHeader.numberFaces * 3);
			hasSkeleton = meshHeader.hasSkeleton;


			infile.read((char*)name.data(), meshHeader.nameLength);
			if (meshHeader.hasSkeleton)
				infile.read((char*)points.data(), meshHeader.numberPoints*sizeof(Point));
			else
				infile.read((char*)purePoints.data(), meshHeader.numberPoints*sizeof(PurePoint));
			infile.read((char*)normals.data(), meshHeader.numberNormals*sizeof(XMFLOAT3));
			infile.read((char*)UVs.data(), meshHeader.numberCoords*sizeof(XMFLOAT2));
			infile.read((char*)vertexIndices.data(), meshHeader.numberFaces*sizeof(XMINT3) * 3);
		}
		else{
			MeshHeader meshHeader;
			infile.read((char*)&meshHeader, sizeof(MeshHeader));
			if (meshHeader.hasSkeleton)
			{
				infile.seekg(meshHeader.nameLength, ios::cur);
				infile.seekg(meshHeader.numberPoints*sizeof(Point), ios::cur);
				infile.seekg(meshHeader.numberNormals*sizeof(XMFLOAT3), ios::cur);
				infile.seekg(meshHeader.numberCoords*sizeof(XMFLOAT2), ios::cur);
				infile.seekg(meshHeader.numberFaces*sizeof(XMINT3) * 3, ios::cur);
			}
			else
			{
				infile.seekg(meshHeader.nameLength, ios::cur);
				infile.seekg(meshHeader.numberPoints*sizeof(PurePoint), ios::cur);
				infile.seekg(meshHeader.numberNormals*sizeof(XMFLOAT3), ios::cur);
				infile.seekg(meshHeader.numberCoords*sizeof(XMFLOAT2), ios::cur);
				infile.seekg(meshHeader.numberFaces*sizeof(XMINT3) * 3, ios::cur);
			}
		}

	}


	for (int i = 0; i < mainHeader.matCount; i++)
	{
		if (i == 0){
			MatHeader matHeader;
			infile.read((char*)&matHeader, sizeof(MatHeader));

			infile.seekg(16 + matHeader.ambientNameLength, ios::cur);

			infile.read((char*)&model->diffuse, 16);
			infile.seekg(matHeader.diffuseNameLength, ios::cur);

			infile.read((char*)&model->specular, 16);
			infile.seekg(matHeader.specularNameLength, ios::cur);

			infile.seekg(16 + matHeader.transparencyNameLength, ios::cur);

			infile.seekg(16 + matHeader.glowNameLength, ios::cur);
		}

		else{
			MatHeader matHeader;
			infile.read((char*)&matHeader, sizeof(MatHeader));
			infile.seekg(80
				+ matHeader.ambientNameLength
				+ matHeader.diffuseNameLength
				+ matHeader.specularNameLength
				+ matHeader.transparencyNameLength
				+ matHeader.glowNameLength,
				ios::cur);
		}
	}

	model->pointLights.resize(mainHeader.pointLightSize);

	if (mainHeader.ambientLightSize)
		infile.seekg(mainHeader.ambientLightSize*sizeof(AmbientLightStruct), ios::cur);
	if (mainHeader.areaLightSize)
		infile.seekg(mainHeader.areaLightSize*sizeof(AreaLightStruct), ios::cur);
	if (mainHeader.dirLightSize)
		infile.seekg(mainHeader.dirLightSize* sizeof(DirectionalLightStruct), ios::cur);
	if (mainHeader.pointLightSize)
		infile.read((char*)model->pointLights.data(), mainHeader.pointLightSize* sizeof(PointLightStruct));
	if (mainHeader.spotLightSize){
		infile.read((char*)&model->spotLight, sizeof(SpotLightStruct));
		infile.seekg(mainHeader.spotLightSize - 1 * sizeof(SpotLightStruct), ios::cur);
	}

	infile.seekg(mainHeader.camCount * 52, ios::cur);

/*	
	for (int i = 0; i < mainHeader.camCount; i++){
		läs kameror
	}
*/
	std::vector<BlendShape> blendShapes;
	for (int i = 0; i < mainHeader.blendShapeCount; i++){
		BlendShape blendShape;
		infile.read((char*)&blendShape.MeshTarget, 4);
		blendShape.points.resize(points.size() + purePoints.size());
		infile.read((char*)blendShape.points.data(), blendShape.points.size()*sizeof(XMFLOAT3));
		blendShape.normals.resize(normals.size());
		infile.read((char*)blendShape.normals.data(), blendShape.normals.size()*sizeof(XMFLOAT3));
		blendShapes.push_back(blendShape);
	}

	model->vertexBufferSize = vertexIndices.size();
	infile.close();

	model->vertexBuffer = CreateVertexBuffer(&points, &purePoints, &normals, &UVs, &vertexIndices, hasSkeleton);
	models.push_back(model);
}

void AssetManager::CreateRenderObject(int modelID, int diffuseID, int specularID)
{
	RenderObject* renderObject = new RenderObject();
	renderObject->model = models[modelID];

	if (diffuseID!=-1)
		renderObject->diffuseTexture = textures[diffuseID];
	if (specularID != -1)
		renderObject->specularTexture = textures[specularID];

	renderObjects.push_back(renderObject);
}

void AssetManager::LoadTexture(string file_path)
{
	ID3D11ShaderResourceView* texture;
	wstring widestr = wstring(file_path.begin(), file_path.end());
	DirectX::CreateWICTextureFromFile(device, widestr.c_str(), nullptr, &texture, 0);
	textures.push_back(texture);
}

ID3D11Buffer* AssetManager::CreateVertexBuffer(vector<Point> *points, vector<PurePoint> *purePoints, vector<XMFLOAT3> *normals, vector<XMFLOAT2> *UVs, vector<XMINT3> *vertexIndices, bool hasSkeleton)
{
	vector<Vertex> vertices;
	vector<PureVertex> pureVertices;

	D3D11_BUFFER_DESC vbDESC;
	vbDESC.Usage = D3D11_USAGE_DEFAULT;
	if (hasSkeleton)
		vbDESC.ByteWidth = sizeof(Vertex)* vertexIndices->size();
	else
		vbDESC.ByteWidth = sizeof(PureVertex)* vertexIndices->size();
	vbDESC.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDESC.CPUAccessFlags = 0;
	vbDESC.MiscFlags = 0;
	vbDESC.StructureByteStride = 0;

	for (int i = 0; i < (signed)vertexIndices->size(); i += 3){
		for (int a = 0; a < 3; a++){
			if (hasSkeleton){
				Vertex tempVertex;
				tempVertex.position = points->at(vertexIndices->at(i + a).x).position;
				tempVertex.normal = normals->at(vertexIndices->at(i + a).y);
				tempVertex.uv = UVs->at(vertexIndices->at(i + a).z);
				for (int b = 0; b < 4; b++)
				{
					tempVertex.boneIndices[b] = points->at(vertexIndices->at(i + a).x).boneIndices[b];
					tempVertex.boneWeigths[b] = points->at(vertexIndices->at(i + a).x).boneWeigths[b];
				}
				vertices.push_back(tempVertex);
			}
			else{
				PureVertex tempVertex;
				tempVertex.position = purePoints->at(vertexIndices->at(i + a).x).position;
				tempVertex.normal = normals->at(vertexIndices->at(i + a).y);
				tempVertex.uv = UVs->at(vertexIndices->at(i + a).z);
				pureVertices.push_back(tempVertex);
			}
		}
	}

	D3D11_SUBRESOURCE_DATA vertexData;

	if (hasSkeleton)
		vertexData.pSysMem = vertices.data();
	else
		vertexData.pSysMem = pureVertices.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	ID3D11Buffer* vertexBuffer;

	HRESULT result = device->CreateBuffer(&vbDESC, &vertexData, &vertexBuffer);
	if (FAILED(result)){
		throw runtime_error("Failed to create vertexBuffer");
		return nullptr;
	}

	return vertexBuffer;
}


RenderObject* AssetManager::GetRenderObject(int id)
{
	return renderObjects[id];
}