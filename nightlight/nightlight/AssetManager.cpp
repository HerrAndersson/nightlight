#include "AssetManager.h"

RenderObject* AssetManager::LoadRenderObject(std::string file_path){
	std::ifstream infile;
	infile.open(file_path.c_str(), std::ifstream::binary);
	if (!infile.is_open())
	{
		std::string outputstring = file_path + " not found.\n";
		OutputDebugString(outputstring.c_str());
		return nullptr;
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

			infile.seekg(16 + matHeader.ambientNameLength, std::ios::cur);

			infile.seekg(16 + matHeader.diffuseNameLength, std::ios::cur);

			infile.seekg(16 + matHeader.specularNameLength, std::ios::cur);

			infile.seekg(16 + matHeader.transparencyNameLength, std::ios::cur);

			infile.seekg(16 + matHeader.glowNameLength, std::ios::cur);
		}

	}


	//light header file read
	LightHeader lightHeader;
	infile.read((char*)&lightHeader, sizeof(LightHeader));

	lightData tempLightStorage;

	std::vector<XMFLOAT3> colors;
	std::vector<XMFLOAT3> positions;
	std::vector<XMFLOAT3> directions;
	std::vector<double> intensity;

	std::vector<double> coneAngle;
	std::vector<double> dropoff;
	std::vector<double> penumbraAngle;

	colors.resize(16);
	intensity.resize(8);
	positions.resize(16);
	directions.resize(16);
	coneAngle.resize(8);
	dropoff.resize(8);
	penumbraAngle.resize(8);

	for (int a = 0; a < lightHeader.ambientLightSize; a++)
	{
		ambientLightStruct ambiLights;

		infile.read((char*)colors.data(), 16);
		infile.read((char*)intensity.data(), 8);
		infile.read((char*)positions.data(), 16);
		ambiLights.color = colors;
		ambiLights.intensity = intensity;
		ambiLights.pos = positions;

		tempLightStorage.ambientLights.push_back(ambiLights);

	}
	for (int a = 0; a < lightHeader.areaLightSize; a++)
	{
		areaLightStruct areaLights;
		
		infile.read((char*)colors.data(), 16);
		infile.read((char*)intensity.data(), 8);
		infile.read((char*)positions.data(), 16);
		areaLights.color = colors;
		areaLights.intensity = intensity;
		areaLights.pos = positions;

		tempLightStorage.areaLights.push_back(areaLights);
	}
	for (int a = 0; a < lightHeader.dirLightSize; a++)
	{
		directionalLightStruct dirLights;

		infile.read((char*)colors.data(), 16);
		infile.read((char*)directions.data(), 16);
		infile.read((char*)intensity.data(), 8);
		infile.read((char*)positions.data(), 16);
		dirLights.color = colors;
		dirLights.intensity = intensity;
		dirLights.pos = positions;
		dirLights.dir = directions;

		tempLightStorage.dirLights.push_back(dirLights);

	}
	for (int a = 0; a < lightHeader.pointLightSize; a++)
	{
		pointLightStruct pointLights;

		infile.read((char*)colors.data(), 16);
		infile.read((char*)intensity.data(), 8);
		infile.read((char*)positions.data(), 16);
		pointLights.color = colors;
		pointLights.intensity = intensity;
		pointLights.pos = positions;

		tempLightStorage.pointLights.push_back(pointLights);

	}
	for (int a = 0; a < lightHeader.spotLightSize; a++)
	{
		spotLightStruct spotLights;

		infile.read((char*)colors.data(), 16);
		infile.read((char*)directions.data(), 16);
		infile.read((char*)intensity.data(), 8);
		infile.read((char*)positions.data(), 16);

		infile.read((char*)coneAngle.data(), 8);
		infile.read((char*)dropoff.data(), 8);
		infile.read((char*)penumbraAngle.data(), 8);

		spotLights.color = colors;
		spotLights.dir = directions;
		spotLights.intensity = intensity;
		spotLights.pos = positions;

		spotLights.coneAngle = coneAngle;
		spotLights.dropoff = dropoff;
		spotLights.penumbraAngle = penumbraAngle;

		tempLightStorage.spotLights.push_back(spotLights);

	}
	




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
		OutputDebugString("Failed to create vertexBuffer");
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
