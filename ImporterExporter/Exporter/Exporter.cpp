// Jonas Petersson
// jnp@bth.se
// 14-03-31

//Länkar

// http://nccastaff.bournemouth.ac.uk/jmacey/RobTheBloke/www/research/maya/mfnmaterial.htm
// http://help.autodesk.com/view/MAYAUL/2015/ENU/
// http://shiba.hpe.cn/jiaoyanzu/wuli/soft/Hlsl/Character-Animation-With-Direct3D.pdf

#include "Exporter.h"

int debug = 0;

// konstuktor
Exporter::Exporter()
{
	SceneData DataStorage;
}

// destruktor
Exporter::~Exporter()
{
	CloseExportFiles();
	CleanUpMaya();
}

// initiera exportern
bool Exporter::InitializeMaya()
{
	// [Ur Maya-dokumentationen]
	// Initialize the Maya library.
	// This method must be called before any Maya functions sets are used.
	// It is acceptable to instantiate Maya fundamental types before calling this, but attempts to do anything else cause unpredictable results.
	// When writing a Maya library mode application, a call to this method should be the first line of main.
	if (!MLibrary::initialize("Exporter", true))
	{
		std::cout << "<Error> MLibrary::initialize()" << std::endl;
		return false;
	}
	return true;
}

// förstör dynamiskt allokerade objekt etc...
void Exporter::CleanUpMaya()
{

	// [Ur Maya-dokumentationen]
	// Undo the initialization performed by the initialize method, cleanup allocated Maya structures in an orderly fashion and terminate the application.
	// Note: It is important that when a Library mode process terminates it calls this method before doing so. Failure to do so could result in the leaking of licenses and errors from Maya's static destructors.
	// If the exitWhenDone parameter is true, which is the default, this method will terminate the process, using the supplied exitCode, and control will NOT be returned to the caller.
	// If the Library app was launched from another process as a separate thread or a child process, this can result in the parent process terminating as well.
	// To avoid that, set exitWhenDone to false, which will prevent the current process from being terminated and will return control to the caller.
	// However, in that case, Library mode must not be re-entered within the same process as Maya will be in an undetermined state and its behavour would be unpredictable.
	MLibrary::cleanup(0, false);
}

bool Exporter::CreateExportFiles(std::string file_path, std::string output_type)
{
	// hitta index för punkten innan filtypen.
	int sub_string_length = (int)file_path.find_last_of(".", file_path.size() - 1);

	// spargenväg för den exporterade filen.
	if (output_type == ".bin")
	{
		std::string binsave_path = file_path.substr(0, sub_string_length) + ".bin";
		std::string save_path = file_path.substr(0, sub_string_length) + ".obj";

		std::cout << "Exporting file to " << save_path.c_str() << std::endl << std::endl;

		export_stream_.open(save_path.c_str(), std::ios_base::out | std::ios_base::trunc);
		outfile.open(binsave_path.c_str(), std::ofstream::binary);
		infile.open(binsave_path.c_str(), std::ifstream::binary);
	}

	else if (output_type == ".lvl")
	{
		std::string lvlsave_path = file_path.substr(0, sub_string_length) + ".lvl";

		std::cout << "Exporting level to " << lvlsave_path.c_str() << std::endl << std::endl;

		export_stream_.open(lvlsave_path.c_str(), std::ios_base::out | std::ios_base::trunc);
	}


	if (!export_stream_.is_open())
	{
		std::cout << "<Error> fstream::open()" << std::endl;
		return false;
	}
	return true;
}

void Exporter::CloseExportFiles()
{
	if (export_stream_.is_open())
	{
		export_stream_.close();
	}
}

