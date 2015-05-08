#include "AssetManager.h"

AssetManager::AssetManager()
{
	
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

void AssetManager::LoadModel(string file_path, Model& model){
	


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
	
	for (int i = 0; i < mainHeader.meshCount; i++){
		if (i == 0){
			MeshHeader meshHeader;
			infile.read((char*)&meshHeader, sizeof(MeshHeader));


			name.resize(meshHeader.nameLength);
			if (meshHeader.hasSkeleton)
				model.points.resize(meshHeader.numberPoints);
			else
				model.purePoints.resize(meshHeader.numberPoints);
			model.normals.resize(meshHeader.numberNormals);
			model.UVs.resize(meshHeader.numberCoords);
			model.vertexIndices.resize(meshHeader.numberFaces * 3);
			model.hasSkeleton = meshHeader.hasSkeleton;


			infile.read((char*)name.data(), meshHeader.nameLength);
			model.name = name;
			if (meshHeader.hasSkeleton)
				infile.read((char*)model.points.data(), meshHeader.numberPoints*sizeof(WeightedPoint));
			else
				infile.read((char*)model.purePoints.data(), meshHeader.numberPoints*sizeof(Point));
			infile.read((char*)model.normals.data(), meshHeader.numberNormals*sizeof(XMFLOAT3));
			infile.read((char*)model.UVs.data(), meshHeader.numberCoords*sizeof(XMFLOAT2));
			infile.read((char*)model.vertexIndices.data(), meshHeader.numberFaces*sizeof(XMINT3) * 3);
		}
		else{
			MeshHeader meshHeader;
			infile.read((char*)&meshHeader, sizeof(MeshHeader));
			if (meshHeader.hasSkeleton)
			{
				infile.seekg(meshHeader.nameLength, ios::cur);
				infile.seekg(meshHeader.numberPoints*sizeof(WeightedPoint), ios::cur);
				infile.seekg(meshHeader.numberNormals*sizeof(XMFLOAT3), ios::cur);
				infile.seekg(meshHeader.numberCoords*sizeof(XMFLOAT2), ios::cur);
				infile.seekg(meshHeader.numberFaces*sizeof(XMINT3) * 3, ios::cur);
			}
			else
			{
				infile.seekg(meshHeader.nameLength, ios::cur);
				infile.seekg(meshHeader.numberPoints*sizeof(Point), ios::cur);
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

			infile.read((char*)&model.diffuse, 16);
			infile.seekg(matHeader.diffuseNameLength, ios::cur);

			infile.read((char*)&model.specular, 16);
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

	model.pointLights.resize(mainHeader.pointLightSize);

	if (mainHeader.ambientLightSize)
		infile.seekg(mainHeader.ambientLightSize*sizeof(AmbientLightStruct), ios::cur);
	if (mainHeader.areaLightSize)
		infile.seekg(mainHeader.areaLightSize*sizeof(AreaLightStruct), ios::cur);
	if (mainHeader.dirLightSize)
		infile.seekg(mainHeader.dirLightSize* sizeof(DirectionalLightStruct), ios::cur);
	if (mainHeader.pointLightSize)
		infile.read((char*)model.pointLights.data(), mainHeader.pointLightSize* sizeof(PointLightStruct));
	if (mainHeader.spotLightSize){
		infile.read((char*)&model.spotLight, sizeof(SpotLightStruct));
		infile.seekg(mainHeader.spotLightSize - 1 * sizeof(SpotLightStruct), ios::cur);
	}

	infile.seekg(mainHeader.camCount * 52, ios::cur);

	/*
	for (int i = 0; i < mainHeader.camCount; i++){
	läs kameror
	}
	*/
	if (mainHeader.blendShapeCount == 3)
		model.hasBlendShapes = true;

	std::vector<BlendShape> blendShapes;
	for (int i = 0; i < mainHeader.blendShapeCount; i++){
		BlendShape blendShape;
		infile.read((char*)&blendShape.MeshTarget, 4);
		blendShape.points.resize(model.points.size() + model.purePoints.size());
		infile.read((char*)blendShape.points.data(), blendShape.points.size()*sizeof(XMFLOAT3));
		blendShape.normals.resize(model.normals.size());
		infile.read((char*)blendShape.normals.data(), blendShape.normals.size()*sizeof(XMFLOAT3));
		blendShapes.push_back(blendShape);
	}

	model.skeleton.resize(mainHeader.boneCount);

	for (int i = 0; i < mainHeader.boneCount; i++){
		infile.read((char*)&model.skeleton[i].parent, 4);
		infile.read((char*)&model.skeleton[i].BindPose, 64);
		infile.read((char*)&model.skeleton[i].invBindPose, 64);

		int frames;
		infile.read((char*)&frames, 4);

		model.skeleton[i].frames.resize(frames);

		infile.read((char*)model.skeleton[i].frames.data(), sizeof(Keyframe)*frames);
	}

	infile.close();
}

void AssetManager::CreateRenderObject(int modelID, int diffuseID, int specularID)
{
	RenderObject* renderObject = new RenderObject();
	renderObject->model = models[modelID];

	if (diffuseID != -1)
		renderObject->diffuseTexture = textures[diffuseID];
	if (specularID != -1)
		renderObject->specularTexture = textures[specularID];

	renderObjects.push_back(renderObject);
}


RenderObject* AssetManager::GetRenderObject(int id)
{
	return renderObjects[id];
}