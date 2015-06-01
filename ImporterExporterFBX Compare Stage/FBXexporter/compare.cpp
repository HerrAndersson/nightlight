#include "fbxdata.h"
#define DELTA 0.000001
#define EQUAL(A,B) (abs((A[0])-(B[0])) < DELTA && abs((A[1])-(B[1])) < DELTA && abs((A[2])-(B[2])) < DELTA && abs((A[3])-(B[3])) < DELTA) ? true:false

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
	const char* lFilenameMaya = "container.fbx";
	const char* lFilenameBin = "ContainerMaya.fbx";

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

		if (lNodeMaya->GetLight() != NULL)
		{
			lLightMaya = (FbxLight*)lNodeMaya->GetNodeAttribute();
			
			FbxVector4 LightPosition = lLightMaya->GetNode()->LclTranslation;
			fbxMaya.LightPos.push_back(LightPosition);
			
			const char* lLightTypes[] = { "Point", "Directional", "Spot" };
			
			fbxMaya.LightType.push_back(lLightTypes[lLightMaya->LightType.Get()]);
			
			fbxMaya.LightColor.push_back(lLightMaya->Color.Get());
		}
		else if (lNodeMaya->GetMesh() != NULL)
		{
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
				//int lIndexCount = (lUseIndex) ? lUVElement->GetIndexArray().GetCount() : 0;
				int lIndexCount = lUVElement->GetDirectArray().GetCount();

				//const int lPolyCount = lMeshMaya->GetPolygonCount();
				//for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
				//{
				// build the max index array that we need to pass into MakePoly
				//const int lPolySize = lMeshMaya->GetPolygonSize(lPolyIndex);
				//for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
				//{
				FbxVector2 lUVValue;

				//get the index of the current vertex in control points array
				//int lPolyVertIndex = lMeshMaya->GetPolygonVertex(lPolyIndex, lVertIndex);

				//the UV index depends on the reference mode
				//int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyVertIndex) : lPolyVertIndex;
				//int lUVIndex = lUVElement->GetDirectArray().GetAt(lPolyVertIndex);

				for (int count = 0; count < lIndexCount; count++)
				{
					lUVValue = lUVElement->GetDirectArray().GetAt(count);

					fbxMaya.uv.push_back(lUVValue);
				}

				//}
				//}
			}

			//material maya
			int materialCount = lNodeMaya->GetMaterialCount();
		
			if (materialCount > 0)
			{
				FbxPropertyT<FbxDouble3> double3;
				FbxPropertyT<FbxDouble> double1;
				FbxColor theColor;

				for (int q = 0; q < materialCount; q++)
				{
					FbxSurfaceMaterial *lMaterial = lNodeMaya->GetMaterial(q);

					if (lMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
					{
						//found phong
						fbxMaya.materialtype.push_back("Phong");

						double3 = ((FbxSurfacePhong*)lMaterial)->Ambient;
						theColor.Set(double3.Get()[0], double3.Get()[1], double3.Get()[2]);
						fbxMaya.ambient.push_back(theColor);

						double3 = ((FbxSurfacePhong*)lMaterial)->Diffuse;
						theColor.Set(double3.Get()[0], double3.Get()[1], double3.Get()[2]);
						fbxMaya.diffuse.push_back(theColor);

						double3 = ((FbxSurfacePhong*)lMaterial)->Specular;
						theColor.Set(double3.Get()[0], double3.Get()[1], double3.Get()[2]);
						fbxMaya.specular.push_back(theColor);

						double3 = ((FbxSurfacePhong*)lMaterial)->Emissive;
						theColor.Set(double3.Get()[0], double3.Get()[1], double3.Get()[2]);
						fbxMaya.emissive.push_back(theColor);

						double1 = ((FbxSurfacePhong*)lMaterial)->TransparencyFactor;
						fbxMaya.transparency.push_back(1.0 - double1.Get());

						double1 = ((FbxSurfacePhong*)lMaterial)->Shininess;
						fbxMaya.shininess.push_back(double1.Get());

						double1 = ((FbxSurfacePhong*)lMaterial)->ReflectionFactor;
						fbxMaya.reflectionfactor.push_back(double1.Get());

						

					}
					else if (lMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
					{
						//found lambert
						fbxMaya.materialtype.push_back("Lambert");

						double3 = ((FbxSurfaceLambert*)lMaterial)->Ambient;
						theColor.Set(double3.Get()[0], double3.Get()[1], double3.Get()[2]);
						fbxMaya.ambient.push_back(theColor);

						double3 = ((FbxSurfaceLambert*)lMaterial)->Diffuse;
						theColor.Set(double3.Get()[0], double3.Get()[1], double3.Get()[2]);
						fbxMaya.diffuse.push_back(theColor);

						double3 = ((FbxSurfaceLambert*)lMaterial)->Emissive;
						theColor.Set(double3.Get()[0], double3.Get()[1], double3.Get()[2]);
						fbxMaya.emissive.push_back(theColor);

						double1 = ((FbxSurfaceLambert*)lMaterial)->TransparencyFactor;
						fbxMaya.transparency.push_back(1.0-double1.Get());

					}

				}
			}
		
			lCameraMaya = lRootNodeMaya->GetCamera();
			if (lRootNodeMaya->GetCamera() != NULL)
			{
				double fovX = lCameraMaya->FieldOfViewX.Get();//horizontal fov
				fbxMaya.fieldofviewX.push_back(fovX);
				double fovY = lCameraMaya->FieldOfViewY.Get();//vertical fov
				fbxMaya.fieldofviewY.push_back(fovY);

				FbxDouble3 camUp = lCameraMaya->UpVector.Get();
				fbxMaya.cameraUpVector.push_back(camUp);
				FbxDouble3 camPos = lCameraMaya->Position.Get();
				fbxMaya.cameraPosition.push_back(camPos);

			}

		}
	}


	//bin file
	for (int i = 0; i < childcountBin; i++)
	{
		lNodeBin = lRootNodeBin->GetChild(i);

		if (lNodeBin->GetLight() != NULL)
		{
			lLightBin = (FbxLight*)lNodeBin->GetNodeAttribute();

			FbxVector4 LightPosition = lNodeBin->LclTranslation;
			fbxBin.LightPos.push_back(LightPosition);

			const char* lLightTypes[] = { "Point", "Directional", "Spot" };

			fbxBin.LightType.push_back(lLightTypes[lLightBin->LightType.Get()]);

			fbxBin.LightColor.push_back(lLightBin->Color.Get());
		}
		else if (lNodeBin->GetMesh() != NULL)
		{

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
				//int lIndexCount = (lUseIndex) ? lUVElement->GetIndexArray().GetCount() : 0;
				int lIndexCount = lUVElement->GetDirectArray().GetCount();


				FbxVector2 lUVValue;

				for (int count = 0; count < lIndexCount; count++)
				{
					lUVValue = lUVElement->GetDirectArray().GetAt(count);

					fbxBin.uv.push_back(lUVValue);
				}



				//material maya
				int materialCount = lNodeBin->GetMaterialCount();

				if (materialCount > 0)
				{
					FbxPropertyT<FbxDouble3> double3;
					FbxPropertyT<FbxDouble> double1;
					FbxColor theColor;

					for (int q = 0; q < materialCount; q++)
					{
						FbxSurfaceMaterial *lMaterial = lNodeBin->GetMaterial(q);

						if (lMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
						{
							//found phong
							fbxBin.materialtype.push_back("Phong");

							double3 = ((FbxSurfacePhong*)lMaterial)->Ambient;
							theColor.Set(double3.Get()[0], double3.Get()[1], double3.Get()[2]);
							fbxBin.ambient.push_back(theColor);

							double3 = ((FbxSurfacePhong*)lMaterial)->Diffuse;
							theColor.Set(double3.Get()[0], double3.Get()[1], double3.Get()[2]);
							fbxBin.diffuse.push_back(theColor);

							double3 = ((FbxSurfacePhong*)lMaterial)->Specular;
							theColor.Set(double3.Get()[0], double3.Get()[1], double3.Get()[2]);
							fbxBin.specular.push_back(theColor);

							double3 = ((FbxSurfacePhong*)lMaterial)->Emissive;
							theColor.Set(double3.Get()[0], double3.Get()[1], double3.Get()[2]);
							fbxBin.emissive.push_back(theColor);

							double1 = ((FbxSurfacePhong*)lMaterial)->TransparencyFactor;
							fbxBin.transparency.push_back(1.0 - double1.Get());

							double1 = ((FbxSurfacePhong*)lMaterial)->Shininess;
							fbxBin.shininess.push_back(double1.Get());

							double1 = ((FbxSurfacePhong*)lMaterial)->ReflectionFactor;
							fbxBin.reflectionfactor.push_back(double1.Get());



						}
						else if (lMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
						{
							//found lambert
							fbxBin.materialtype.push_back("Lambert");

							double3 = ((FbxSurfaceLambert*)lMaterial)->Ambient;
							theColor.Set(double3.Get()[0], double3.Get()[1], double3.Get()[2]);
							fbxBin.ambient.push_back(theColor);

							double3 = ((FbxSurfaceLambert*)lMaterial)->Diffuse;
							theColor.Set(double3.Get()[0], double3.Get()[1], double3.Get()[2]);
							fbxBin.diffuse.push_back(theColor);

							double3 = ((FbxSurfaceLambert*)lMaterial)->Emissive;
							theColor.Set(double3.Get()[0], double3.Get()[1], double3.Get()[2]);
							fbxBin.emissive.push_back(theColor);

							double1 = ((FbxSurfaceLambert*)lMaterial)->TransparencyFactor;
							fbxBin.transparency.push_back(1.0 - double1.Get());

						}

					}
				}

				lCameraBin = lRootNodeBin->GetCamera();
				if (lRootNodeBin->GetCamera() != NULL)
				{
					double fovX = lCameraBin->FieldOfViewX.Get();//horizontal fov
					fbxBin.fieldofviewX.push_back(fovX);
					double fovY = lCameraBin->FieldOfViewY.Get();//vertical fov
					fbxBin.fieldofviewY.push_back(fovY);

					FbxDouble3 camUp = lCameraBin->UpVector.Get();
					fbxBin.cameraUpVector.push_back(camUp);
					FbxDouble3 camPos = lCameraBin->Position.Get();
					fbxBin.cameraPosition.push_back(camPos);

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

	int correctVtxCount = 0;
	int correctNormCount = 0;
	int correctUVCount = 0;

	for (int j = 0; j < fbxMaya.vtx.size(); j++)
	{
		if (EQUAL(fbxMaya.vtx.at(j), fbxBin.vtx.at(j)))
		{
			correctVtxCount++;
		}
	}
	for (int j = 0; j < fbxMaya.norm.size(); j++)
	{
		if (EQUAL(fbxMaya.norm.at(j), fbxBin.norm.at(j)))
		{
			correctNormCount++;
		}
	}
	for (int j = 0; j < fbxMaya.uv.size(); j++)
	{
		if (EQUAL(fbxMaya.uv.at(j), fbxBin.uv.at(j)))
		{
		correctUVCount++;
		}
	}

std::cout << "Correct vtx: " << correctVtxCount << " of " << fbxMaya.vtx.size() << std::endl;
std::cout << "Correct normals: " << correctNormCount << " of " << fbxMaya.norm.size() << std::endl;
std::cout << "Correct UV: " << correctUVCount << " of " << fbxMaya.uv.size() << std::endl;



//}

int lightPosCount = 0;
int lightColourCount = 0;
int lightTypeCount = 0;

if (fbxMaya.LightPos.size() > fbxBin.LightPos.size() || fbxMaya.LightPos.size() < fbxBin.LightPos.size())
{
	std::cout<<"Light not correct." << std::endl;
}
else
{

for (int x = 0; x < fbxMaya.LightType.size(); x++)
{
	if (EQUAL(fbxMaya.LightPos.at(x),fbxBin.LightPos.at(x)))
	{
		lightPosCount++;
	}
	if (EQUAL(fbxMaya.LightColor.at(x), fbxBin.LightColor.at(x)))
	{
		lightColourCount++;
	}
	if (EQUAL(fbxMaya.LightType.at(x), fbxBin.LightType.at(x)))
	{
		lightTypeCount++;
	}
}
}
std::cout << "Correct light positions: " << lightPosCount << " of " << fbxMaya.LightPos.size() << std::endl;
std::cout << "Correct light colours: " << lightColourCount << " of " << fbxMaya.LightColor.size() << std::endl;
std::cout << "Correct light type: " << lightTypeCount << " of " << fbxMaya.LightType.size() << std::endl;

int materialtypeCount =0;
int ambientCount =0;
int diffuseCount=0;
int emissiveCount = 0;
int transparencyCount = 0;
int specularCount = 0;
int shinyCount = 0;
int reflectionCount = 0;



for (int q = 0; q < fbxMaya.ambient.size(); q++)
{
	if (EQUAL(fbxMaya.materialtype.at(q),fbxBin.materialtype.at(q)))
	{
		materialtypeCount++;
	}
	if (EQUAL(fbxMaya.ambient.at(q), fbxBin.ambient.at(q)))
	{
		ambientCount++;
	}
	if (EQUAL(fbxMaya.diffuse.at(q), fbxBin.diffuse.at(q)))
	{
		diffuseCount++;
	}
	if (EQUAL(fbxMaya.emissive.at(q), fbxBin.emissive.at(q)))
	{
		emissiveCount++;
	}
	if (fbxMaya.transparency.at(q) == fbxBin.transparency.at(q))
	{
		transparencyCount++;
	}
	if (fbxMaya.materialtype.at(q) == "Phong" && fbxBin.materialtype.at(q) == "Phong")
	{
		if (EQUAL(fbxMaya.specular.at(q),fbxBin.specular.at(q)))
		{
			specularCount++;
		}
		if (fbxMaya.shininess.at(q) == fbxBin.shininess.at(q))
		{
			shinyCount++;
		}
		if (fbxMaya.reflectionfactor.at(q) == fbxBin.reflectionfactor.at(q))
		{
			reflectionCount++;
		}
	}

}

std::cout << "Correct material types: " << materialtypeCount << " of " << fbxMaya.materialtype.size() << std::endl;
std::cout << "Correct ambients: " << ambientCount << " of " << fbxMaya.ambient.size() << std::endl;
std::cout << "Correct diffuses: " << diffuseCount << " of " << fbxMaya.diffuse.size() << std::endl;
std::cout << "Correct emissives: " << emissiveCount << " of " << fbxMaya.emissive.size() << std::endl;
std::cout << "Correct transparencies: " << transparencyCount << " of " << fbxMaya.transparency.size() << std::endl;
std::cout << "Correct speculars: " << specularCount << " of " << fbxMaya.specular.size() << std::endl;
std::cout << "Correct shininess: " << shinyCount << " of " << fbxMaya.shininess.size() << std::endl;
std::cout << "Correct reflections: " << reflectionCount << " of " << fbxMaya.reflectionfactor.size() << std::endl;


int cameraPosCount = 0;
int cameraUpCount = 0;
int fovXcount = 0;
int fovYcount = 0;

	for (int q = 0; q < fbxMaya.cameraPosition.size(); q++)
	{
		if (EQUAL(fbxMaya.cameraPosition.at(q), fbxBin.cameraPosition.at(q)))
		{
			cameraPosCount++;
		}
		if (EQUAL(fbxMaya.cameraUpVector.at(q), fbxBin.cameraUpVector.at(q)))
		{
			cameraUpCount++;
		}
		if (fbxMaya.fieldofviewX.at(q) == fbxBin.fieldofviewX.at(q))
		{
			fovXcount++;
		}
		if (fbxMaya.fieldofviewY.at(q) == fbxBin.fieldofviewY.at(q))
		{
			fovYcount++;
		}
	}


	std::cout << "Correct light positions: " << lightPosCount << " of " << fbxMaya.LightPos.size() << std::endl;
	std::cout << "Correct light colours: " << lightColourCount << " of " << fbxMaya.LightColor.size() << std::endl;
	std::cout << "Correct light type: " << lightTypeCount << " of " << fbxMaya.LightType.size() << std::endl;

	std::cout << "Correct camera positions: " << cameraPosCount << " of " << fbxMaya.cameraPosition.size() << std::endl;
	std::cout << "Correct up vector: " << cameraUpCount << " of " << fbxMaya.cameraUpVector.size() << std::endl;
	std::cout << "Correct fov in x: " << fovXcount << " of " << fbxMaya.fieldofviewX.size() << std::endl;
	std::cout << "Correct fov in y: " << fovYcount << " of " << fbxMaya.fieldofviewY.size() << std::endl;



		std::cout << "EVERYTHING IS AWESOME!" << std::endl;

	// The file has been imported; we can get rid of the importer.
	lImporterMaya->Destroy();
	lImporterBin->Destroy();

	return 0;
}

