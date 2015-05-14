
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "dataHandler.h"
DataHandler::DataHandler()
{
	objectData = new AssetManager;
}
DataHandler::~DataHandler()
{}
int DataHandler::FBXexport(std::vector<std::string>& binFileList, std::vector<Model>&modelList)
{
	for (int i = 0; i < binFileList.size(); i++)
	{
		//vectors
		typedef double Vector4[4];
		typedef double Vector2[2];

		//Create the FBX SDK manager
		FbxManager* lSdkManager = FbxManager::Create();

		//Create an IOSettings object.
		FbxIOSettings * ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
		lSdkManager->SetIOSettings(ios);

		//Configure the FbxIOSettings object. What to export and so on
		(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_MATERIAL, true);
		(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_TEXTURE, true);
		(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_LINK, true);
		(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_SHAPE, true);
		(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_GOBO, true);
		(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_ANIMATION, true);
		(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
		
		bool lEmbedMedia = true;
		(*(lSdkManager->GetIOSettings())).SetBoolProp(EXP_FBX_EMBEDDED, lEmbedMedia);

		
		//Save the filename as a string so we can use it when needed. 
		string nameStr = binFileList.at(i);
		
		//get rid of .bin in the name.
		for (int j = 0; j < 4; j++)
		{
			nameStr.pop_back();
		}
		
		//add "Scene"
		string sceneNameStr = nameStr;
		sceneNameStr = sceneNameStr +="Scene";
		
		//convert to char*
		char * sceneName = new char[binFileList.at(i).length()];
		std::strcpy(sceneName, sceneNameStr.c_str());
		
		//add the scene name from name in modellist
		FbxScene* lScene = FbxScene::Create(lSdkManager, sceneName);
		  
		//Create an exporter.
		FbxExporter* lExporter = FbxExporter::Create(lSdkManager, "");
		
		//filename of the file to which the scene will be exported.
		string fileNameStr = nameStr;
		
		//convert to char*
		char * lFilenameOut = new char[binFileList.at(i).length()];
		std::strcpy(lFilenameOut, fileNameStr.c_str());
		
		//Initialize the exporter.
		bool lExportStatus = lExporter->Initialize(lFilenameOut, -4, lSdkManager->GetIOSettings());
		if (!lExportStatus) {
			printf("Call to FbxExporter::Initialize() failed.\n");
			printf("Error returned: %s\n\n", lExporter->GetStatus().GetErrorString());
			return false;
		}
		
		// indices of the vertices per each polygon (faceIds)
		int size = modelList.at(i).vertexIndices.size();
		
		vector<int> vtxId;
		vector<int> normId;
		vector<int> uvId;
		
		//Get all the ID's
		for (int j = 0; j < size; j++)
		{
			//if (j % 3 == 0)
			//{
				vtxId.push_back(modelList.at(i).vertexIndices.at(j).x);
			//}
			normId.push_back(modelList.at(i).vertexIndices.at(j).y);
			uvId.push_back(modelList.at(i).vertexIndices.at(j).z);
		}
		// indices of the vertices per each polygon
	//static int vtxId2[24] = {
	//	       vtxId.at(0), vtxId.at(1), vtxId.at(2), vtxId.at(3),
	//	       vtxId.at(4), vtxId.at(5), vtxId.at(6), vtxId.at(7),
	//	       vtxId.at(8), vtxId.at(9), vtxId.at(10), vtxId.at(11),
	//	       vtxId.at(12), vtxId.at(13), vtxId.at(14), vtxId.at(15),
	//	       vtxId.at(16), vtxId.at(17), vtxId.at(18), vtxId.at(19),
	//	       vtxId.at(20), vtxId.at(21), vtxId.at(22), vtxId.at(23)
		//0, 1, 2, 3, // front  face  (Z+)
			//1, 5, 6, 2, // right  side  (X+)
			//5, 4, 7, 6, // back   face  (Z-)
			//4, 0, 3, 7, // left   side  (X-)
			//0, 4, 5, 1, // bottom face  (Y-)
			//3, 2, 6, 7  // top    face  (Y+)
			//0, 1, 2, 3, // front  face  (Z+)
			//1, 5, 6, 2, // right  side  (X+)
			//5, 4, 7, 6, // back   face  (Z-)
			//4, 0, 3, 7, // left   side  (X-)
			//0, 4, 5, 1, // bottom face  (Y-)
			//3, 2, 6, 7  // top    face  (Y+)
		//};
//		// control points
//		static Vector4 lControlPoints[8] = {
//			{ modelList.at(i).purePoints.at(0).position.x, modelList.at(i).purePoints.at(0).position.y, modelList.at(i).purePoints.at(0).position.z, 1.0 },
//			{ modelList.at(i).purePoints.at(1).position.x, modelList.at(i).purePoints.at(1).position.y, modelList.at(i).purePoints.at(1).position.z, 1.0 },
//			{ modelList.at(i).purePoints.at(2).position.x, modelList.at(i).purePoints.at(2).position.y, modelList.at(i).purePoints.at(2).position.z, 1.0 },
//			{ modelList.at(i).purePoints.at(3).position.x, modelList.at(i).purePoints.at(3).position.y, modelList.at(i).purePoints.at(3).position.z, 1.0 },
//			{ modelList.at(i).purePoints.at(4).position.x, modelList.at(i).purePoints.at(4).position.y, modelList.at(i).purePoints.at(4).position.z, 1.0 },
//			{ modelList.at(i).purePoints.at(5).position.x, modelList.at(i).purePoints.at(5).position.y, modelList.at(i).purePoints.at(5).position.z, 1.0 },
//			{ modelList.at(i).purePoints.at(6).position.x, modelList.at(i).purePoints.at(6).position.y, modelList.at(i).purePoints.at(6).position.z, 1.0 },
//			{ modelList.at(i).purePoints.at(7).position.x, modelList.at(i).purePoints.at(7).position.y, modelList.at(i).purePoints.at(7).position.z, 1.0 },
//		};
//		static Vector4 lControlPoints[8] = {
//			{ modelList.at(i).purePoints.at(0).position.x, modelList.at(i).purePoints.at(0).position.y, modelList.at(i).purePoints.at(0).position.z, 1.0 },
//			{ modelList.at(i).purePoints.at(3).position.x, modelList.at(i).purePoints.at(3).position.y, modelList.at(i).purePoints.at(3).position.z, 1.0 },
//			{ modelList.at(i).purePoints.at(6).position.x, modelList.at(i).purePoints.at(6).position.y, modelList.at(i).purePoints.at(6).position.z, 1.0 },
//			{ modelList.at(i).purePoints.at(9).position.x, modelList.at(i).purePoints.at(9).position.y, modelList.at(i).purePoints.at(9).position.z, 1.0 },
//			{ modelList.at(i).purePoints.at(12).position.x, modelList.at(i).purePoints.at(12).position.y, modelList.at(i).purePoints.at(12).position.z, 1.0 },
//			{ modelList.at(i).purePoints.at(15).position.x, modelList.at(i).purePoints.at(15).position.y, modelList.at(i).purePoints.at(15).position.z, 1.0 },
//			{ modelList.at(i).purePoints.at(18).position.x, modelList.at(i).purePoints.at(18).position.y, modelList.at(i).purePoints.at(18).position.z, 1.0 },
//			{ modelList.at(i).purePoints.at(21).position.x, modelList.at(i).purePoints.at(21).position.y, modelList.at(i).purePoints.at(21).position.z, 1.0 },
//		};
		////control points
   		
		//get the controlpoints
		int sizePoints = modelList.at(i).purePoints.size();
		vector<vector<double>> lControlPoints;
		
		for (int j = 0; j < sizePoints; j++)
		{
			{
				vector <double> point;
				
				point.push_back(modelList.at(i).purePoints.at(j).position.x);
				point.push_back(modelList.at(i).purePoints.at(j).position.y);
				point.push_back(modelList.at(i).purePoints.at(j).position.z);
				point.push_back(1.0f);
				lControlPoints.push_back(point);
				//if (j % 3 == 0)
			//{
			//	point.push_back(modelList.at(i).purePoints.at(j).position.x);
			//	point.push_back(modelList.at(i).purePoints.at(j).position.y);
			//	point.push_back(modelList.at(i).purePoints.at(j).position.z);
			//	point.push_back(1.0f);
			//	lControlPoints.push_back(point);
			//}
			}
		}

		//normals vertices per each polygon 
		int sizeNormals = modelList.at(i).normals.size();
		vector<float> lNormals;
		for (int j = 0; j < sizeNormals; j++)
		{
			lNormals.push_back(modelList.at(i).normals[j].x);
			lNormals.push_back(modelList.at(i).normals[j].y);
			lNormals.push_back(modelList.at(i).normals[j].z);
			lNormals.push_back(1.0f);
		}

		//static Vector4 lNormals[8]=
		//{
		//    { -0.577350258827209, -0.577350258827209, 0.577350258827209, 1.0 },
		//    { 0.577350258827209, -0.577350258827209, 0.577350258827209, 1.0 },
		//    { 0.577350258827209, 0.577350258827209, 0.577350258827209, 1.0 },
		//    { -0.577350258827209, 0.577350258827209, 0.577350258827209, 1.0 },
		//    { -0.577350258827209, -0.577350258827209, -0.577350258827209, 1.0 },
		//    { 0.577350258827209, -0.577350258827209, -0.577350258827209, 1.0 },
		//    { 0.577350258827209, 0.577350258827209, -0.577350258827209, 1.0 },
		//    { -0.577350258827209, 0.577350258827209, -0.577350258827209, 1.0 }
		//};
		////uvs
		//
		//// normals vertices per each polygon 
		//int size = modelList.at(i).UVs.size();
		//vector<XMFLOAT2> lUVs;
		//for (int i; i < size; i++)
		//{
		//    lUVs.push_back(modelList.at(i).UVs[i]);
		//}
		//    int size = modelList.at(i).UVs.size();
		static Vector2 lUVs[14] =
		{
			{ 0.0, 1.0 },
			{ 1.0, 0.0 },
			{ 0.0, 0.0 },
			{ 1.0, 1.0 }
		};
		//indices of the uvs per each polygon
		static int uvsId[24] =
		{
			0, 1, 3, 2, 2, 3, 5, 4, 4, 5, 7, 6, 6, 7, 9, 8, 1, 10, 11, 3, 12, 0, 2, 13
		};
		//create the main structure.
		FbxMesh* lMesh = FbxMesh::Create(lScene, "");
		
		// Create control points.
		lMesh->InitControlPoints(sizePoints);
		FbxVector4* vertex = lMesh->GetControlPoints();
		
		//Maybe kanske this but probably not
		//memcpy((void*)vertex, (void*)&lControlPoints, sizePoints * sizeof(FbxVector4));
		//Maybe kanske this but probably not
		memcpy((void*)vertex, (void*)&lControlPoints, sizeof(lControlPoints));

		//create the materials
		FbxGeometryElementMaterial* lMaterialElement = lMesh->CreateElementMaterial();
		lMaterialElement->SetMappingMode(FbxGeometryElement::eAllSame);
		lMaterialElement->SetReferenceMode(FbxGeometryElement::eIndexToDirect);
		lMaterialElement->GetIndexArray().Add(0);

		int numFaces = (vtxId.size() / 3);
		// Create polygons later after FbxGeometryElementMaterial is created.Assign material indices.
		int vId = 0;
		for (int f = 0; f < numFaces; f++)
		{
			lMesh->BeginPolygon();
			//lMesh->ReservePolygonCount(3);
			for (int v = 0; v < 3; v++)
				//	lMesh->AddPolygon(vtxId[vId++]);
				lMesh->AddPolygon(vtxId.at(vId++));
			lMesh->EndPolygon();
		}

		//specify normals per control point.
		FbxGeometryElementNormal* lNormalElement = lMesh->CreateElementNormal();
		lNormalElement->SetMappingMode(FbxGeometryElement::eByControlPoint);
		lNormalElement->SetReferenceMode(FbxGeometryElement::eDirect);
		for (int n = 0; n < sizeNormals; n++)
			lNormalElement->GetDirectArray().Add(FbxVector4(lNormals.at(n), lNormals.at(n), lNormals.at(n)));
		
		//for (int n = 0; n<8; n++)
		//    lNormalElement->GetDirectArray().Add(FbxVector4(lNormals[n][0], lNormals[n][1], lNormals[n][2]));
		//create nodeName from file name
		
		string meshNameStr = binFileList.at(i);
		
		//get rid of .bin
		for (int j = 0; j < 4; j++)
		{
			meshNameStr.pop_back();
		}
		char * meshName = new char[binFileList.at(i).length()];
		std::strcpy(meshName, meshNameStr.c_str());
		
		//Create the node containing the mesh
		FbxNode* lNode = FbxNode::Create(lScene, meshName);
		
		//Find the translation of object and add here
		//lNode->LclTranslation.Set(pLclTranslation);
		lNode->SetNodeAttribute(lMesh);
		lNode->SetShadingMode(FbxNode::eTextureShading);
		
		//create UVset
		FbxGeometryElementUV* lUVElement1 = lMesh->CreateElementUV("UVSet1");
		FBX_ASSERT(lUVElement1 != NULL);
		lUVElement1->SetMappingMode(FbxGeometryElement::eByPolygonVertex);
		lUVElement1->SetReferenceMode(FbxGeometryElement::eIndexToDirect);
		for (int i = 0; i < 4; i++)
			lUVElement1->GetDirectArray().Add(FbxVector2(lUVs[i][0], lUVs[i][1]));
		for (int i = 0; i < 24; i++)
			lUVElement1->GetIndexArray().Add(uvsId[i % 4]);
		
		// Add the mesh node to the root node in the scene.
		FbxNode *lRootNode = lScene->GetRootNode();
		lRootNode->AddChild(lNode);
		lExporter->Export(lScene);
		
		//Get rid of objects
		lExporter->Destroy();

	}
	return 0;
}

void DataHandler::importBinData(std::vector<std::string>& binFileList, std::vector<Model>&modelList)
{
	Model model;
	for (int i = 0; i < binFileList.size(); i++)
	{
		objectData->LoadModel("../Bin/" + binFileList.at(i), model);

		modelList.push_back(model);
	}

}
bool DataHandler::getBinFilenamesInDirectory(char *folder_path, std::vector<std::string> &list_to_fill)
{
	WIN32_FIND_DATA fdata;
	HANDLE dhandle;
	// måste lägga till \* till genvägen
	{
		char buf[MAX_PATH];
		sprintf_s(buf, sizeof(buf), "%s\\*", folder_path);
		if ((dhandle = FindFirstFile(buf, &fdata)) == INVALID_HANDLE_VALUE) {
			return false;
		}
	}
	while (true)
	{
		if (FindNextFile(dhandle, &fdata))
		{
			// vi vill endast ha ".bin"-filer
			if (strlen(fdata.cFileName) > 4)
			{
				if (strcmp(&fdata.cFileName[strlen(fdata.cFileName) - 3], ".bin") == 1)
				{
					list_to_fill.push_back(fdata.cFileName);
				}
			}
		}
		else
		{
			if (GetLastError() == ERROR_NO_MORE_FILES)
			{
				break;
			}
			else
			{
				FindClose(dhandle);
				return false;
			}
		}
	}
	if (!FindClose(dhandle))
	{
		return false;
	}
	return true;
}