
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
			
			vtxId.push_back(modelList.at(i).vertexIndices.at(j).x);
			normId.push_back(modelList.at(i).vertexIndices.at(j).y);
			uvId.push_back(modelList.at(i).vertexIndices.at(j).z);
		}

		////control points
   		
		//get the controlpoints
		int sizePoints = modelList.at(i).purePoints.size();
		vector<FbxVector4> lControlPoints;
		
		for (int j = 0; j < sizePoints; j++)
		{
			{
				FbxVector4 point = { (double)(modelList.at(i).purePoints.at(j).position.x), (double)(modelList.at(i).purePoints.at(j).position.y), (double)(modelList.at(i).purePoints.at(j).position.z), (double)(1.0f) };
				lControlPoints.push_back(point);
		
			}
		}

		//normals vertices per each polygon 
		int sizeNormals = modelList.at(i).normals.size();
		vector<FbxVector4> lNormals;
		for (int j = 0; j < sizeNormals; j++)
		{
			FbxVector4 normal = { (double)(modelList.at(i).normals.at(j).x), (double)(modelList.at(i).normals.at(j).y), (double)(modelList.at(i).normals.at(j).z), (double)(1.0f) };
			lNormals.push_back(normal);
		}

		

		vector<FbxVector2> lUVs;
		for (int j = 0; j < modelList.at(i).UVs.size(); j++)
		{
			
			FbxVector2 UVs = { (double)(modelList.at(i).UVs.at(j).x), (double)(modelList.at(i).UVs.at(j).y) };
			lUVs.push_back(UVs);
		}

	
		//create the main structure.
		FbxMesh* lMesh = FbxMesh::Create(lScene, "");
		
		// Create control points.
		lMesh->InitControlPoints(sizePoints);
		FbxVector4* vertex = lMesh->GetControlPoints();

		//set controlPoints
		for (int p = 0; p < sizePoints; p++)
		{
			lMesh->SetControlPointAt(lControlPoints.at(p), p);
		}
		
		///MATERIALS
		//create the materials
		FbxGeometryElementMaterial* lMaterialElement = lMesh->CreateElementMaterial();
		lMaterialElement->SetMappingMode(FbxGeometryElement::eAllSame);
		lMaterialElement->SetReferenceMode(FbxGeometryElement::eIndexToDirect);
		lMaterialElement->GetIndexArray().Add(0);

		FbxString lMaterialName = lFilenameOut;
		FbxString lShadingName = "Phong";
		//lMaterialName += i;
		FbxDouble3 lBlack(0.0, 0.0, 0.0);
		FbxDouble3 lRed(0.0, 0.0, 0.0);
		FbxDouble3 lColor;
		FbxSurfacePhong *lMaterial = FbxSurfacePhong::Create(lScene, lMaterialName.Buffer());

		//Generate primary and secondary colors.
		lMaterial->Emissive.Set(lBlack);
		lMaterial->Ambient.Set(lRed);
		lColor = FbxDouble3(modelList.at(i).diffuse.x, modelList.at(i).diffuse.y, modelList.at(i).diffuse.z);
		lMaterial->Diffuse.Set(lColor);
		lMaterial->TransparencyFactor.Set(0.0);
		lMaterial->ShadingModel.Set(lShadingName);
		lMaterial->Shininess.Set(modelList.at(i).specular.z);


		FbxFileTexture* lTexture = FbxFileTexture::Create(lScene, "Diffuse Texture");
		string textFileName = "../Textures/" + nameStr + ".png";
		// convert to char*
		char * textureFileName = new char[textFileName.size()];
		std::strcpy(textureFileName, textFileName.c_str());

		// Set texture properties.
		lTexture->SetFileName(textureFileName); // Needs a texture string from bin file. 
		lTexture->SetTextureUse(FbxTexture::eStandard);
		lTexture->SetMappingType(FbxTexture::eUV);
		lTexture->SetMaterialUse(FbxFileTexture::eModelMaterial);
		lTexture->SetSwapUV(false);
		lTexture->SetTranslation(0.0, 0.0);
		lTexture->SetScale(1.0, 1.0);
		lTexture->SetRotation(0.0, 0.0);

		//connect the texture to the corresponding property of the material
		if (lMaterial)
			lMaterial->Diffuse.ConnectSrcObject(lTexture);

	//	lTexture = FbxFileTexture::Create(lScene, "Ambient Texture");
	//
	//	//Not needed
	//	lTexture->SetFileName("gradient.jpg"); // Not needed
	//	lTexture->SetTextureUse(FbxTexture::eStandard);
	//	lTexture->SetMappingType(FbxTexture::eUV);
	//	lTexture->SetMaterialUse(FbxFileTexture::eModelMaterial);
	//	lTexture->SetSwapUV(false);
	//	lTexture->SetTranslation(0.0, 0.0);
	//	lTexture->SetScale(1.0, 1.0);
	//	lTexture->SetRotation(0.0, 0.0);
	//
	//	// don't forget to connect the texture to the corresponding property of the material
	//	if (lMaterial)
	//		lMaterial->Ambient.ConnectSrcObject(lTexture);
	//
	//	lTexture = FbxFileTexture::Create(lScene, "Emissive Texture");
	//
	//	// Set texture properties.
	//	lTexture->SetFileName("spotty.jpg"); // Resource file is in current directory.
	//	lTexture->SetTextureUse(FbxTexture::eStandard);
	//	lTexture->SetMappingType(FbxTexture::eUV);
	//	lTexture->SetMaterialUse(FbxFileTexture::eModelMaterial);
	//	lTexture->SetSwapUV(false);
	//	lTexture->SetTranslation(0.0, 0.0);
	//	lTexture->SetScale(1.0, 1.0);
	//	lTexture->SetRotation(0.0, 0.0);
	//
	//	// don't forget to connect the texture to the corresponding property of the material
	//	if (lMaterial)
	//		lMaterial->Emissive.ConnectSrcObject(lTexture);
		
		int numFaces = (vtxId.size() / 3);
		//Create polygons later after FbxGeometryElementMaterial is created.Assign material indices.
		
		int vId = 0;
		for (int f = 0; f < numFaces; f++)
		{
			lMesh->BeginPolygon();
			lMesh->ReservePolygonCount(3);
			for (int v = 0; v < 3; v++)
				//	lMesh->AddPolygon(vtxId[vId++]);
				lMesh->AddPolygon(vtxId.at(vId++));
			lMesh->EndPolygon();
		}

		//specify normals per control point.
		FbxGeometryElementNormal* lNormalElement = lMesh->CreateElementNormal();
		lNormalElement->SetMappingMode(FbxGeometryElement::eByPolygonVertex);
		lNormalElement->SetReferenceMode(FbxGeometryElement::eDirect);

		//set normals
		for (int n = 0; n < normId.size(); n++)
		{
			lNormalElement->GetDirectArray().Add(lNormals.at(normId.at(n)));

		}
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
		
		//LIGHTS AND CAMERA
