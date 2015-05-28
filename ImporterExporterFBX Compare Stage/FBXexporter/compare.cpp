#include "fbxdata.h"
//#define DELTA 0.0000000000001
//#define EQUAL(A,B) (abs((A)-(B)) < DELTA) ? true:false

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
	FbxImporter* lImporterMaya = FbxImporter::Create(lSdkManager, "");
	FbxImporter* lImporterBin = FbxImporter::Create(lSdkManager, "");

	// Declare the path and filename of the file containing the scene.
	// In this case, we are assuming the file is in the same directory as the executable.
	const char* lFilenameMaya = "box1.fbx";
	const char* lFilenameBin = "box1.fbx";

	// Initialize the importer.
	bool lImportStatusMaya = lImporterMaya->Initialize(lFilenameMaya, -1, lSdkManager->GetIOSettings());
	
	if (!lImportStatusMaya) {
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lImporterMaya->GetStatus().GetErrorString());
		exit(-1);
	}
	bool lImportStatusBin = lImporterBin->Initialize(lFilenameBin, -1, lSdkManager->GetIOSettings());

	if (!lImportStatusBin) {
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lImporterBin->GetStatus().GetErrorString());
		exit(-1);
	}



	// Create a new scene so it can be populated by the imported file.
	FbxScene* lSceneMaya = FbxScene::Create(lSdkManager, "mayaScene");
	FbxScene* lSceneBin = FbxScene::Create(lSdkManager, "binScene");

	// Import the contents of the file into the scene.
	lImporterMaya->Import(lSceneMaya);
	lImporterBin->Import(lSceneBin);


	
	FbxGeometryElementNormal* lNormalElement;

	FbxVector4 norm2;
	// Add the mesh node to the root node in the scene.
	FbxNode *lRootNodeMaya = lSceneMaya->GetRootNode();
	FbxNode *lRootNodeBin = lSceneBin->GetRootNode();
	FbxNode *lNodeMaya, *lNodeBin;
	FbxMesh *lMeshMaya, *lMeshBin;
	FbxCamera *lCameraMaya, *lCameraBin;
	FbxLight *lLightMaya, *lLightBin;

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

		FbxGeometryElementNormal* lNormalElementMaya = lMeshMaya->GetElementNormal();
		for (int lVertexIndex = 0; lVertexIndex < controlCount; lVertexIndex++)
		{
			int lNormalIndex = 0;

			if (lNormalElementMaya->GetReferenceMode() == FbxGeometryElement::eDirect)
				lNormalIndex = lVertexIndex;
			if (lNormalElementMaya->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				lNormalIndex = lNormalElementMaya->GetIndexArray().GetAt(lVertexIndex);

			FbxVector4 lNormal = lNormalElementMaya->GetDirectArray().GetAt(lNormalIndex);
			fbxMaya.norm.push_back(lNormal);
		}

		//iterating over uv sets
		FbxStringList lUvSetNameList;
		lMeshMaya->GetUVSetNames(lUvSetNameList);
		for (int lUVsetIndex = 0; lUVsetIndex < lUvSetNameList.GetCount(); lUVsetIndex++)
		{
			//get the incexed uv set
			const char* lUVSetName = lUvSetNameList.GetStringAt(lUVsetIndex);
			const FbxGeometryElementUV* lUVElement = lMeshMaya->GetElementUV(lUVSetName);
			
			if (!lUVElement)
				continue;

			if (lUVElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
				lUVElement->GetMappingMode() != FbxGeometryElement::eByControlPoint)
				return 0;

			//index array for holding the index referenced to uv data
			bool lUseIndex = lUVElement->GetReferenceMode() != FbxGeometryElement::eDirect;
			int lIndexCount = (lUseIndex) ? lUVElement->GetIndexArray().GetCount() : 0;

			const int lPolyCount = lMeshMaya->GetPolygonCount();
			for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
			{
				// build the max index array that we need to pass into MakePoly
				const int lPolySize = lMeshMaya->GetPolygonSize(lPolyIndex);
				for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
				{
					FbxVector2 lUVValue;

					//get the index of the current vertex in control points array
					int lPolyVertIndex = lMeshMaya->GetPolygonVertex(lPolyIndex, lVertIndex);

					//the UV index depends on the reference mode
					int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyVertIndex) : lPolyVertIndex;

					lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

					fbxMaya.uv.push_back(lUVValue);

				}
			}
		}


		
	}

	//bin file
	for (int i = 0; i < childcountBin; i++)
	{
		lNodeBin = lRootNodeBin->GetChild(i);
		lMeshBin = lNodeBin->GetMesh();

		int controlCount = lMeshBin->GetControlPointsCount();
		for (int j = 0; j < controlCount; j++)
		{
			fbxBin.vtx.push_back(lMeshBin->GetControlPointAt(j));
		}

		FbxGeometryElementNormal* lNormalElementBin = lMeshBin->GetElementNormal();
		for (int lVertexIndex = 0; lVertexIndex < controlCount; lVertexIndex++)
		{
			int lNormalIndex = 0;

			if (lNormalElementBin->GetReferenceMode() == FbxGeometryElement::eDirect)
				lNormalIndex = lVertexIndex;
			if (lNormalElementBin->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				lNormalIndex = lNormalElementBin->GetIndexArray().GetAt(lVertexIndex);

			FbxVector4 lNormal = lNormalElementBin->GetDirectArray().GetAt(lNormalIndex);
			fbxBin.norm.push_back(lNormal);
		}


		//iterating over uv sets
		FbxStringList lUvSetNameList;
		lMeshBin->GetUVSetNames(lUvSetNameList);
		for (int lUVsetIndex = 0; lUVsetIndex < lUvSetNameList.GetCount(); lUVsetIndex++)
		{
			//get the incexed uv set
			const char* lUVSetName = lUvSetNameList.GetStringAt(lUVsetIndex);
			const FbxGeometryElementUV* lUVElement = lMeshBin->GetElementUV(lUVSetName);

			if (!lUVElement)
				continue;

			if (lUVElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
				lUVElement->GetMappingMode() != FbxGeometryElement::eByControlPoint)
				return 0;

			//index array for holding the index referenced to uv data
			bool lUseIndex = lUVElement->GetReferenceMode() != FbxGeometryElement::eDirect;
			int lIndexCount = (lUseIndex) ? lUVElement->GetIndexArray().GetCount() : 0;

			const int lPolyCount = lMeshBin->GetPolygonCount();
			for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
			{
				// build the max index array that we need to pass into MakePoly
				const int lPolySize = lMeshBin->GetPolygonSize(lPolyIndex);
				for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
				{
					FbxVector2 lUVValue;

					//get the index of the current vertex in control points array
					int lPolyVertIndex = lMeshBin->GetPolygonVertex(lPolyIndex, lVertIndex);

					//the UV index depends on the reference mode
					int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyVertIndex) : lPolyVertIndex;

					lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

					fbxBin.uv.push_back(lUVValue);

				}
			}
		}
	}
	
	//getchar();
	//if (fbxMaya.vtx.size() > fbxBin.vtx.size() || fbxMaya.vtx.size() < fbxBin.vtx.size())
	//{
	//	return 0;
	//}
	//else
	//{
		for (int j = 0; j < fbxMaya.vtx.size(); j++)
		{
			if (fbxMaya.vtx.at(j) != fbxBin.vtx.at(j))
			{
				return 0;
			}
			if (fbxMaya.norm.at(j) != fbxBin.norm.at(j))
		{
			return 0;
		}
		if (fbxMaya.uv.at(j) != fbxBin.uv.at(j))
			{
				return 0;
			}
		}
	//}

	//#define DELTA 0.0000000000001
	//#define EQUAL(A,B) (abs((A)-(B)) < DELTA) ? true:false

	// The file has been imported; we can get rid of the importer.
	lImporterMaya->Destroy();
	lImporterBin->Destroy();

	return 0;
}

