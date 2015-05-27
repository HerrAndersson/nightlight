#include "fbxdata.h"

int main(int argc, char** argv) {


	fbxData fbxMaya, fbxBin;

	// Create the FBX SDK manager
	FbxManager* lSdkManager = FbxManager::Create();

	// Create an IOSettings object.
	FbxIOSettings * ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	// Import options determine what kind of data is to be imported.
	// True is the default, but here we’ll set some to true explicitly, and others to false.
	(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_MATERIAL, true);
	(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_TEXTURE, true);
	(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_LINK, false);
	(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_SHAPE, false);
	(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_GOBO, false);
	(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_ANIMATION, false);
	(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);

	// Create an importer.
	FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");

	// Declare the path and filename of the file containing the scene.
	// In this case, we are assuming the file is in the same directory as the executable.
	const char* lFilenameMaya = "box1.fbx";
	const char* lFilenameBin = "box2.fbx";

	// Initialize the importer.
	bool lImportStatusMaya = lImporter->Initialize(lFilenameMaya, -1, lSdkManager->GetIOSettings());
	
	if (!lImportStatusMaya) {
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		exit(-1);
	}
	bool lImportStatusBin = lImporter->Initialize(lFilenameBin, -1, lSdkManager->GetIOSettings());

	if (!lImportStatusBin) {
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		exit(-1);
	}



	// Create a new scene so it can be populated by the imported file.
	FbxScene* lSceneMaya = FbxScene::Create(lSdkManager, "mayaScene");
	FbxScene* lSceneBin = FbxScene::Create(lSdkManager, "binScene");

	// Import the contents of the file into the scene.
	lImporter->Import(lSceneMaya);
	lImporter->Import(lSceneBin);


	
	FbxGeometryElementNormal* lNormalElement;

	FbxVector4 norm2;
	// Add the mesh node to the root node in the scene.
	FbxNode *lRootNodeMaya = lSceneMaya->GetRootNode();
	FbxNode *lRootNodeBin = lSceneBin->GetRootNode();
	FbxNode *lNodeMaya, *lNodeBin;
	FbxMesh *lMeshMaya, *lMeshBin;

	//Iterera över alla noder i fbx-filen
	//Hitta alla meshes i denna nod
	//hitta vertex positioner från mesh och spara undan
	//normaler, uv osv
	//gör detta två gånger och jämför sedan två listor av alla värden

	//maya file
	int childcountMaya = lRootNodeMaya->GetChildCount();
	int childcountBin = lRootNodeBin->GetChildCount();
	for (int i = 0; i < childcountMaya; i++)
	{
		lNodeMaya = lRootNodeMaya->GetChild(i);
		lMeshMaya = lNodeMaya->GetMesh();

		int controlCount = lMeshMaya->GetControlPointsCount();
		for (int j = 0; j < controlCount; j++)
		{
			fbxMaya.vtx.push_back(lMeshMaya->GetControlPointAt(j));
		}

		



		//lNormalElement = lMesh->GetElementNormal();
		//lMesh->GetNormals(lNormalElement);
		
	}

	//bin file
	for (int i = 0; i < childcountBin; i++)
	{
		lNodeBin = lRootNodeBin->GetChild(i);
		lMeshBin = lNodeBin->GetMesh();

		int controlCount = lMeshBin->GetControlPointsCount();
		for (int j = 0; j < controlCount; j++)
		{
			fbxMaya.vtx.push_back(lMeshBin->GetControlPointAt(j));
		}

	}
	//getchar();


	// The file has been imported; we can get rid of the importer.
	lImporter->Destroy();

	return 0;
}