//		//create a camera (pls don't)
//		FbxCamera* myCamera = FbxCamera::Create(lScene, sceneName);
//		// Set camera properties for classic TV projection with aspect ratio 4:3
//		myCamera->SetFormat(FbxCamera::eNTSC);
//		FbxNode* camNode = FbxNode::Create(lScene, sceneName);
//		camNode->LclTranslation.Set(FbxDouble3(-2, 0, i));
//		camNode->LclRotation.Set(FbxDouble3(0, 0, 0));
//		camNode->SetNodeAttribute(myCamera);
//		
//		
//		FbxLight* lLight = FbxLight::Create(lScene, sceneName);
//		//Create a node for our light in the scene.
//		FbxNode* lLightNode = FbxNode::Create(lScene, sceneName);
//		lLightNode->LclTranslation.Set(FbxDouble3(0, 2, i));
//		lLightNode->LclRotation.Set(FbxDouble3(0, 0, 0));
//		//Create a light.
//		//Set the node attribute of the light node.
//		lLightNode->SetNodeAttribute(lLight);

		
	
		//set the translation of object and add here
		lNode->LclTranslation.Set(FbxDouble3(0, 0, i));
		lNode->LclRotation.Set(FbxDouble3(0, 0, 0));

		lNode->SetNodeAttribute(lMesh);
		lNode->SetShadingMode(FbxNode::eTextureShading);
		
		if (lNode)
			lNode->AddMaterial(lMaterial);
		

		////create UVset
		FbxGeometryElementUV* lUVElement1 = lMesh->CreateElementUV("UVSet1");
		FBX_ASSERT(lUVElement1 != NULL);
		lUVElement1->SetMappingMode(FbxGeometryElement::eByPolygonVertex);
		lUVElement1->SetReferenceMode(FbxGeometryElement::eIndexToDirect);

		
		for (int u = 0; u < modelList.at(i).UVs.size(); u++)
			lUVElement1->GetDirectArray().Add((lUVs.at(u)));

		for (int i = 0; i<uvId.size(); i++)
			lUVElement1->GetIndexArray().Add(uvId.at(i));
		
		// Add the mesh node to the root node in the scene.
		FbxNode *lRootNode = lScene->GetRootNode();
		lRootNode->AddChild(lNode);

	//	//add the camera and light(pls don't though)
	//	lRootNode->AddChild(camNode);
	//	lRootNode->AddChild(lLightNode);

//PARENT CHILD HIERARCHY
	//FbxNode* lNode1 = FbxNode::Create(lScene, "thing1");
	//lNode1->LclTranslation.Set(FbxDouble3(1, 0, 0));
	//lNode1->SetNodeAttribute(lMesh);
	//FbxNode* lNode2 = FbxNode::Create(lScene, "thing2");
	//lNode2->LclTranslation.Set(FbxDouble3(2, 0, 0));
	//lNode2->SetNodeAttribute(lMesh);
	//FbxNode* lNode3 = FbxNode::Create(lScene, "thing3");
	//lNode3->LclTranslation.Set(FbxDouble3(3, 0, 0));
	//lNode3->SetNodeAttribute(lMesh);
	//FbxNode* lNode4 = FbxNode::Create(lScene, "thing4");
	//lNode4->LclTranslation.Set(FbxDouble3(4, 0, 0));
	//lNode4->SetNodeAttribute(lMesh);
	//lNode->AddChild(lNode1);
	//lNode1->AddChild(lNode2);
	//lNode2->AddChild(lNode3);
	//lNode3->AddChild(lNode4);

		

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
				if (strcmp(&fdata.cFileName[strlen(fdata.cFileName) - 3], ".bin") == true)
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