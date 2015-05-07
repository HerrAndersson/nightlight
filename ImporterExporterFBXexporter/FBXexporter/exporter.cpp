#include <fbxsdk.h>
#include <fbxsdk\fileio\fbxiosettings.h>

#pragma comment(lib, "libfbxsdk.lib")

int main(int argc, char** argv) {

	//Create the FBX SDK manager
	FbxManager* lSdkManager = FbxManager::Create();

	//Create an IOSettings object.
	FbxIOSettings * ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	//Configure the FbxIOSettings object
	(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_MATERIAL, true);
	(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_TEXTURE, true);
	(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_LINK, false);
	(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_SHAPE, true);
	(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_GOBO, false);
	(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_ANIMATION, true);
	(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);

	bool lEmbedMedia = true;
	(*(lSdkManager->GetIOSettings())).SetBoolProp(EXP_FBX_EMBEDDED, lEmbedMedia);


	FbxScene* lScene = FbxScene::Create(lSdkManager, "newScene");
	
	//Export the contents of the file.	
	//Create an exporter.
	FbxExporter* lExporter = FbxExporter::Create(lSdkManager, "");

	// Declare the path and filename of the file to which the scene will be exported.
	const char* lFilenameOut = "file.fbx";
		
	// Initialize the exporter.
	bool lExportStatus = lExporter->Initialize(lFilenameOut, -1, lSdkManager->GetIOSettings());

	if (!lExportStatus) {
		printf("Call to FbxExporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lExporter->GetStatus().GetErrorString());
		return false;
	}

	// Create a node for our mesh in the scene.
	FbxNode* lMeshNode = FbxNode::Create(lScene, "meshNode");

	// Create a mesh.
	FbxMesh* lMesh = FbxMesh::Create(lScene, "mesh");

	// Set the node attribute of the mesh node.
	lMeshNode->SetNodeAttribute(lMesh);

	// Add the mesh node to the root node in the scene.
	FbxNode *lRootNode = lScene->GetRootNode();
	lRootNode->AddChild(lMeshNode);

	// Define the eight corners of the cube.
	// The cube spans from
	//    -50 to  50 along the X axis
	//      0 to 100 along the Y axis
	//    -50 to  50 along the Z axis
	
	FbxVector4 vertex0(-50, 0, 50);
	FbxVector4 vertex1(50, 0, 50);
	FbxVector4 vertex2(50, 100, 50);
	FbxVector4 vertex3(-50, 100, 50);
	FbxVector4 vertex4(-50, 0, -50);
	FbxVector4 vertex5(50, 0, -50);
	FbxVector4 vertex6(50, 100, -50);
	FbxVector4 vertex7(-50, 100, -50);

	// Initialize the control point array of the mesh.
	lMesh->InitControlPoints(24);
	FbxVector4* lControlPoints = lMesh->GetControlPoints();

	// Define each face of the cube.
	// Face 1
	lControlPoints[0] = vertex0;
	lControlPoints[1] = vertex1;
	lControlPoints[2] = vertex2;
	lControlPoints[3] = vertex3;
	// Face 2
	lControlPoints[4] = vertex1;
	lControlPoints[5] = vertex5;
	lControlPoints[6] = vertex6;
	lControlPoints[7] = vertex2;
	// Face 3
	lControlPoints[8] = vertex5;
	lControlPoints[9] = vertex4;
	lControlPoints[10] = vertex7;
	lControlPoints[11] = vertex6;
	// Face 4
	lControlPoints[12] = vertex4;
	lControlPoints[13] = vertex0;
	lControlPoints[14] = vertex3;
	lControlPoints[15] = vertex7;
	// Face 5
	lControlPoints[16] = vertex3;
	lControlPoints[17] = vertex2;
	lControlPoints[18] = vertex6;
	lControlPoints[19] = vertex7;
	// Face 6
	lControlPoints[20] = vertex1;
	lControlPoints[21] = vertex0;
	lControlPoints[22] = vertex4;
	lControlPoints[23] = vertex5;
	
	lExporter->Export(lScene);

	//Get rid of objects
	lExporter->Destroy();

	return 0;
}

