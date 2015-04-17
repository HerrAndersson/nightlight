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

	std::vector<Vertex> vertices;
	vertices.reserve(meshHeader.numberPoints);


	std::string name;
	std::vector<XMFLOAT3> points;
	std::vector<XMFLOAT3> normals;
	std::vector<XMFLOAT2> UVs;
	std::vector<XMINT3> vertexIndices;

	name.reserve(meshHeader.nameLength);
	points.reserve(meshHeader.numberPoints);
	normals.reserve(meshHeader.numberNormals);
	UVs.reserve(meshHeader.numberCoords);
	vertexIndices.reserve(meshHeader.numberFaces);

	infile.read((char*)&meshHeader, sizeof(MeshHeader));
	infile.read((char*)&name, meshHeader.nameLength);
	infile.read((char*)&points, meshHeader.numberPoints*sizeof(XMFLOAT3));
	infile.read((char*)&normals, meshHeader.numberNormals*sizeof(XMFLOAT3));
	infile.read((char*)&UVs, meshHeader.numberCoords*sizeof(XMFLOAT2));
	infile.read((char*)&vertexIndices, sizeof(XMINT3));
	infile.close();

	RenderObject asset;
	asset.vertexBuffer = CreateVertexBuffer(&points, &normals, &UVs, &vertexIndices);


	return nullptr;
}

ID3D11Buffer* AssetManager::CreateVertexBuffer(std::vector<XMFLOAT3> *points, std::vector<XMFLOAT3> *normals, std::vector<XMFLOAT2> *UVs, std::vector<XMINT3> *vertexIndices){

	std::vector<Vertex> vertices;
	D3D11_BUFFER_DESC vbDESC;
	vbDESC.Usage = D3D11_USAGE_DEFAULT;
	vbDESC.ByteWidth = sizeof(Vertex)* 3 * vertexIndices->size();
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

	OutputDebugString("Success");
	return nullptr;
}

AssetManager::AssetManager(ID3D11Device* device)
{
	device = this->device;
}


AssetManager::~AssetManager()
{
}
