#include "AssetManager.h"

void AssetManager::LoadModel(std::string file_path){
	Model* model = new Model();

	std::ifstream infile;
	infile.open(file_path.c_str(), std::ifstream::binary);
	if (!infile.is_open())
	{
		std::string outputstring = file_path + " not found.\n";
		throw std::runtime_error(outputstring.c_str());
		return;
	}
	MainHeader mainHeader;
	MeshHeader meshHeader;
	infile.read((char*)&mainHeader, sizeof(MainHeader));
	infile.read((char*)&meshHeader, sizeof(MeshHeader));

	std::string name;
	std::vector<XMFLOAT3> points;
	std::vector<XMFLOAT3> normals;
	std::vector<XMFLOAT2> UVs;
	std::vector<XMINT3> vertexIndices;

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

			infile.seekg(16 + matHeader.ambientNameLength, std::ios::cur);

			infile.read((char*)&model->diffuse, 16);
			infile.seekg(matHeader.diffuseNameLength, std::ios::cur);

			infile.read((char*)&model->specular, 16);
			infile.seekg(matHeader.specularNameLength, std::ios::cur);

			infile.seekg(16 + matHeader.transparencyNameLength, std::ios::cur);

			infile.seekg(16 + matHeader.glowNameLength, std::ios::cur);
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
				std::ios::cur);
		}
	}


	lightData tempLightStorage;
	tempLightStorage.ambientLights.resize(mainHeader.ambientLightSize);
	tempLightStorage.areaLights.resize(mainHeader.areaLightSize);
	tempLightStorage.spotLights.resize(mainHeader.spotLightSize);
	tempLightStorage.dirLights.resize(mainHeader.dirLightSize);
	model->pointLights.resize(mainHeader.pointLightSize);


	if (mainHeader.ambientLightSize)
		infile.read((char*)tempLightStorage.ambientLights.data(), mainHeader.ambientLightSize*sizeof(ambientLightStruct));
	if (mainHeader.areaLightSize)
		infile.read((char*)tempLightStorage.areaLights.data(), mainHeader.areaLightSize*sizeof(areaLightStruct));
	if (mainHeader.dirLightSize)
		infile.read((char*)tempLightStorage.dirLights.data(), mainHeader.dirLightSize* sizeof(directionalLightStruct));
	if (mainHeader.pointLightSize)
		infile.read((char*)model->pointLights.data(), mainHeader.pointLightSize* sizeof(pointLightStruct));
	if (mainHeader.spotLightSize)
		infile.read((char*)&model->spotLight, sizeof(spotLightStruct));


	model->vertexBufferSize = vertexIndices.size();
	infile.close();

	model->vertexBuffer = CreateVertexBuffer(&points, &normals, &UVs, &vertexIndices);
	models.push_back(model);
}

void AssetManager::CreateRenderObject(int modelID, int diffuseID, int specularID)
{
	RenderObject* renderObject = new RenderObject();
	renderObject->model = models[modelID];
	renderObject->diffuseTexture = textures[diffuseID];
	renderObject->specularTexture = textures[specularID];
	renderObjects.push_back(renderObject);
}

void AssetManager::LoadTexture(std::string file_path)
{
	ID3D11ShaderResourceView* texture;
	std::wstring widestr = std::wstring(file_path.begin(), file_path.end());
	DirectX::CreateWICTextureFromFile(device, widestr.c_str(), nullptr, &texture, 0);
	textures.push_back(texture);
}

ID3D11Buffer* AssetManager::CreateVertexBuffer(std::vector<XMFLOAT3> *points, std::vector<XMFLOAT3> *normals, std::vector<XMFLOAT2> *UVs, std::vector<XMINT3> *vertexIndices){

	std::vector<Vertex> vertices;


	D3D11_BUFFER_DESC vbDESC;
	vbDESC.Usage = D3D11_USAGE_DEFAULT;
	vbDESC.ByteWidth = sizeof(Vertex)* vertexIndices->size();
	vbDESC.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDESC.CPUAccessFlags = 0;
	vbDESC.MiscFlags = 0;
	vbDESC.StructureByteStride = 0;
	
	for (int i = 0; i < vertexIndices->size(); i+=3){
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
		throw std::runtime_error("Failed to create vertexBuffer");
		return nullptr;
	}

	return vertexBuffer;
}

/*
void AssetManager::setUpBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(Vertex);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	
	deviceContext->IASetVertexBuffers(0, 1, &asset.vertexBuffer, &stride, &offset);

	//Set the index buffer to active in the input assembler so it can be rendered.
	//deviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0); No index buffer for the moment

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

ID3D11Buffer* AssetManager::getVertexBuffer()
{

	return asset.vertexBuffer;

}
*/
//turns a text file into a vector of strings line-by-line
void AssetManager::FileToStrings(std::string file_path, std::vector<std::string> &output)
{
	std::string delimiter = "\n";
	std::ifstream file(file_path);
	if (!file.is_open()){
		printf("Failed to open models.txt\n");
	}
	std::string s((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());


	size_t pos = 0;
	std::string token;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		output.push_back(token);
		s.erase(0, pos + delimiter.length());
	}
};

std::vector<int> AssetManager::StringToIntArray(std::string input)
{
	std::vector<int> output;
	int from = 0;
	
	for (int to = 0; to < input.size(); to++)
	{
		if (input[to] == ','){
			output.push_back(std::stoi(input.substr(from, to - from)));
			from = to+1;
		}
	}
	output.push_back(std::stoi(input.substr(from, input.size() - from)));

	
	return output;
}

AssetManager::AssetManager(ID3D11Device* device_)
{
	device = device_;

	std::vector<std::string> modelNames;
	FileToStrings("Assets/models.txt", modelNames);
	for (int i = 0; i < modelNames.size(); i++)
		LoadModel("Assets/" + modelNames[i]);

	std::vector<std::string> textureNames;
	FileToStrings("Assets/textures.txt", textureNames);
	for (int i = 0; i < textureNames.size(); i++)
		LoadTexture("Assets/" + textureNames[i]);

	std::vector<std::string> renderObjectIDs;
	FileToStrings("Assets/renderObjects.txt", renderObjectIDs);
	for (int i = 0; i < renderObjectIDs.size(); i++)
	{
		std::vector<int> IDs = StringToIntArray(renderObjectIDs[i]);
		CreateRenderObject(IDs[0], IDs[1], IDs[2]);
	}
	int stop = 0;
};


AssetManager::~AssetManager()
{
	//Delete all vertex buffers and textures
};

