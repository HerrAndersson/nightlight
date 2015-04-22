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
#include <maya\MLibrary.h>
#include <maya\MFileIO.h>
#include <maya\MGlobal.h>


#include <maya\MDagPath.h>
#include <maya\MFloatPointArray.h>

#include <maya/MFloatMatrix.h>
#include <maya/MMatrix.h>
#include <maya/MFnTransform.h>

#include<maya/MFnIkJoint.h>
#include<maya/MQuaternion.h>
#include<maya/MEulerRotation.h>

#include <maya\MFnMesh.h>
#include <maya/MFnCamera.h>

#include <maya\MFnDependencyNode.h>
#include <maya\MFnAttribute.h>

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
#pragma comment(lib, "Foundation.lib")
#pragma comment(lib, "OpenMaya.lib")
#pragma comment(lib, "OpenMayaAnim.lib")

//___________________________________________________________________________________________
//|																							|
//|										GLOBAL STRUKT-DATA									|
//|_________________________________________________________________________________________|

enum matType{ LAMBERT, BLINN, PHONG };

struct vec3{
	float x, y, z;
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

struct MeshData
{
	MDagPath mesh_path;
	MString name;
	UINT id;

	std::vector<vec3> points;
	std::vector<vec3> normals;
	std::vector<uvSet> uvSets;
	std::vector<face> faces;
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

struct KeyData
{

	//MTime keyFrame;	//Keyframe position on timeline 
	std::vector<vec3> points;		//Keyframes value
	std::vector<vec3> normals;		//Keyframes value
	std::vector<uvSet> uvSets;		//Keyframes value

	//Current Key	//Not sure if needed in a struct nor for this assignment
	//Next Key		//Not sure if needed in a struct nor for this assignment
	//InTangent		//Not sure if needed in a struct nor for this assignment
	//OutTangent	//Not sure if needed in a struct nor for this assignment
	//FrameID
};

struct AnimData
{
	MTime animationStart;
	MTime animationEnd;
	int numKeys;
	MTime keyFrame;					//Keyframe position on timeline 

	std::vector<KeyData> KeyFrames;


	//Stuff for later:
	//Joints, position, orientations data
	//SkeletonHierachy
	//Weights (start and count)
	//String Name
	//ParentID
	//Flags
	//Start Index

	//For morph animation:
	//Vertex data in need of change
	//Position of model
	//Position to be interpolated into (as well as maybe: Second normal to be interpolated into)
	//Frame Rate (How many Frame rates for each animation)
	//Frame time (Uses to to figure out how many seconds for each frame rate)
	//Total Animation Time
	//Current Animation Time
	//Num animated components
};

struct SceneData
{
	std::vector<material> materials;
	std::vector<MeshData> meshes;
	std::vector<cameraData> cameras;
	std::vector<lightData> lights;
	std::vector<AnimData> AnimationData;
};

struct MainHeader{
	int meshCount, matCount, camCount, lightCount;
};

struct MeshHeader
{
	int nameLength, numberPoints, numberNormals, numberCoords, numberFaces;
};

struct MatHeader{
	int diffuseNameLength, ambientNameLength, specularNameLength, transparencyNameLength, glowNameLength;
};

/* överflödig
struct camHeader
{

};
*/

struct LightHeader{
	int ambientLightSize, areaLightSize, dirLightSize, pointLightSize, spotLightSize;
};

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
	bool InitializeMaya();
	void CleanUpMaya();

	void extractLight(MObject& mObj);
	void extractColor(Color& tempcolor, MFnDependencyNode& fn, MString name);
	void extractCamera(MObject& obj);
	void extractKeyData(MObject& key);

	void outputTransformData(MObject& obj);
	void outputParentInfo(MObject& obj);
	bool outputParentInfo(MDagPath& bone, int sf, int ef);

	bool CreateExportFiles(std::string file_path);
	void CloseExportFiles();

	bool GetMayaFilenamesInDirectory(char *folder_path, std::vector<std::string> &list_to_fill);

	void ProcessScene(const char *file_path);

	bool IdentifyAndExtractScene();
	bool IdentifyAndExtractMeshes();

	bool ExtractMeshData(MFnMesh &mesh, UINT index);

	void ExportScene();
	void ExportMeshes();
};