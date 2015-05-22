#pragma once
#include <vector>
#include <string>
#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>
#include <fstream>

using namespace DirectX;

namespace assetUtility {
	struct MainHeader
	{
		int meshCount, blendShapeCount, matCount, camCount, ambientLightSize, areaLightSize, dirLightSize, pointLightSize, spotLightSize, AnimationData, boneCount, sceneGraph;
	};

	struct Node{
		int parent;
		XMMATRIX transform;
		std::string name;
		int type = 0;
		int mesh;
	};

	struct MeshHeader
	{
		int nameLength, numberPoints, numberNormals, numberCoords, numberFaces;
		bool hasSkeleton;
	};

	struct MatHeader
	{
		int diffuseNameLength, ambientNameLength, specularNameLength, transparencyNameLength, glowNameLength;
	};

	struct WeightedVertex
	{
		XMFLOAT3 position;
		XMFLOAT2 uv;
		XMFLOAT3 normal;
		int boneIndices[4];
		float boneWeigths[4];
	};

	struct WeightedBlendVertex
	{
		XMFLOAT3 position0;
		XMFLOAT2 uv;
		XMFLOAT3 normal0;
		XMFLOAT3 position1;
		XMFLOAT3 position2;
		XMFLOAT3 position3;
		XMFLOAT3 normal1;
		XMFLOAT3 normal2;
		XMFLOAT3 normal3;
		int boneIndices[4];
		float boneWeigths[4];
	};

	struct WeightedPoint{
		XMFLOAT3 position;
		int boneIndices[4];
		float boneWeigths[4];
	};

	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT2 uv;
		XMFLOAT3 normal;
	};

	struct BlendVertex
	{
		XMFLOAT3 position0;
		XMFLOAT2 uv;
		XMFLOAT3 normal0;
		XMFLOAT3 position1;
		XMFLOAT3 position2;
		XMFLOAT3 position3;
		XMFLOAT3 normal1;
		XMFLOAT3 normal2;
		XMFLOAT3 normal3;
	};

	struct Point{
		XMFLOAT3 position;
	};

	struct Keyframe{
		float time;
		XMFLOAT3 trans;
		XMFLOAT4 rot;
	};

	struct Bone{
		int parent;
		XMMATRIX BindPose;
		XMMATRIX invBindPose;
		std::vector<Keyframe> frames;
	};

	struct Animation{
		int length, startFrame;
	};

	struct AmbientLightStruct
	{
		double intensity;
		XMFLOAT3 color;
		XMFLOAT3 pos;
	};

	struct AreaLightStruct
	{
		double intensity;
		XMFLOAT3 color;
		XMFLOAT3 pos;
	};

	struct DirectionalLightStruct
	{
		double intensity;
		XMFLOAT3 color;
		XMFLOAT3 dir;
		XMFLOAT3 pos;
	};

	struct PointLightStruct
	{
		double intensity;
		XMFLOAT3 color;
		XMFLOAT3 pos;
	};

	struct SpotLightStruct
	{
		double intensity;
		XMFLOAT3 color;
		XMFLOAT3 dir;
		XMFLOAT3 pos;

		double coneAngle;
		double penumbraAngle;
		double dropoff;
	};

	struct Model
	{
		~Model() {
			
			pointLights.clear();
		}
		std::string name;
		ID3D11Buffer* vertexBuffer;
		bool hasSkeleton = false;
		bool hasBlendShapes = false;
		int vertexBufferSize;
		std::vector<PointLightStruct> pointLights;
		SpotLightStruct spotLight;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;
		std::vector<Bone> skeleton;

	////specific FBX part
		std::vector<WeightedPoint> points;
		std::vector<Point> purePoints;
		std::vector<XMFLOAT3> normals;
		std::vector<XMFLOAT2> UVs;
		std::vector<XMINT3> vertexIndices;
	};

	struct BlendShape
	{
		int MeshTarget;
		std::vector<XMFLOAT3> points;
		std::vector<XMFLOAT3> normals;
	};

	struct RenderObject
	{
		Model* model;
		ID3D11ShaderResourceView* diffuseTexture = nullptr;
		ID3D11ShaderResourceView* specularTexture = nullptr;
	};

	struct LightData
	{
		std::vector<PointLightStruct> pointLights;
		std::vector<AmbientLightStruct> ambientLights;
		std::vector<AreaLightStruct> areaLights;
		std::vector<DirectionalLightStruct> dirLights;
		std::vector<SpotLightStruct> spotLights;
	};

	struct MaterialData
	{
		XMFLOAT4 diffuse;
		std::string diffuseTextureName;
		XMFLOAT4 specular;
		std::string specularTextureName;
	};

	struct binData{
		std::vector<Model> modelList;
		std::vector<MaterialData> materialList;
		std::vector<Node> sceneGraph;
	};

};
