// Jonas Petersson
// jnp@bth.se
// 14-03-31

#pragma once

#define NT_APP //extern Maya-application (inget plug-in | MLibrary based application).

//___________________________________________________________________________________________
//|																							|
//|										INKLUDERA											|
//|_________________________________________________________________________________________|

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include <maya\MLibrary.h>
#include <maya\MFileIO.h>
#include <maya\MGlobal.h>


#include <maya\MDagPath.h>
#include <maya\MDagPathArray.h>
#include <maya\MFloatPointArray.h>

#include <maya/MFloatMatrix.h>
#include <maya/MMatrix.h>
#include <maya/MFnTransform.h>

#include<maya/MFnIkJoint.h>
#include<maya/MQuaternion.h>
#include<maya/MEulerRotation.h>
#include<maya/MFnBlendShapeDeformer.h>
#include<maya/MItGeometry.h>

#include <maya\MFnMesh.h>
#include <maya/MFnCamera.h>

#include <maya\MFnDependencyNode.h>
#include <maya\MFnAttribute.h>
#include <maya\MItDependencyGraph.h>
#include <maya\MFnSkinCluster.h>
#include <maya\MFnMatrixData.h>

#include <maya/MItDependencyNodes.h>

#include <maya\MItDag.h>
#include <maya\MItMeshPolygon.h>
#include <maya\MPlug.h>

#include <maya/MFnLambertShader.h>
#include <maya/MFnBlinnShader.h>
#include <maya/MFnPhongShader.h>


#include <maya/MFnLight.h>

#include <maya/MFnDagNode.h>
#include <maya/MColor.h>

#include <maya/MFnPointLight.h>
#include <maya/MFnAmbientLight.h>
#include <maya/MFnDirectionalLight.h>
#include <maya/MFnSpotLight.h>
#include <maya/MFnAreaLight.h>

#include <maya/MTime.h>
#include <maya/MFnAnimCurve.h>
#include <maya/MAnimControl.h>

#include <maya/MQuaternion.h>
#include <maya/MEulerRotation.h>
#include <maya/MStatus.h>
#pragma comment(lib, "Foundation.lib")
#pragma comment(lib, "OpenMaya.lib")
#pragma comment(lib, "OpenMayaAnim.lib")

//___________________________________________________________________________________________
//|																							|
//|										GLOBAL STRUKT-DATA									|
//|_________________________________________________________________________________________|

static const float EPS = 0.1f;

enum matType{ LAMBERT, BLINN, PHONG };

struct vec3{
	float x, y, z;
};

struct point{
	float x, y, z;
	int boneIndices[4];
	float boneWeigths[4];
};

struct vec2{
	float u, v;
};

struct uvSet
{
	std::vector<vec2> UVs;
	MFloatVectorArray tangents;
	MFloatVectorArray binormals;
};


struct faceIndices
{
	int pointID, normalID;
	std::vector<int> texCoordsID;
};

struct face
{
	faceIndices verts[3];
};

struct Joint
{
	int parent;
	MFloatVector LocalTx;
	MFloatVector GlobalTx;
	MFloatVector invBindPose;
	Joint(int par, MFloatVector &data){
		parent = par;
		invBindPose = data;
	}
	Joint(){
		parent = 0;
	}
};

struct MeshData
{
	MDagPath mesh_path;
	MString name;
	UINT id;
	bool hasSkeleton = false;

	std::vector<point> points;
	std::vector<vec3> purepoints;
	std::vector<vec3> normals;
	std::vector<uvSet> uvSets;
	std::vector<face> faces;
	std::vector<Joint> skeleton;
};

struct Color
{
	float r, g, b, a;
	std::string texfileInternal, texfileExternal;
};

struct material
{
	// 0 = lambert, 1 = blinn, 2 = phong
	int type;
	Color ambient, diffuse, specular, transparency, glow;
};

struct cameraData
{
	float verticalFieldOfView, aspectRatio, nearClippingPlane, farClippingPlane;
	MFloatVector position, viewDirection, upVector;
	MFloatMatrix projectionMatrix;
	MMatrix transformMatrix;
};



struct ambientLightStruct
{
	double intensity;
	MFloatVector color;
	MFloatVector pos;
};

