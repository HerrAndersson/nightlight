#include "AssetManager.h"

AssetManager::AssetManager(ID3D11Device* device)
{
	this->device = device;

	vector<string> modelNames;
	FileToStrings("Assets/models.txt", modelNames);
	for (int i = 0; i < (signed)modelNames.size(); i++)
		LoadModel("Assets/Models/" + modelNames[i]);

	vector<string> textureNames;
	FileToStrings("Assets/textures.txt", textureNames);
	for (int i = 0; i < (signed)textureNames.size(); i++)
		LoadTexture("Assets/Textures/" + textureNames[i]);

	vector<string> renderObjectIDs;
	FileToStrings("Assets/renderObjects.txt", renderObjectIDs);
	for (int i = 0; i < (signed)renderObjectIDs.size(); i++)
	{
		vector<int> IDs = StringToIntArray(renderObjectIDs[i]);
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
	MeshHeader meshHeader;
	infile.read((char*)&mainHeader, sizeof(MainHeader));
	infile.read((char*)&meshHeader, sizeof(MeshHeader));

	string name;
	vector<XMFLOAT3> points;
	vector<XMFLOAT3> normals;
	vector<XMFLOAT2> UVs;
	vector<XMINT3> vertexIndices;

	name.resize(meshHeader.nameLength);
	points.resize(meshHeader.numberPoints);
	normals.resize(meshHeader.numberNormals);
	UVs.resize(meshHeader.numberCoords);
	vertexIndices.resize(meshHeader.numberFaces * 3);

	infile.read((char*)name.data(), meshHeader.nameLength);
	infile.read((char*)points.data(), meshHeader.numberPoints*sizeof(XMFLOAT3));
	infile.read((char*)normals.data(), meshHeader.numberNormals*sizeof(XMFLOAT3));
	infile.read((char*)UVs.data(), meshHeader.numberCoords*sizeof(XMFLOAT2));
	infile.read((char*)vertexIndices.data(), meshHeader.numberFaces*sizeof(XMINT3) * 3);

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
	if (mainHeader.spotLightSize)
		infile.read((char*)&model->spotLight, sizeof(SpotLightStruct));

	model->vertexBufferSize = vertexIndices.size();
	infile.close();

	model->vertexBuffer = CreateVertexBuffer(&points, &normals, &UVs, &vertexIndices);
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

ID3D11Buffer* AssetManager::CreateVertexBuffer(vector<XMFLOAT3> *points, vector<XMFLOAT3> *normals, vector<XMFLOAT2> *UVs, vector<XMINT3> *vertexIndices)
{
	vector<Vertex> vertices;

	D3D11_BUFFER_DESC vbDESC;
	vbDESC.Usage = D3D11_USAGE_DEFAULT;
	vbDESC.ByteWidth = sizeof(Vertex)* vertexIndices->size();
	vbDESC.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDESC.CPUAccessFlags = 0;
	vbDESC.MiscFlags = 0;
	vbDESC.StructureByteStride = 0;
	
	for (int i = 0; i < (signed)vertexIndices->size(); i+=3){
		for (int a = 0; a < 3; a++){
			Vertex tempVertex;
			tempVertex.position = points->at(vertexIndices->at(i+a).x);
			tempVertex.normal = normals->at(vertexIndices->at(i+a).y);
			tempVertex.uv = UVs->at(vertexIndices->at(i+a).z);
			vertices.push_back(tempVertex);
		}
	}

	D3D11_SUBRESOURCE_DATA vertexData;

	vertexData.pSysMem = vertices.data();
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

//turns a text file into a vector of strings line-by-line
void AssetManager::FileToStrings(string file_path, vector<string> &output)
{
	string delimiter = "\n";
	ifstream file(file_path);
	if (!file.is_open()){
		printf("Failed to open models.txt\n");
	}
	string s((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());


	size_t pos = 0;
	string token;
	while ((pos = s.find(delimiter)) != string::npos) {
		token = s.substr(0, pos);
		output.push_back(token);
		s.erase(0, pos + delimiter.length());
	}
	output.push_back(s);
};

vector<int> AssetManager::StringToIntArray(string input)
{
	vector<int> output;
	int from = 0;
	
	for (int to = 0; to < (signed)input.size(); to++)
	{
		if (input[to] == ','){
			output.push_back(stoi(input.substr(from, to - from)));
			from = to+1;
		}
	}
	output.push_back(stoi(input.substr(from, input.size() - from)));

	return output;
}

RenderObject* AssetManager::GetRenderObject(int id)
{
	return renderObjects[id];
}