bool Exporter::GetMayaFilenamesInDirectory(char *folder_path, std::vector<std::string> &list_to_fill)
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

	// även en "tom" katalog ger tvEresultat - . och ..
	// vi vill inte ha dem :-)
	//fileList.push_back(fdata.cFileName);

	while (true)
	{
		if (FindNextFile(dhandle, &fdata))
		{
			// vi vill endast ha ".mb"-filer
			if (strlen(fdata.cFileName) > 4)
			{
				if (strcmp(&fdata.cFileName[strlen(fdata.cFileName) - 3], ".mb") == 0)
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

// funktionen som startar allting - initierar Maya, hämtar Mayafilernas namn, går igenom scenen, etc...
void Exporter::StartExporter(std::string directory_path)
{
	std::cout << "Initializing exporter" << std::endl << std::endl;
	if (!InitializeMaya())
	{
		std::cout << "Failed to initialize exporter" << std::endl;
		return;
	}

	std::vector<std::string> file_list;

	if (!GetMayaFilenamesInDirectory((char*)directory_path.c_str(), file_list))
	{
		std::cout << ".mb file not found" << std::endl;
		return;
	}

	char tmp_str[MAX_PATH];

	for (auto file = file_list.begin(); file != file_list.end(); file++)
	{
		// Formaterar och lagrar tecken och värden i en buffer
		sprintf_s(tmp_str, sizeof(tmp_str), "%s%s", directory_path.c_str(), file->c_str());

		if (file->find("level") != std::string::npos)
		{
			ProcessLevel(tmp_str);
			CloseExportFiles();
		}
		else
		{
			ProcessScene(tmp_str);
			CloseExportFiles();
		}
	}

	return;
}

void Exporter::ProcessScene(const char *file_path)
{
	MStatus status = MS::kSuccess;

	// [Ur Maya-dokumentationen]
	// Set everything back to a new file state.
	status = MFileIO::newFile(true);
	if (!status)
	{
		std::cout << "<Error> MFileIO::newFile()" << std::endl;
		return;
	}

	// [Ur Maya-dokumentationen]
	// Open the given file, and set the current active file to this file. If there are unsaved changes in the current scene, this operation will fail unless the force flag is set to true.
	status = MFileIO::open(file_path, NULL, true);
	if (!status)
	{
		std::cout << "<Error> MFileIO::open()" << std::endl;
		return;
	}

	// skapar exportfilerna
	if (!CreateExportFiles(file_path, ".bin"))
	{
		std::cout << "<Error> CreateExportFiles()" << std::endl;
		return;
	}

	// identifiera och extrahera data från scenen
	std::cout << "Identifying scene information" << std::endl << std::endl;
	if (!IdentifyAndExtractScene())
	{
		std::cout << "<Error> IdentifyAndExtractScene()" << std::endl;
		return;
	}
	else
	{
		// exportera data till fil
		std::cout << "Exporting scene information" << std::endl << std::endl;
		ExportScene();
	}
	scene_.meshes.clear();
	scene_.materials.clear();
	scene_.cameras.clear();
	scene_.blendShapes.clear();
	scene_.lights.ambientLights.clear();
	scene_.lights.areaLights.clear();
	scene_.lights.dirLights.clear();
	scene_.lights.spotLights.clear();
	scene_.lights.pointLights.clear();
	scene_.animations.clear();
	scene_.skeleton.clear();
}

void Exporter::ProcessLevel(const char *file_path)
{
	MStatus status = MS::kSuccess;

	// [Ur Maya-dokumentationen]
	// Set everything back to a new file state.
	status = MFileIO::newFile(true);
	if (!status)
	{
		std::cout << "<Error> MFileIO::newFile()" << std::endl;
		return;
	}

	// [Ur Maya-dokumentationen]
	// Open the given file, and set the current active file to this file. If there are unsaved changes in the current scene, this operation will fail unless the force flag is set to true.
	status = MFileIO::open(file_path, NULL, true);
	if (!status)
	{
		std::cout << "<Error> MFileIO::open()" << std::endl;
		return;
	}

	// skapar exportfilerna
	if (!CreateExportFiles(file_path, ".lvl"))
	{
		std::cout << "<Error> CreateExportFiles()" << std::endl;
		return;
	}

	if (!isAssetListLoaded)
	{
		fileToStrings("../Bin/gameObjectTypes.txt", levelGameObjectTypes);

		isAssetListLoaded = true;
	}

	if (!IdentifyAndExtractLevelInformation())
	{
		std::cout << "<Error> IdentifyAndExtractLevelInformation()" << std::endl;
		cin.ignore();
		return;
	}

	else
	{
		// exportera leveldata till fil
		std::cout << "Exporting level information" << std::endl << std::endl;
		ExportLevel();
	}


}

//___________________________________________________________________________________________
//|																							|
//|								FUNKTIONER FÖR ATT IDENTIFIERA DATA							|
//|_________________________________________________________________________________________|

bool Exporter::IdentifyAndExtractLevelInformation()
{
	formattedLevelData.clear();

	UINT index = 0;
	scene_.meshes.clear();

	MDagPath dag_path;
	MItDag dag_iter(MItDag::kBreadthFirst, MFn::kMesh);
	MSpace::Space transform_space = MSpace::kPostTransform;

	std::vector<std::string> nodeVec;
	bool relevantObjectFound = false;

	std::vector<MString> takenNames;


	while (!dag_iter.isDone())
	{
		if (dag_iter.getPath(dag_path))
		{
			MFnDagNode dag_node = dag_path.node();
			MFnTransform dag_node_parent = dag_node.parent(0);

			MString nodeName = dag_node_parent.name();
			std::string nodeNameStr = nodeName.asChar();
			nodeVec.clear();


			bool taken = false;
			for (int i = 0; i < takenNames.size(); i++){
				if (strcmp(takenNames[i].asChar(), nodeName.asChar()) == 0)
				{
					taken = true;
				}
			}

			if (taken == false)
			{
				takenNames.push_back(nodeName);

				std::transform(nodeNameStr.begin(), nodeNameStr.end(), nodeNameStr.begin(), ::tolower);
				int gameObjectID = -1;

				splitStringToVector(nodeNameStr, nodeVec, "_");

				for (int i = 0; i < levelGameObjectTypes.size() && !relevantObjectFound; i++)
				{
					if (nodeVec[2].find(levelGameObjectTypes.at(i)) != std::string::npos)
					{
						gameObjectID = i;
						relevantObjectFound = true;
					}
				}
				if (relevantObjectFound)
				{
					std::string formattedOutput;

					std::string goType = levelGameObjectTypes.at(gameObjectID);

					MMatrix transformMat = dag_node_parent.transformationMatrix();

					MEulerRotation eulerRotation;
					MFloatVector position;

					dag_node_parent.getRotation(eulerRotation);
					position = dag_node_parent.translation(transform_space);

					int coordX = (int)(position.x + EPS);
					int coordY = (int)(position.z + EPS);

					if (coordX < 0 || coordY < 0)
					{
						cout << "A gameObject:" + nodeNameStr + " is out of bounds.";
						return false;
					}

					formattedOutput += nodeVec.at(1) + "," +
						std::to_string(gameObjectID) + "," +
						std::to_string(-position.x) + "," + std::to_string(-position.y) + "," + std::to_string(-position.z) + "," +
						std::to_string(eulerRotation.y) + "," +
						std::to_string(coordX) + "," + std::to_string(coordY) + ",";

					if (goType == "door")
					{
						if (nodeVec.at(3) == "open")
						{
							formattedOutput += "1,";
						}
						else
						{
							formattedOutput += "0,";
						}

						if (nodeVec.at(4) == "start")
						{
							formattedOutput += "1,";
						}
						else if (nodeVec.at(4) == "end")
						{
							formattedOutput += "2,";
						}
						else
						{
							formattedOutput += "0,";
						}

						formattedOutput += nodeVec.at(1) + nodeVec.at(2);
					}
					if (goType == "lever")
					{
						if (nodeVec.at(3) == "poweron")
						{
							formattedOutput += "1,";
						}
						else
						{
							formattedOutput += "0,";
						}



						if (nodeVec.at(4) == "down")
						{
							formattedOutput += "1,";
						}
						else
						{
							formattedOutput += "0,";
						}

						formattedOutput += nodeVec.at(1) + nodeVec.at(2);
						formattedOutput += ",";
						formattedOutput += nodeVec.at(5);
					}
					if (goType == "pressure" ||
						goType == "container")
					{
						formattedOutput += nodeVec.at(3);
					}

					formattedLevelData.push_back(formattedOutput);

					relevantObjectFound = false;
				}
			}
		}
		dag_iter.next();
	}
	return true;
}

// identifiera och extrahera data från scenen
bool Exporter::IdentifyAndExtractScene()
{
	bool status = true;

	status = IdentifyAndExtractMeshes();

	return status;
}

//CURRENTLY ONLY SAVING THE MOST RELEVANT DATA
void Exporter::extractCamera(MObject& cam)
{
	MSpace::Space world_space = MSpace::kWorld;
	MSpace::Space transform_space = MSpace::kTransform;

	//Temp storage for camera
	cameraData TempCameraStorage;

	//Create a fucntion set for the camera
	MFnCamera fn(cam);

	//Ignore orthographic cameras. Top, right, left....
	if (fn.isOrtho())
		return;

	//attach a function set to the parent transform
	MFnDependencyNode fnParent(fn.parent(0));

	//Output some camera info
	std::cout << "Camera: " << fn.name().asChar()
		<< "\n\tparent "
		<< fnParent.name().asChar()
		<< std::endl;

	//Get Transform experimentation
	MFnTransform fs(fn.parent(0));
	MMatrix matrix = fs.transformation().asMatrix();
	std::cout << "\nTransform Matrix: " << matrix << std::endl;

	TempCameraStorage.transformMatrix = matrix;

	//aspect ratio
	std::cout << "\nAspect ratio: " << fn.aspectRatio()
		<< std::endl;
	TempCameraStorage.aspectRatio = fn.aspectRatio();

	//near clipping plane
	std::cout << "\nNear: : " << fn.nearClippingPlane()
		<< std::endl;
	TempCameraStorage.nearClippingPlane = fn.nearClippingPlane();

	//far clipping plane
	std::cout << "\nFar: " << fn.farClippingPlane()
		<< std::endl;
	TempCameraStorage.farClippingPlane = fn.farClippingPlane();

	//horizontal field of view
	std::cout << "\nHorizontal fov: " << fn.horizontalFieldOfView()
		<< std::endl;

	//vertical field of view
	std::cout << "\nVertical fov: " << fn.verticalFieldOfView()
		<< std::endl;
	TempCameraStorage.verticalFieldOfView = fn.verticalFieldOfView();

	//badly formated Projection matrix
	std::cout << "\nProjectionMatrix: " << fn.projectionMatrix()
		<< std::endl;
	TempCameraStorage.projectionMatrix = fn.projectionMatrix();



	MFloatMatrix modelMatrix = fs.transformation().asMatrix().matrix;
	MFloatVector direction = modelMatrix * (MFloatVector)fn.viewDirection();

	direction.normalize();
	direction.x *= -1.0f;
	direction.y *= -1.0f;

	//Up direction
	std::cout << "\nUp Vector: " << fn.upDirection()
		<< std::endl;
	TempCameraStorage.upVector = fn.upDirection();

	//view direction
	std::cout << "\nView Direction: " << direction
		<< std::endl;
	TempCameraStorage.viewDirection = direction;

	/*
		MQuaternion rotation;
		MEulerRotation eulerRotation1;

		fs.getRotation(rotation, transform_space);
		fs.getRotation(eulerRotation1);

		MEulerRotation eulerRotation2 = rotation.asEulerRotation(); // samma som eulerRotation1
		*/

	TempCameraStorage.position = fs.translation(transform_space);
	//pushback to store everything
	scene_.cameras.push_back(TempCameraStorage);

}

void Exporter::extractColor(Color& tempcolor, MFnDependencyNode& fn, MString name)
{
	MPlug p;

	MString r = name;
	r += "R";
	MString g = name;
	g += "G";
	MString b = name;
	b += "B";
	MString a = name;
	a += "A";

	p = fn.findPlug(r.asChar());
	p.getValue(tempcolor.r);
	p = fn.findPlug(g.asChar());
	p.getValue(tempcolor.g);
	p = fn.findPlug(b.asChar());
	p.getValue(tempcolor.b);
	p = fn.findPlug(a.asChar());
	p.getValue(tempcolor.a);
	p = fn.findPlug(name.asChar());

	MPlugArray connections;
	p.connectedTo(connections, true, false);

	int debug = connections.length();

	for (int i = 0; i != connections.length(); ++i)
	{
		// if file texture found
		if (connections[i].node().apiType() == MFn::kFileTexture)
		{
			// bind a function set to it ....
			MFnDependencyNode fnDep(connections[i].node());

			// to get the node name
			tempcolor.texfileInternal = fnDep.name().asChar();
			MPlug filename = fnDep.findPlug("ftn");

			//sparar hela sökvägen till texturen
			tempcolor.texfileExternal = filename.asString().asChar();

			//kopierar texturfiler
			std::string base_filename = tempcolor.texfileExternal.substr(tempcolor.texfileExternal.find_last_of("/\\") + 1);

			CopyFile(tempcolor.texfileExternal.c_str(), base_filename.c_str(), false);

			// stop looping
			break;
		}

	}


}

void Exporter::extractLight(MObject& mObj)
{
	//different space variables
	MSpace::Space world_space = MSpace::kPostTransform;
	MSpace::Space object_space = MSpace::kLast;
	MSpace::Space transform_space = MSpace::kPreTransform;

	//temp storage for light
	pointLightStruct tempPointLights;
	ambientLightStruct tempAmbiLights;
	spotLightStruct tempSpotLights;
	directionalLightStruct tempDirLights;
	areaLightStruct tempAreaLights;

	//binder en ljusfunktion till objektet
	MFnLight func(mObj);

	//dess parent
	MFnDagNode functionParent(func.parent(0));

	//ljusets färg
	MColor col(0.0f, 0.0f, 0.0f);
	//possible fault: func maybe fnlight????
	col = func.color();


	//Get Transform experimentation
	MFnTransform fs(func.parent(0));
	//MMatrix matrix = fs.transformation().asMatrix();
	//std::cout << "\nTransform Matrix: " << matrix << std::endl;

	MVector lightTranslation = fs.translation(transform_space);


	//output light
	std::cout << "parent " << functionParent.name().asChar()
		<< "\ntype " << mObj.apiTypeStr()
		<< "\nLight color " << col.r << " " << col.g << " " << col.b
		<< "\nLight intensity " << func.intensity()
		<< "\nLight direction " << func.lightDirection(0, world_space, 0) << "\n" << std::endl;

	std::cout << "translation: " << lightTranslation << "\n" << std::endl;

	//för specifika attribut till specifika ljustyper:
	switch (mObj.apiType())
	{
		//pointlight-only attributes:
	case MFn::kPointLight:
	{
		MFnPointLight fnPointLight(mObj);
		tempPointLights.color.x = col.r;
		tempPointLights.color.y = col.g;
		tempPointLights.color.z = col.b;
		tempPointLights.intensity = func.intensity();
		tempPointLights.pos = lightTranslation;

		//push back in the light temp storage
		scene_.lights.pointLights.push_back(tempPointLights);

	}
	break;
	//ambientlight-only
	case MFn::kAmbientLight:
	{
		MFnAmbientLight fnAmbientLight(mObj);
		tempAmbiLights.color.x = col.r;
		tempAmbiLights.color.y = col.g;
		tempAmbiLights.color.z = col.b;
		tempAmbiLights.intensity = func.intensity();
		tempAmbiLights.pos = lightTranslation;

		//push back in the light temp storage
		scene_.lights.ambientLights.push_back(tempAmbiLights);

	}
	break;
	//spotlight-only
	case MFn::kSpotLight:
	{
		MFnSpotLight fnSpotLight(mObj);
		//cone angle represents the angle that the spotlight cone makes with the spotlight direction vector
		//penumbra angle is the outer edge of the light 
		//dropoff represents the degree to which the light intensity decreases with the angular distance from the light direction vector.
		std::cout << "Cone angle " << fnSpotLight.coneAngle() << "\nPenumbra angle " << fnSpotLight.penumbraAngle()
			<< "\nDropoff " << fnSpotLight.dropOff() << "\n" << std::endl;

		tempSpotLights.color.x = col.r;
		tempSpotLights.color.y = col.g;
		tempSpotLights.color.z = col.b;
		tempSpotLights.intensity = func.intensity();
		tempSpotLights.coneAngle = fnSpotLight.coneAngle();
		tempSpotLights.penumbraAngle = fnSpotLight.penumbraAngle();
		tempSpotLights.dropoff = fnSpotLight.dropOff();
		tempSpotLights.dir = func.lightDirection(0, MSpace::kWorld, 0);
		tempSpotLights.pos = lightTranslation;

		//push back in the light temp storage
		scene_.lights.spotLights.push_back(tempSpotLights);

	}
	break;
	//directional light-only
	case MFn::kDirectionalLight:
	{
		MFnDirectionalLight fnDirLight(mObj);

		tempDirLights.color.x = col.r;
		tempDirLights.color.y = col.g;
		tempDirLights.color.z = col.b;
		tempDirLights.intensity = func.intensity();
		tempDirLights.dir = func.lightDirection(0, MSpace::kWorld, 0);
		tempDirLights.pos = lightTranslation;

		//push back in the light temp storage
		scene_.lights.dirLights.push_back(tempDirLights);

	}
	break;
	//arealight-only
	case MFn::kAreaLight:
	{
		MFnAreaLight fnAreaLight(mObj);

		tempAreaLights.color.x = col.r;
		tempAreaLights.color.y = col.g;
		tempAreaLights.color.z = col.b;
		tempAreaLights.intensity = func.intensity();
		tempAreaLights.pos = lightTranslation;

		//push back in the light temp storage
		scene_.lights.areaLights.push_back(tempAreaLights);

	}
	break;

	default:
		break;

		//scene_.lights.push_back(TempLightStorage);

	}

}

//void Exporter::extractKeyData(MObject& key)
//{
//	blendKeys temp;
//	WeightFrame frame;
//
//	temp.startFrame = MAnimControl::animationStartTime().value();
//	temp.totalFrames = MAnimControl::animationEndTime().value();
//
//	//get Keyframe values
//	MFnAnimCurve AnimCurve(key);
//
//	std::string type = AnimCurve.name().asChar();
//	if (!strcmp(type.substr(0, 5).c_str(), "blend"))
//	{
//		temp.numKeys = AnimCurve.numKeys();
//
//		// Make sure not to do anything if there are no keyframes
//		if (temp.numKeys == 0) return;
//
//		std::cout << "AnimCurve " << AnimCurve.name().asChar() << std::endl;
//		std::cout << "NumKeys " << temp.numKeys << std::endl;
//		cout << "Start Time: " << MAnimControl::animationStartTime() << endl;
//		cout << "End TIme: " << temp.totalFrames << endl;
//
//		// get all keyframe times & values
//		for (unsigned int i = 0; i < temp.numKeys; i++)
//		{
//			frame.currentFrame = AnimCurve.time(i).value();
//			frame.weight = AnimCurve.value(i);
//
//			// write keyframe info
//			std::cout << " frame " << AnimCurve.time(i);
//			std::cout << " value " << frame.weight << endl;
//
//			temp.WeightF.push_back(frame);
//		}
//		std::cout << std::endl;
//
//		scene_.blendkeys.push_back(temp);
//	}
//}

void Exporter::OutputBlendShapes(MFnBlendShapeDeformer& fn, MObject& Base)
{
	//Output info about the base shape
	MFnDependencyNode fnDep(Base);

	//Write base name
	cout << "\tBase " << fnDep.name().asChar() << endl;

	//attach the function set to the object
	unsigned int nWeights = fn.numWeights();

	cout << "\t\tNumWeights " << nWeights << endl;

	MObjectArray targets;

	//Only want non-history items
	for (unsigned int i = 0; i != nWeights; ++i)
	{
		//fn.getBaseObjects(targets);

		//Get an array of target shapes
		fn.getTargets(Base, i + 1, targets);

		//cout << "Weight unimportant stuff " << fn.weightIndexList(l) << endl;

		cout << "\tCurrent Target: " << i << endl;
		//cout << "\tName:" << name().asChar() << endl;
		cout << "\t\tnumTargets " << targets.length() << endl;

		//output each target shape
		for (unsigned int j = 0; j < targets.length(); ++j)
		{
			outPutTarget(targets[j], Base);
		}
	}
}

void Exporter::outPutTarget(MObject& target, MObject& Base)
{
	//Attach the function set to the object
	MItGeometry it(target);

	BlendShapeTarget temp;
	WeightFrame frame;

	//Write number of points
	cout << "\t\tNumPoints " << it.count() << endl;

	//Iterate through all geometry points
	while (!it.isDone())
	{
		MPoint P = it.position();

		//print point
		cout << "\t\t\t"
			<< P.x << " "
			<< P.y << " "
			<< P.z << endl;
		cout << "\t\t\t" << it.normal() << endl;
		//cout << "\t\t\t" << it.weight() << endl;

		vec3 point = { P.x, P.y, P.z };
		vec3 norm = { it.normal().x, it.normal().y, it.normal().z };

		temp.points.push_back(point);
		temp.normals.push_back(norm);

		it.next();
	}
	cout << endl;

	MItDependencyNodes matIt(MFn::kAnimCurve);
	MFnMesh BlendName(target);
	MFnTransform parent(BlendName.parent(0));
	while (!matIt.isDone())
	{
		MFnAnimCurve AnimCurve(matIt.item());
		cout << parent.name().asChar() << endl;
		cout << AnimCurve.name().substring(AnimCurve.name().length() - parent.name().length(), AnimCurve.name().length()).asChar() << endl;

		if (!strcmp(AnimCurve.name().substring(AnimCurve.name().length() - parent.name().length(), AnimCurve.name().length()).asChar(), parent.name().asChar()))
		{

			//std::string type = AnimCurve.name().substring(BlendName.name().length(), AnimCurve.name().length()).asChar();

			std::string comp = AnimCurve.name().asChar();
			if (!strcmp(comp.substr(0, 5).c_str(), "blend"))
			{
				std::cout << "AnimCurve " << AnimCurve.name().asChar() << std::endl;
				std::cout << "NumKeys " << AnimCurve.numKeys() << std::endl;
				cout << "Start Time: " << MAnimControl::animationStartTime() << endl;
				cout << "End TIme: " << MAnimControl::animationEndTime() << endl;

				// Make sure not to do anything if there are no keyframes
				if (AnimCurve.numKeys() == 0) return;

				temp.WeightF.resize(AnimCurve.numKeys());
				// get all keyframe times & values
				for (unsigned int i = 0; i < AnimCurve.numKeys(); i++)
				{
					temp.WeightF[i].currentFrame = AnimCurve.time(i).value();
					temp.WeightF[i].weight = AnimCurve.value(i);
					// write keyframe info
					std::cout << " frame " << AnimCurve.time(i);
					std::cout << " value " << temp.WeightF[i].weight << endl;
				}

			}
		}
		matIt.next();
	}

	MFnMesh basemfn(Base);

	MDagPath dag_path;
	MItDag dag_iter(MItDag::kBreadthFirst, MFn::kMesh);
	int y = 0;
	while (!dag_iter.isDone())
	{
		if (dag_iter.getPath(dag_path))
		{
			MFnDagNode dag_node = dag_path.node();
			if (!dag_node.isIntermediateObject())
			{

				if (!strcmp(basemfn.partialPathName().asChar(), dag_node.partialPathName().asChar()))
					temp.MeshTarget = y;
				y++;
			}
		}
		dag_iter.next();
	}
	scene_.blendShapes.push_back(temp);
}

// identifierar alla mesharna i scenen och extraherar data från dem
bool Exporter::IdentifyAndExtractMeshes()
{
	UINT index = 0;
	scene_.meshes.clear();

	//itererar över DG:n och lagrar rgba-värden och texturnamn i ett temporärt material
	material tempmaterial;
	MItDependencyNodes matIt(MFn::kLambert);
	MString aC("ambientColor"), dC("color"), sC("specularColor"), gC("incandescence"), tC("transparency");
	while (!matIt.isDone()){
		if (matIt.item().hasFn(MFn::kPhong))
		{
			MFnPhongShader tempphong(matIt.item());
			tempmaterial.type = PHONG;
			extractColor(tempmaterial.ambient, tempphong, aC);
			extractColor(tempmaterial.diffuse, tempphong, dC);
			extractColor(tempmaterial.specular, tempphong, sC);
			extractColor(tempmaterial.glow, tempphong, gC);
			extractColor(tempmaterial.transparency, tempphong, tC);
		}
		else if (matIt.thisNode().hasFn(MFn::kBlinn))
		{
			MFnBlinnShader tempblinn(matIt.item());
			tempmaterial.type = BLINN;
			extractColor(tempmaterial.ambient, tempblinn, aC);
			extractColor(tempmaterial.diffuse, tempblinn, dC);
			extractColor(tempmaterial.specular, tempblinn, sC);
			extractColor(tempmaterial.glow, tempblinn, gC);
			extractColor(tempmaterial.transparency, tempblinn, tC);
		}
		else if (matIt.item().hasFn(MFn::kLambert))
		{
			MFnLambertShader templamb(matIt.item());
			tempmaterial.type = LAMBERT;
			extractColor(tempmaterial.ambient, templamb, aC);
			extractColor(tempmaterial.diffuse, templamb, dC);
			extractColor(tempmaterial.specular, templamb, sC);
			extractColor(tempmaterial.glow, templamb, gC);
			extractColor(tempmaterial.transparency, templamb, tC);
		}
		else
			printf("No material found\n");
		scene_.materials.push_back(tempmaterial);
		matIt.next();
	}

	//Turn off or on Blendshapes
	matIt.reset(MFn::kBlendShape);
	while (!matIt.isDone())
	{
		MFnBlendShapeDeformer bs(matIt.item());

		//Get the envelope attribute plug
		MPlug pl = bs.findPlug("en");

		//Set the 0 to disable FFD effect, enable by setting it to 1:
		pl.setValue(1.0f);

		matIt.next();
	}

	//Get Actual Blendshapes
	matIt.reset(MFn::kBlendShape);
	while (!matIt.isDone())
	{
		MFnBlendShapeDeformer bs(matIt.item());

		MObjectArray base_objects;

		//print blend shape name
		cout << "Blendshape " << bs.name().asChar() << endl;

		//Get a list of objects that this blend shape deforms
		bs.getBaseObjects(base_objects);

		cout << "NumBaseOBjects " << base_objects.length() << endl;

		//loop through each blendshaped object
		for (int i = 0; i < base_objects.length(); ++i)
		{
			//Get the base shape
			MObject Base = base_objects[i];

			//Output all of the target shapes and weights
			OutputBlendShapes(bs, Base);
		}
		//Get next blend shapes
		matIt.next();
	}

	MDagPath dag_path;
	MItDag dag_iter(MItDag::kBreadthFirst, MFn::kMesh);

	while (!dag_iter.isDone())
	{
		if (dag_iter.getPath(dag_path))
		{
			MFnDagNode dag_node = dag_path.node();

			// vill endast ha "icke-history"-föremål
			if (!dag_node.isIntermediateObject())
			{
				// triangulera meshen innan man hämtar punkterna
				MFnMesh mesh(dag_path);
				ExtractMeshData(mesh, index);
				index++;
			}
		}

		dag_iter.next();
	}

	MItDependencyNodes it(MFn::kSkinClusterFilter);
	for (; !it.isDone(); it.next()) {


		MObject object = it.item();


		OutputSkinCluster(object);

	}

	//Hitta kamera data
	dag_iter.reset(dag_iter.root(), MItDag::kBreadthFirst, MFn::kCamera);
	while (!dag_iter.isDone())
	{

		extractCamera(dag_iter.item());
		dag_iter.next();
	}

	//itererar dag och söker data för tillgängliga ljus
	//om ej ljus finns i scenen ignoreras denna iteration för sagda scen.
	dag_iter.reset(dag_iter.root(), MItDag::kBreadthFirst, MFn::kLight);
	while (!dag_iter.isDone())
	{
		//funktion till vår iterator
		MFnLight func(dag_iter.item());

		//namn:
		export_stream_ << "Light: " << func.name().asChar() << std::endl;

		//kalla pEextractLight function
		extractLight(dag_iter.item());

		//vidare till nästa ljus i dag'en
		dag_iter.next();


		/*
		if (dag_iter.getPath(dag_path))
		{
		auto test = dag_path.fullPathName();
		export_stream_ << "light: " << test << std::endl;
		}
		dag_iter.next();
		*/
	}

	dag_iter.reset(dag_iter.root(), MItDag::kBreadthFirst, MFn::kJoint);
	while (!dag_iter.isDone())
	{
		if (dag_iter.getPath(dag_path))
		{
			MFnDagNode dag_node = dag_path.node();

			if (!dag_node.isIntermediateObject())
			{
				extractJointData(dag_path);
			}
		}
		dag_iter.next();
	}

	//general purpose iterator, sista argument är filtret
	/*
		dag_iter.reset(dag_iter.root(), MItDag::kBreadthFirst, MFn::kLight);
		while (!dag_iter.isDone())
		{
		if (dag_iter.getPath(dag_path))
		{

		}
		dag_iter.next();
		}
		*/


	return true;
}

//___________________________________________________________________________________________
//|																							|
//|								FUNKTIONER FÖR ATT EXTRAHERA DATA							|
//|_________________________________________________________________________________________|


// hämta all nödvändig data och lägger det i ett MeshData-objekt, som senare används vid exportering.
bool Exporter::ExtractMeshData(MFnMesh &mesh, UINT index)
{
	MeshData mesh_data;

	MFloatPointArray points;

	MFloatVectorArray normals;

	MSpace::Space world_space = MSpace::kWorld;

	// DAG-path
	mesh_data.mesh_path = mesh.dagPath();

	// namn och id
	mesh_data.name = mesh.name();
	mesh_data.id = index;

	std::string name = mesh.partialPathName().asChar();
	if (!strcmp(name.substr(0, 5).c_str(), "Blend")){
		return true;
	}

	// triangulera meshen innan man hämtar punkterna
	MString command = "polyTriangulate -ch 1 " + mesh_data.name;
	if (!MGlobal::executeCommand(command))
	{
		return false;
	}

	// hämta icke-indexerade vertexpunkter
	if (!mesh.getPoints(points, world_space))
	{
		return false;
	}

	for (int i = 0; i < points.length(); i++){
		point temppoints = { points[i].x, -points[i].y, points[i].z };
		vec3 temppurepoints = { points[i].x, -points[i].y, points[i].z };
		mesh_data.points.push_back(temppoints);
		mesh_data.purepoints.push_back(temppurepoints);
	}

	// hämta icke-indexerade normaler
	if (!mesh.getNormals(normals, world_space))
	{
		return false;
	}

	for (int i = 0; i < normals.length(); i++){
		vec3 tempnormals = { normals[i].x, -normals[i].y, normals[i].z };
		mesh_data.normals.push_back(tempnormals);
	}

	//variabler för att mellanlagra uvdata och tangenter/bitangenter
	MStringArray uvSets;
	mesh.getUVSetNames(uvSets);

	uvSet tempUVSet;
	MFloatArray Us;
	MFloatArray Vs;
	vec2 UVs;

	// iterera över uvsets och ta ut koordinater, tangenter och bitangenter
	for (int i = 0; i < uvSets.length(); i++)
	{
		MString currentSet = uvSets[i];
		mesh.getUVs(Us, Vs, &currentSet);
		for (int a = 0; a < Us.length(); a++){
			UVs.u = Us[a];
			UVs.v = 1.0f - Vs[a];
			//1-Vs in order to get correct UV angles
			tempUVSet.UVs.push_back(UVs);
		}
		mesh.getTangents(tempUVSet.tangents, world_space, &currentSet);
		mesh.getBinormals(tempUVSet.binormals, world_space, &currentSet);
		mesh_data.uvSets.push_back(tempUVSet);
	}

	//itererar över trianglar och returnerar ID:n för associerade vertiser, normaler och uvset
	MItMeshPolygon itFaces(mesh.dagPath());
	while (!itFaces.isDone()) {
		face tempface;

		//		printf("%d", itFaces.vertexIndex(0));
		//		printf(" %d", itFaces.vertexIndex(1));
		//		printf(" %d\n", itFaces.vertexIndex(2));

		int vc = itFaces.polygonVertexCount();
		for (int i = 0; i < vc; ++i) {
			tempface.verts[i].pointID = itFaces.vertexIndex(i);
			tempface.verts[i].normalID = itFaces.normalIndex(i);

			for (int k = 0; k < uvSets.length(); ++k) {
				int temptexCoordsID;
				itFaces.getUVIndex(i, temptexCoordsID, &uvSets[k]);

				tempface.verts[i].texCoordsID.push_back(temptexCoordsID);
			}
		}

		mesh_data.faces.push_back(tempface);
		itFaces.next();
	}

	// lägg till mesh_data i scen-datan
	scene_.meshes.push_back(mesh_data);

	return true;
}


void Exporter::RecursiveJointExtraction(MFnTransform& joint, int parentIndex){

	Bone output;
	output.parent = parentIndex;

	output.invBindPose = joint.transformation().asMatrixInverse().matrix;
	MVector test = joint.translation(MSpace::kWorld);


	MPlug tempBindPosePlug = joint.findPlug("bindPose");

	output.BindPose = joint.transformation().asMatrix().matrix;
	


	MItDependencyNodes matIt(MFn::kAnimCurve);
	while (!matIt.isDone())
	{
		MFnAnimCurve animCurve(matIt.item());
		cout << animCurve.name().asChar() << endl;

		if (!strcmp(animCurve.name().substring(0, joint.name().length() - 1).asChar(), joint.name().asChar())){

			std::string type = animCurve.name().substring(joint.name().length(), animCurve.name().length()).asChar();
			output.frames.resize(animCurve.numKeys());
			for (int i = 0; i < animCurve.numKeys(); i++)
			{
				output.frames[i].time = animCurve.time(i).value();
				if (!strcmp(type.c_str(), "_translateX")){
					cout << animCurve.value(i) << endl;
					output.frames[i].trans.x = animCurve.value(i);
				}
				if (!strcmp(type.c_str(), "_translateY")){
					cout << animCurve.value(i) << endl;
					output.frames[i].trans.y = animCurve.value(i);
				}
				if (!strcmp(type.c_str(), "_translateZ")){
					cout << animCurve.value(i) << endl;
					output.frames[i].trans.z = animCurve.value(i);
				}
				if (!strcmp(type.c_str(), "_rotateX")){
					cout << animCurve.value(i) << endl;
					output.frames[i].rot.x = animCurve.value(i);
				}
				if (!strcmp(type.c_str(), "_rotateY")){
					cout << animCurve.value(i) << endl;
					output.frames[i].rot.y = animCurve.value(i);
				}
				if (!strcmp(type.c_str(), "_rotateZ")){
					cout << animCurve.value(i) << endl;
					output.frames[i].rot.z = animCurve.value(i);
				}
			}
		}
		matIt.next();
	}


	scene_.skeleton.push_back(output);
	int children = joint.childCount();
	for (int i = 0; i < children; i++)
		RecursiveJointExtraction(MFnTransform(joint.child(i)), scene_.skeleton.size());

};

void Exporter::extractJointData(MDagPath path)
{
	MFnTransform joint(path);
	int childcount = joint.childCount();
	MFnDagNode rootpath(joint.parent(0));

	if (!strcmp(rootpath.fullPathName().asChar(), ""))
		RecursiveJointExtraction(joint, -1);




	// 	MFloatMatrix res;
	// 	cout << path.partialPathName().asChar() << std::endl;
	// 	MMatrix invMat;
	// 
	// 	MStatus status;
	// 	MTransformationMatrix restpose = joint.restPosition(&status);
	// 
	// 	cout << status <<" "<< restpose.asMatrix() << endl;
	// 
	// 
	// 	jointTrans jt;
	// 
	// 	//attach the function set to the object
	// 	MFnTransform tr(path);
	// 
	// 	// Gets transform data as a matrix, though quaternions more interesting! :D
	// 	MMatrix mat = tr.transformation().asMatrix();
	// 
	// 	MQuaternion JointOrient(0, 0, 0, 1);
	// 	MQuaternion Rotation(0, 0, 0, 1);
	// 
	// 	//Get the transforms local translation
	// 	MVector Translation = tr.translation(MSpace::kTransform);
	// 
	// 	//Get the transforms scale
	// 	tr.getScale(jt.scale);
	// 
	// 	//Get the transforms rotation as quaternions
	// 	tr.getRotation(Rotation);
	// 
	// 	//IK joints contains both joint orientations as well as a rotation, therefore I check for the transform of an IK
	// 	if (tr.object().hasFn(MFn::kJoint))
	// 	{
	// 		MFnIkJoint IKjoint(tr.object());
	// 		IKjoint.getOrientation(JointOrient);
	// 	}
	// 
	// 	//Get Translation data
	// 	jt.tx = Translation.x;
	// 	jt.ty = Translation.y;
	// 	jt.tz = Translation.z;
	// 
	// 	//Get Rotation Data
	// 	jt.rx = Rotation.x;
	// 	jt.ry = Rotation.y;
	// 	jt.rz = Rotation.z;
	// 	jt.rw = Rotation.w;
	// 
	// 	//Get Joint Orientation Data
	// 	jt.rox = JointOrient.x;
	// 	jt.roy = JointOrient.y;
	// 	jt.roz = JointOrient.z;
	// 	jt.row = JointOrient.w;
	// 
	// 
	// 	MObject jointNode = path.node();
	// 	MFnDependencyNode fnJoint(jointNode);
	// 	MObject attrWorldMatrix = fnJoint.attribute("worldMatrix");
	// 
	// 	MPlug plugWorldMatrixArray(jointNode, attrWorldMatrix);
	// 
	// 	for (unsigned i = 0; i < plugWorldMatrixArray.numElements(); i++)
	// 	{
	// 		MPlug elementPlug = plugWorldMatrixArray[i];
	// 
	// 		MItDependencyGraph dgIt(elementPlug, MFn::kInvalid, MItDependencyGraph::kDownstream, MItDependencyGraph::kDepthFirst, MItDependencyGraph::kPlugLevel);
	// 
	// 		dgIt.disablePruningOnFilter();
	// 
	// 		for (; !dgIt.isDone(); dgIt.next())
	// 		{
	// 			MObject thisNode = dgIt.thisNode();
	// 
	// 			if (thisNode.apiType() == MFn::kSkinClusterFilter)
	// 			{
	// 				MFnSkinCluster skinFn(thisNode);
	// 
	// 				MPlug bindPreMatrixArrayPlug = skinFn.findPlug("bindPreMatrix");
	// 				int logicalIndex = skinFn.indexForInfluenceObject(path);
	// 				MPlug bindPreMatrixPlug = bindPreMatrixArrayPlug.elementByLogicalIndex(logicalIndex);
	// 				MObject dataObject;
	// 				bindPreMatrixArrayPlug.getValue(dataObject);
	// 
	// 				MFnMatrixData matDataFn(dataObject);
	// 
	// 				invMat = matDataFn.matrix().inverse();
	// 				res = invMat.matrix;
	// 				cout << logicalIndex << std::endl;
	// 			}
	// 		}
	// 	}
	// 	std::cout << res << std::endl;
}

void Exporter::OutputSkinCluster(MObject& obj)
{
	// attach a skin cluster function set to
	// access the data
	skinData SD;
	MFnSkinCluster fn(obj);
	MDagPathArray infs;

	//Get influences
	SD.influences = fn.influenceObjects(infs);

	// loop through the geometries affected by this cluster
	int nGeoms = fn.numOutputConnections();
	for (int i = 0; i < nGeoms; ++i) {
		unsigned int index;
		index = fn.indexForOutputConnection(i);

		// get the dag path of the i'th geometry
		MDagPath skinPath;
		fn.getPathAtIndex(index, skinPath);

		// iterate through the components of this geometry
		MItGeometry gIter(skinPath);

		// print out the name of the skin cluster,
		// the vertexCount and the influenceCount
		cout << "Skin: " << skinPath.partialPathName().asChar() << endl;
		cout << "pointcount: " << gIter.count() << endl;
		cout << "numInfluences: " << SD.influences << endl;

		//Get points affected
		SD.points = gIter.count();

		for (; !gIter.isDone(); gIter.next()) {

			MObject comp = gIter.component();
			// Get the weights for this vertex (one per influence object)
			//
			MFloatArray wts;
			unsigned int infCount;
			fn.getWeights(skinPath, comp, wts, infCount);
			if (0 != infCount && !gIter.isDone())
			{
				int numWeights = 0;
				float outWts[40] = { 1.0f, 0 };
				int outInfs[40] = { 0 };

				// Output the weight data for this vertex
				//
				for (int j = 0; j != infCount; ++j)
				{
					// ignore weights of little effect
					if (wts[j] > 0.001f)
					{
						if (numWeights != 0)
						{
							outWts[0] -= wts[j];
							outWts[numWeights] = wts[j];
						}
						outInfs[numWeights] = j;
						++numWeights;
					}
				}
				float norm = outWts[0] + outWts[1] + outWts[2] + outWts[3];


				MDagPath dag_path;
				MItDag dag_iter(MItDag::kBreadthFirst, MFn::kMesh);
				int currentmesh = 0, y = 0;
				while (!dag_iter.isDone())
				{
					if (dag_iter.getPath(dag_path))
					{
						MFnDagNode dag_node = dag_path.node();
						if (!dag_node.isIntermediateObject())
						{
							MFnMesh mesh(dag_path);
							if (!strcmp(skinPath.partialPathName().asChar(), mesh.partialPathName().asChar()))
								currentmesh = y;
							y++;
						}
					}

					dag_iter.next();
				}


				for (int x = 0; x < 4; x++)
				{
					scene_.meshes[currentmesh].points[gIter.index()].boneIndices[x] = outInfs[x];
					scene_.meshes[currentmesh].points[gIter.index()].boneWeigths[x] = outWts[x] / norm;
				}
				scene_.meshes[currentmesh].hasSkeleton = true;
			}
		}
	}
}


//___________________________________________________________________________________________
//|																							|
//|								FUNKTIONER FÖR ATT EXPORTERA								|
//|_________________________________________________________________________________________|
//exporterar allt inom level-filen: mesh-antal, namn per mesh, antal etc etc
void Exporter::ExportLevel()
{
	for (std::string formattedOutput : formattedLevelData)
	{
		export_stream_ << formattedOutput << std::endl;
	}
}

#include<maya/MFnCamera.h>
// exporterar scenen
void Exporter::ExportScene()
{
	ExportMeshes();
}

// exporterar alla meshar (antal meshar, namn per mesh, antal vertexpunkter per mesh och vertexpositioner per mesh)
void Exporter::ExportMeshes()
{


	export_stream_ << "\tmeshes " << scene_.meshes.size() << std::endl;
	for (auto mesh_iter = scene_.meshes.begin(); mesh_iter != scene_.meshes.end(); mesh_iter++)
	{
		export_stream_ << "\t\tname " << mesh_iter->name.asChar() << std::endl;

		MItMeshPolygon polygon_iter(mesh_iter->mesh_path);
		int vertexIndex = 0;

		export_stream_ << "\t\t\tvertices " << polygon_iter.count() * 3 << std::endl;

		for (int i = 0; i < mesh_iter->points.size(); i++){
			export_stream_ << "v " << mesh_iter->points[i].x << " " << mesh_iter->points[i].y << " " << mesh_iter->points[i].z << std::endl;
		}
		for (int i = 0; i < mesh_iter->uvSets[0].UVs.size(); i++){
			export_stream_ << "vt " << mesh_iter->uvSets[0].UVs[i].u << " " << mesh_iter->uvSets[0].UVs[i].v << std::endl;
		}
		for (int i = 0; i < mesh_iter->normals.size(); i++){
			export_stream_ << "vn " << mesh_iter->normals[i].x << " " << mesh_iter->normals[i].y << " " << mesh_iter->normals[i].z << std::endl;
		}
		for (int i = 0; i < mesh_iter->faces.size(); i++){
			export_stream_ << "f " << mesh_iter->faces[i].verts[0].pointID + 1 << "/" << mesh_iter->faces[i].verts[0].texCoordsID[0] + 1 << "/" << mesh_iter->faces[i].verts[0].normalID + 1 <<
				" " << mesh_iter->faces[i].verts[1].pointID + 1 << "/" << mesh_iter->faces[i].verts[1].texCoordsID[0] + 1 << "/" << mesh_iter->faces[i].verts[1].normalID + 1 <<
				" " << mesh_iter->faces[i].verts[2].pointID + 1 << "/" << mesh_iter->faces[i].verts[2].texCoordsID[0] + 1 << "/" << mesh_iter->faces[i].verts[2].normalID + 1
				<< std::endl;
		}
	}


	MainHeader mainHeader;
	mainHeader.meshCount = scene_.meshes.size();
	mainHeader.blendShapeCount = scene_.blendShapes.size();
	mainHeader.matCount = scene_.materials.size();
	mainHeader.pointLightSize = scene_.lights.pointLights.size();
	mainHeader.ambientLightSize = scene_.lights.ambientLights.size();
	mainHeader.areaLightSize = scene_.lights.areaLights.size();
	mainHeader.spotLightSize = scene_.lights.spotLights.size();
	mainHeader.dirLightSize = scene_.lights.dirLights.size();
	mainHeader.camCount = scene_.cameras.size();
	mainHeader.boneCount = scene_.skeleton.size();

	outfile.write((const char*)&mainHeader, sizeof(MainHeader));

	for (int i = 0; i < mainHeader.meshCount; i++){
		MeshHeader meshHeader;
		meshHeader.nameLength = scene_.meshes[i].name.length();
		meshHeader.numberFaces = scene_.meshes[i].faces.size();
		meshHeader.numberPoints = scene_.meshes[i].points.size();
		meshHeader.numberNormals = scene_.meshes[i].normals.size();
		meshHeader.numberCoords = scene_.meshes[i].uvSets[0].UVs.size();
		meshHeader.hasSkeleton = scene_.meshes[i].hasSkeleton;

		outfile.write((const char*)&meshHeader, sizeof(MeshHeader));
		outfile.write((const char*)scene_.meshes[i].name.asChar(), meshHeader.nameLength);
		if (meshHeader.hasSkeleton)
			outfile.write((const char*)scene_.meshes[i].points.data(), meshHeader.numberPoints*sizeof(point));
		else
			outfile.write((const char*)scene_.meshes[i].purepoints.data(), meshHeader.numberPoints*sizeof(vec3));
		outfile.write((const char*)scene_.meshes[i].normals.data(), meshHeader.numberNormals*sizeof(vec3));
		outfile.write((const char*)scene_.meshes[i].uvSets[0].UVs.data(), meshHeader.numberCoords*sizeof(vec2));
		for (int a = 0; a < meshHeader.numberFaces; a++){
			for (int b = 0; b < 3; b++){
				outfile.write((const char*)&scene_.meshes[i].faces[a].verts[b].pointID, 4);
				outfile.write((const char*)&scene_.meshes[i].faces[a].verts[b].normalID, 4);
				outfile.write((const char*)&scene_.meshes[i].faces[a].verts[b].texCoordsID[0], 4);
			}
		}
	}

	for (int i = 0; i < mainHeader.matCount; i++){
		MatHeader matHeader;
		matHeader.ambientNameLength = scene_.materials[i].ambient.texfileInternal.length();
		matHeader.diffuseNameLength = scene_.materials[i].diffuse.texfileInternal.length();
		matHeader.specularNameLength = scene_.materials[i].specular.texfileInternal.length();
		matHeader.transparencyNameLength = scene_.materials[i].transparency.texfileInternal.length();
		matHeader.glowNameLength = scene_.materials[i].glow.texfileInternal.length();


		outfile.write((const char*)&matHeader, sizeof(MatHeader));
		outfile.write((const char*)&scene_.materials[i].ambient, 16);
		outfile.write((const char*)scene_.materials[i].ambient.texfileInternal.data(), matHeader.ambientNameLength);

		outfile.write((const char*)&scene_.materials[i].diffuse, 16);
		outfile.write((const char*)scene_.materials[i].diffuse.texfileInternal.data(), matHeader.diffuseNameLength);

		outfile.write((const char*)&scene_.materials[i].specular, 16);
		outfile.write((const char*)scene_.materials[i].specular.texfileInternal.data(), matHeader.specularNameLength);

		outfile.write((const char*)&scene_.materials[i].transparency, 16);
		outfile.write((const char*)scene_.materials[i].transparency.texfileInternal.data(), matHeader.transparencyNameLength);

		outfile.write((const char*)&scene_.materials[i].glow, 16);
		outfile.write((const char*)scene_.materials[i].glow.texfileInternal.data(), matHeader.glowNameLength);
	}

	outfile.write((const char*)scene_.lights.ambientLights.data(), mainHeader.ambientLightSize*sizeof(ambientLightStruct));
	outfile.write((const char*)scene_.lights.areaLights.data(), mainHeader.areaLightSize*sizeof(areaLightStruct));
	outfile.write((const char*)scene_.lights.dirLights.data(), mainHeader.dirLightSize*sizeof(directionalLightStruct));
	outfile.write((const char*)scene_.lights.pointLights.data(), mainHeader.pointLightSize*sizeof(pointLightStruct));
	outfile.write((const char*)scene_.lights.spotLights.data(), mainHeader.spotLightSize*sizeof(spotLightStruct));

	for (int i = 0; i < mainHeader.camCount; i++){
		outfile.write((const char*)&scene_.cameras[i], 52);
	}

	for (int i = 0; i < mainHeader.blendShapeCount; i++){
		outfile.write((const char*)&scene_.blendShapes[i].MeshTarget, 4);
		outfile.write((const char*)scene_.blendShapes[i].points.data(), sizeof(vec3)*scene_.meshes[scene_.blendShapes[i].MeshTarget].points.size());
		outfile.write((const char*)scene_.blendShapes[i].normals.data(), sizeof(vec3)*scene_.meshes[scene_.blendShapes[i].MeshTarget].normals.size());
	}

	for (int i = 0; i < mainHeader.boneCount; i++){
		outfile.write((const char*)&scene_.skeleton[i].parent, 4);
		outfile.write((const char*)&scene_.skeleton[i].BindPose, 64);
		outfile.write((const char*)&scene_.skeleton[i].invBindPose, 64);

		int frames = scene_.skeleton[i].frames.size();

		outfile.write((const char*)&frames, 4);

		outfile.write((const char*)scene_.skeleton[i].frames.data(), sizeof(Keyframe)*frames);
	}

	outfile.close();
	debug++;
	/*
			// How to read
			// first read the header
			MainHeader readMainHeader;
			infile.read((char*)&readMainHeader, sizeof(MainHeader));

			for (int i = 0; i < readMainHeader.meshCount; i++){
			MeshHeader meshHeader;
			infile.read((char*)&meshHeader, sizeof(MeshHeader));

			infile.read((char*)&scene_.meshes[i].name, meshHeader.nameLength);
			outfile << scene_.meshes[i].name << std::endl;

			infile.read((char*)&scene_.meshes[i].points, meshHeader.numberPoints*sizeof(vec3));
			for (int a = 0; a < meshHeader.numberPoints; a++){
			outfile << scene_.meshes[i].points[a].x << " " << scene_.meshes[i].points[a].y << " " << scene_.meshes[i].points[a].z << std::endl;
			}

			infile.read((char*)&scene_.meshes[i].normals, meshHeader.numberNormals*sizeof(vec3));
			for (int a = 0; a < meshHeader.numberNormals; a++){
			outfile << scene_.meshes[i].normals[a].x << " " << scene_.meshes[i].normals[a].y << " " << scene_.meshes[i].normals[a].z << std::endl;
			}

			infile.read((char*)&scene_.meshes[i].uvSets[0].UVs, meshHeader.numberCoords*sizeof(vec2));
			for (int a = 0; a < meshHeader.numberCoords; a++){
			outfile << scene_.meshes[i].uvSets[0].UVs[a].u << " " << scene_.meshes[i].uvSets[0].UVs[a].v << std::endl;
			}

			for (int a = 0; a < meshHeader.numberFaces; a++){
			for (int b = 0; b < 3; b++){
			infile.read((char*)&scene_.meshes[i].faces[a].verts[b].pointID, 4);
			infile.read((char*)&scene_.meshes[i].faces[a].verts[b].normalID, 4);
			infile.read((char*)&scene_.meshes[i].faces[a].verts[b].texCoordsID[0], 4);
			}
			}
			for (int a = 0; a < meshHeader.numberFaces; a++){
			outfile << scene_.meshes[i].faces[a].verts[0].pointID << "/" << scene_.meshes[i].faces[a].verts[0].texCoordsID[0] << "/" << scene_.meshes[i].faces[a].verts[0].normalID << " "
			<< scene_.meshes[i].faces[a].verts[1].pointID << "/" << scene_.meshes[i].faces[a].verts[1].texCoordsID[0] << "/" << scene_.meshes[i].faces[a].verts[1].normalID << " "
			<< scene_.meshes[i].faces[a].verts[2].pointID << "/" << scene_.meshes[i].faces[a].verts[2].texCoordsID[0] << "/" << scene_.meshes[i].faces[a].verts[2].normalID << " "
			<< std::endl;
			}
			}
			infile.close();
			*/


	/*
			int j = 0;
			while (j != 2)
			{
			while (!polygon_iter.isDone ())
			{
			MIntArray index_array;
			polygon_iter.getVertices (index_array);
			if (index_array.length () == 3)
			{
			for (int i = 0; i < 3; i++)
			{
			if (j == 0)
			{
			vertexIndex = polygon_iter.vertexIndex (2 - i);
			export_stream_ << "\t\t\t\tp " <<
			mesh_iter->points [vertexIndex].x << " " <<
			mesh_iter->points [vertexIndex].y << " " <<
			mesh_iter->points [vertexIndex].z << std::endl;
			}
			else
			{
			export_stream_ << "\t\t\t\tn " <<
			mesh_iter->normals [vertexIndex].x << " " <<
			mesh_iter->normals [vertexIndex].y << " " <<
			mesh_iter->normals [vertexIndex].z << std::endl;
			}
			}
			}
			else
			{
			std::cout << "Error: non-triangular polygon detected. Please only use triangles." << std::endl;
			std::cout << "Attempts to continue export with missing polygon..." << std::endl;
			}
			polygon_iter.next ();
			}
			//Reset Polygon iterationen för normalerna
			polygon_iter.reset ();
			j++;
			}


			for (int i = 0; i < mesh_iter->uvSets.size (); i++)
			{
			export_stream_ << "\t\t\tUVSet " << i << std::endl;

			for (int x = 0; x < mesh_iter->uvSets [i].Us.length (); x++)
			{
			export_stream_ << "\t\t\t\tuv " << mesh_iter->uvSets [i].Us [x] << " " << mesh_iter->uvSets [i].Vs [x] << std::endl;
			}

			for (int x = 0; x < mesh_iter->uvSets [i].Us.length (); x++)
			{
			export_stream_ << "\t\t\t\tt " << mesh_iter->uvSets [i].tangents [x] << std::endl;
			}

			for (int x = 0; x < mesh_iter->uvSets [i].Us.length (); x++)
			{
			export_stream_ << "\t\t\t\tb " << mesh_iter->uvSets [i].binormals [x] << std::endl;
			}
			}
			*/

}

void Exporter::splitStringToVector(std::string input, std::vector<std::string> &output, std::string delimiter) {
	size_t pos = 0;
	std::string token;
	while ((pos = input.find(delimiter)) != std::string::npos) {
		token = input.substr(0, pos);
		if (!token.empty()) {
			output.push_back(token);
		}
		input.erase(0, pos + delimiter.length());
	}
	output.push_back(input);
};

//turns a text file into a vector of strings line-by-line
void Exporter::fileToStrings(std::string file_path, std::vector<std::string> &output)
{
	std::ifstream file(file_path);
	if (!file.is_open())
	{
		printf("Could not open ");
		printf(file_path.c_str());
		printf("\n");
		return;
	}
	std::string s((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	splitStringToVector(s, output, "\n");
};