struct areaLightStruct
{
	double intensity;
	MFloatVector color;
	MFloatVector pos;
};

struct directionalLightStruct
{
	double intensity;
	MFloatVector color;
	MFloatVector dir;
	MFloatVector pos;
};

struct pointLightStruct
{
	double intensity;
	MFloatVector color;
	MFloatVector pos;
};

struct spotLightStruct
{
	double intensity;
	MFloatVector color;
	double coneAngle;
	double penumbraAngle;
	double dropoff;
	MFloatVector dir;
	MFloatVector pos;
};


struct lightData
{
	std::vector<pointLightStruct> pointLights;
	std::vector<ambientLightStruct> ambientLights;
	std::vector<areaLightStruct> areaLights;
	std::vector<directionalLightStruct> dirLights;
	std::vector<spotLightStruct> spotLights;
};

struct WeightFrame
{
	float weight;
	int currentFrame;	//Key's frame in the whole animation
};

struct BlendShapeTarget
{
	int MeshTarget;
	std::vector<vec3> points;
	std::vector<vec3> normals;
	std::vector<WeightFrame> WeightF;
};

struct skinData
{
	int points;
	int influences;
};

struct Keyframe{
	float time;
	MFloatVector trans;
	MFloatVector rot;
};

struct Bone{
	int parent;
	MFloatMatrix invBindPose;
	std::vector<Keyframe> frames;
};

struct Animation{
	int length, startFrame;
};

struct Node{
	int parent;
	int type=0;
	int mesh;
	MFloatMatrix transform;
	std::string name;
};

struct SceneData
{
	std::vector<Node> sceneGraph;
	std::vector<material> materials;
	std::vector<MeshData> meshes;
	std::vector<BlendShapeTarget> blendShapes;
	std::vector<cameraData> cameras;
	std::vector<Animation> animations;
	std::vector<Bone> skeleton;
	lightData lights;
};

struct MainHeader{
	int meshCount, blendShapeCount, matCount, camCount, ambientLightSize, areaLightSize, dirLightSize, pointLightSize, spotLightSize, AnimationData, boneCount, sceneGraph;
};

struct MeshHeader
{
	int nameLength, numberPoints, numberNormals, numberCoords, numberFaces;
	bool hasSkeleton;
};

struct MatHeader{
	int diffuseNameLength, ambientNameLength, specularNameLength, transparencyNameLength, glowNameLength;
};

/* överflödig
struct camHeader
{

};
*/

//___________________________________________________________________________________________
//|																							|
//|										EXPORTERKLASS										|
//|_________________________________________________________________________________________|

class Exporter
{
public:
	Exporter();
	~Exporter();

	void StartExporter(std::string directory_path);

private:
	std::fstream export_stream_;
	std::ofstream outfile;
	std::ifstream infile;
	SceneData scene_;

private:
	std::vector<std::string> levelGameObjectTypes;
	std::vector<std::string> formattedLevelData;
	bool isAssetListLoaded = false;

	bool InitializeMaya();
	void CleanUpMaya();
	void extractLight(MObject& mObj);
	void extractColor(Color& tempcolor, MFnDependencyNode& fn, MString name);
	void extractCamera(MObject& obj);
	void extractJointData(MDagPath path);
	void OutputSkinCluster(MObject& obj);
	void RecursiveJointExtraction(MFnTransform& joint, int parentIndex);

	void OutputBlendShapes(MFnBlendShapeDeformer& fn, MObject& Base);
	void outPutTarget(MObject& target, MObject& Base);

	bool CreateExportFiles(std::string file_path);
	void CloseExportFiles();

	bool GetMayaFilenamesInDirectory(char *folder_path, std::vector<std::string> &list_to_fill);

	void ProcessScene(const char *file_path);

	bool IdentifyAndExtractScene();
	bool IdentifyAndExtractMeshes();

	bool ExtractMeshData(MFnMesh &mesh, UINT index);

	void ExportScene();
	void ExportMeshes();
	void splitStringToVector(std::string input, std::vector<std::string> &output, std::string delimiter);
	void fileToStrings(std::string file_path, std::vector<std::string> &output);
	void createSceneGraph(MFnDagNode& path, int parentIndex);
};