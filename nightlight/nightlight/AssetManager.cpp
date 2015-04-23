#include "AssetManager.h"

RenderObject* AssetManager::LoadRenderObject(std::string file_path){
	std::ifstream infile;
	infile.open(file_path.c_str(), std::ifstream::binary);
	if (!infile.is_open())
	{
		std::string outputstring = file_path + " not found.\n";
		throw std::runtime_error(outputstring.c_str());
		return nullptr;
	}
	char test;
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
	vertexIndices.resize(meshHeader.numberFaces*3);

	infile.read((char*)name.data(), meshHeader.nameLength);
	infile.read((char*)points.data(), meshHeader.numberPoints*sizeof(XMFLOAT3));
	infile.read((char*)normals.data(), meshHeader.numberNormals*sizeof(XMFLOAT3));
	infile.read((char*)UVs.data(), meshHeader.numberCoords*sizeof(XMFLOAT2));
	infile.read((char*)vertexIndices.data(), meshHeader.numberFaces*sizeof(XMINT3)*3);

	for (int i = 0; i < mainHeader.matCount;i++)
	{
		if (i == 0){
			MatHeader matHeader;
			infile.read((char*)&matHeader, sizeof(MatHeader));

			infile.seekg(16+matHeader.ambientNameLength, std::ios::cur);

			Texture *diffuseTexture = new Texture();
			infile.read((char*)&asset.diffuse, 16);
			diffuseTexture->textureName.resize(matHeader.diffuseNameLength);
			infile.read((char*)diffuseTexture->textureName.data(), matHeader.diffuseNameLength);
			asset.diffuseTexture = diffuseTexture;

			Texture *specularTexture = new Texture();
			infile.read((char*)&asset.specular, 16);
			specularTexture->textureName.resize(matHeader.specularNameLength);
			infile.read((char*)specularTexture->textureName.data(), matHeader.specularNameLength);
			asset.specularTexture = specularTexture;

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


	infile.read((char*)&test, 0);
	lightData tempLightStorage;
	tempLightStorage.ambientLights.resize(mainHeader.ambientLightSize);
	tempLightStorage.areaLights.resize(mainHeader.areaLightSize);
	tempLightStorage.spotLights.resize(mainHeader.spotLightSize);
	tempLightStorage.dirLights.resize(mainHeader.dirLightSize);
	tempLightStorage.pointLights.resize(mainHeader.pointLightSize);

	if (mainHeader.ambientLightSize)
		infile.read((char*)tempLightStorage.ambientLights.data(), mainHeader.ambientLightSize*sizeof(ambientLightStruct));
	if (mainHeader.areaLightSize)
		infile.read((char*)tempLightStorage.areaLights.data(), mainHeader.areaLightSize*sizeof(areaLightStruct));
	if (mainHeader.dirLightSize)
		infile.read((char*)tempLightStorage.dirLights.data(), mainHeader.dirLightSize* sizeof(directionalLightStruct));
	if (mainHeader.pointLightSize)
		infile.read((char*)tempLightStorage.pointLights.data(), mainHeader.pointLightSize* sizeof(pointLightStruct));
	if (mainHeader.spotLightSize)
		infile.read((char*)tempLightStorage.spotLights.data(), mainHeader.spotLightSize* sizeof(spotLightStruct));

	std::vector<Vertex> vertices;
	vertices.reserve(meshHeader.numberPoints);
	//RenderObject asset;
	asset.vertexBuffer = CreateVertexBuffer(&points, &normals, &UVs, &vertexIndices);
	asset.vertexBufferSize = vertexIndices.size();
	infile.close();

	//RenderObject* object; return object;
	
	return &asset;
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

AssetManager::AssetManager(ID3D11Device* device_)
{
	device = device_;

	//loop för att läsa alla assets

}


AssetManager::~AssetManager()
{
}
