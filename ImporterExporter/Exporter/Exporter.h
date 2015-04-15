// Jonas Petersson
// jnp@bth.se
// 14-03-31

#pragma once

#define NT_APP //extern Maya-application (inget plug-in | MLibrary based application).

//___________________________________________________________________________________________
//|																							|
//|										INKLUDERA											|
//|_________________________________________________________________________________________|

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
#include<maya/MFnTransform.h>

#include <maya\MFnMesh.h>
#include<maya/MFnCamera.h>

#include <maya\MFnDependencyNode.h>
#include <maya\MFnAttribute.h>

#include <maya/MItDependencyNodes.h>

#include <maya\MItDag.h>
#include <maya\MItMeshPolygon.h>
#include <maya\MPlug.h>

#include<maya/MFnLambertShader.h>
#include<maya/MFnBlinnShader.h>
#include<maya/MFnPhongShader.h>


#include <maya/MFnLight.h>

#include <maya/MFnDagNode.h>
#include <maya/MColor.h>

#include <maya/MFnPointLight.h>
#include <maya/MFnAmbientLight.h>
#include <maya/MFnDirectionalLight.h>
#include <maya/MFnSpotLight.h>
#include <maya/MFnAreaLight.h>


#pragma comment(lib, "Foundation.lib")
#pragma comment(lib, "OpenMaya.lib")

//___________________________________________________________________________________________
//|																							|
//|										GLOBAL STRUKT-DATA									|
//|_________________________________________________________________________________________|

enum matType{ LAMBERT, BLINN, PHONG };

struct uvSet
{
	MFloatArray Us;
	MFloatArray Vs;
	MFloatVectorArray tangents;
	MFloatVectorArray binormals;
};

struct MeshData
{
	MDagPath mesh_path;
	MString name;
	UINT id;

	MFloatPointArray points;
	MFloatVectorArray normals;
	std::vector<uvSet> uvSets;
};

struct Color
{
	float r, g, b, a;
	std::string texfileInternal,texfileExternal;
};

struct material
{
	// 0 = lambert, 1 = blinn, 2 = phong
	int type;
	Color ambient, diffuse, specular, transparency, glow;
};

struct cameraData
{
	MFloatMatrix projectionMatrix;
	MFloatVector upVector;
	MFloatVector viewDirection;

};

struct lightData
{
	MFloatVector ambientLight;
	MFloatVector areaLight;
	MFloatVector directionalLight;
	MFloatVector pointLight;
	MFloatVector spotLight;
};

struct SceneData
{
	std::vector<material> materials;
	std::vector<MeshData> meshes;
	std::vector<cameraData> cameras;
	std::vector<lightData> lights;
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
		SceneData scene_;

	private:
		bool InitializeMaya();
		void CleanUpMaya();
		void extractLight (MObject& mObj);
		void extractColor(Color& tempcolor, MFnDependencyNode& fn, MString name);
		void extractCamera(MObject& obj);

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