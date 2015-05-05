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

struct BlendShapeTarget
{
	int MeshTarget;
	std::vector<vec3> points;
	std::vector<vec3> normals;
};

struct TangentData
{
	int LERP, SLERP, Ltest, Stest;
	float ix, iy, ox, oy;
};

struct KeyFrames
{
	MTime keyFrame;					    //Keyframe position on timeline 
	float AnimValue;					//Not necessary weights, but for morph animation it is
	MTime currTime;
};

struct parentData
{
	int numParents;
	//Parent ID
	int numChildren;
	//Child ID
};

struct jointTrans
{
	//Trans
	//Rot
	//JointOrient
	double scale[3];
};

struct skinData
{
	//Object influenced by a skeleton
	//Number points in the object
	//How many influences
	//Skin Weight values
};

struct JointData
{
	std::vector<parentData> parData;
	std::vector<jointTrans> jointTransformations;
	std::vector<skinData> skinD;
};

struct AnimData
{
	MTime animationStart;
	MTime animationEnd;
	int numKeys;

	//std::vector<TangentData> Tdata;
	std::vector<KeyFrames> KeyData;

	//Stuff we get out:
	//Start Animation
	//End Animation
	//Num Keyframes
	//current Frame
	//Value (such as weight and position)
	//Current time in seconds

	//Transform values of everything including joints (If we save them in the bind pose we get the bind pose values as well as Quaternions)
	//Parent/child information as well as how many

	//What are we lacking:
	//Skin Weights (Cannot tell if you get this correctly)
	//
};

struct SceneData
{
	std::vector<material> materials;
	std::vector<MeshData> meshes;
	std::vector<BlendShapeTarget> blendShapes;
	std::vector<cameraData> cameras;
	lightData lights;
	std::vector<AnimData> AnimationData;
};

struct MainHeader{
	int meshCount, blendShapeCount, matCount, camCount, ambientLightSize, areaLightSize, dirLightSize, pointLightSize, spotLightSize, AnimationData;
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
	void extractKeyData(MObject& key, AnimData&);
	void extractJointData(MDagPath path);
	void OutputSkinCluster(MObject& obj);

	void outputTransformData(MObject& obj);
	void outputParentInfo(MObject& obj);
	void OutputBlendShapes(MFnBlendShapeDeformer& fn, MObject& Base, AnimData&);
	void outPutTarget(MObject& target, AnimData&, MObject& Base);

	bool CreateExportFiles(std::string file_path, std::string output_type);
	void CloseExportFiles();

	bool GetMayaFilenamesInDirectory(char *folder_path, std::vector<std::string> &list_to_fill);

	void ProcessScene(const char *file_path);

	void ProcessLevel(const char *file_path);

	bool IdentifyAndExtractScene();
	bool IdentifyAndExtractMeshes();

	bool IdentifyAndExtractLevelInformation();

	bool ExtractMeshData(MFnMesh &mesh, UINT index);

	void ExportLevel();
	void ExportScene();
	void ExportMeshes();

	void splitStringToVector(std::string input, std::vector<std::string> &output, std::string delimiter);
	void fileToStrings(std::string file_path, std::vector<std::string> &output);
};