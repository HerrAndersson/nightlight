// Jonas Petersson
// jnp@bth.se
// 14-03-31

//Länkar

// http://nccastaff.bournemouth.ac.uk/jmacey/RobTheBloke/www/research/maya/mfnmaterial.htm
// http://help.autodesk.com/view/MAYAUL/2015/ENU/

#include "Exporter.h"

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

bool Exporter::CreateExportFiles(std::string file_path)
{
	// hitta index för punkten innan filtypen.
	int sub_string_length = (int)file_path.find_last_of(".", file_path.size() - 1);

	// spargenväg för den exporterade filen.
	std::string save_path = file_path.substr(0, sub_string_length) + ".txt";

	std::cout << "Exporting file to " << save_path.c_str() << std::endl << std::endl;

	export_stream_.open(save_path.c_str(), std::ios_base::out | std::ios_base::trunc);
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

	// även en "tom" katalog ger två resultat - . och ..
	// vi vill inte ha dem :-)
	//fileList.push_back(fdata.cFileName);

	while (true)
	{
		if (FindNextFile(dhandle, &fdata))
		{
			// vi vill endast ha ".mb"-filer
			if (strlen(fdata.cFileName) > 4)
			{
				if (strcmp(&fdata.cFileName[strlen(fdata.cFileName)-3], ".mb") == 0)
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

		ProcessScene(tmp_str);

		CloseExportFiles();
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
	if (!CreateExportFiles(file_path))
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
}

//___________________________________________________________________________________________
//|																							|
//|								FUNKTIONER FÖR ATT IDENTIFIERA DATA							|
//|_________________________________________________________________________________________|

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


	//aspect ratio
	std::cout << "\nAspect ratio: " << fn.aspectRatio()
			<< std::endl;
	
	//near clipping plane
	std::cout << "\nNear: : " << fn.nearClippingPlane()
				<< std::endl;
	
	//far clipping plane
	std::cout << "\nFar: " << fn.farClippingPlane()
		<< std::endl;
	
	//horizontal field of view
	std::cout << "\nHorizontal fov: " << fn.horizontalFieldOfView()
		<< std::endl;

	//vertical field of view
	std::cout << "\nVertical fov: " << fn.verticalFieldOfView()
		<< std::endl;

	//badly formated Projection matrix
	std::cout << "\nProjectionMatrix: " << fn.projectionMatrix()
		<< std::endl;
	TempCameraStorage.projectionMatrix = fn.projectionMatrix();


	//Up direction
	std::cout << "\nUp Vector: " << fn.upDirection()
		<< std::endl;
	TempCameraStorage.upVector = fn.upDirection();

	//view direction
	std::cout << "\nView Direction: " << fn.viewDirection()
		<< std::endl;
	TempCameraStorage.viewDirection = fn.viewDirection();
	
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

			for (int i = 0; i!=connections.length(); ++i)
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



					// stop looping
					break;

				}

			}


}

void Exporter::extractLight(MObject& mObj)
{
	//binder en ljusfunktion till objektet
	MFnLight func (mObj);

	//dess parent
	MFnDagNode functionParent (func.parent(0));

	//ljusets färg
	MColor col(0.0f, 0.0f, 0.0f);
	//possible fault: func maybe fnlight????
	col = func.color();

	//output light
	std::cout << "parent " << functionParent.name().asChar()
		<< "\ntype " << mObj.apiTypeStr()
		<< "\nLight color " << col.r << " " << col.g << " " << col.b
		<< "\nLight intensity " << func.intensity()
		<< "\nLight direction " << func.lightDirection (0, MSpace::kWorld, 0) << "\n" << std::endl;

	//för specifika attribut till specifika ljustyper:
	switch (mObj.apiType ())
	{
			//pointlight-only attributes:
		case MFn::kPointLight:
		{
			MFnPointLight fnPointLight(mObj);
		}
			break;
			//ambientlight-only
		case MFn::kAmbientLight:
		{
			MFnAmbientLight fnAmbientLight(mObj);
		}
			break;
			//spotlight-only
		case MFn::kSpotLight:
		{
			MFnSpotLight fnSpotLight(mObj);
		}
			break;
			//directional light-only
		case MFn::kDirectionalLight:
		{
			MFnDirectionalLight fnDirLight(mObj);
		}
			break;
			//arealight-only
		case MFn::kAreaLight:
		{
			MFnAreaLight fnAreaLight (mObj);
		}
			break;

		default:
			break;

	}

}

// identifierar alla mesharna i scenen och extraherar data från dem
bool Exporter::IdentifyAndExtractMeshes()
{
	UINT index = 0;
	scene_.meshes.clear();

	//itererar över DG:n och lagrar rgba värden i ett temporärt material
	material tempmaterial;
	MItDependencyNodes matIt(MFn::kLambert);
	MString aC("ambientColor"), dC("color"), sC("specularColor"), gC("incandescence"), tC("transparency");
	while (!matIt.isDone()){
		if (matIt.item().hasFn(MFn::kPhong))
		{
			MFnPhongShader tempphong(matIt.item());
			extractColor(tempmaterial.ambient, tempphong, aC);
			extractColor(tempmaterial.diffuse, tempphong, dC);
			extractColor(tempmaterial.specular, tempphong, sC);
			extractColor(tempmaterial.glow, tempphong, gC);
			extractColor(tempmaterial.transparency, tempphong, tC);
		}
		else if (matIt.thisNode().hasFn(MFn::kBlinn))
		{
			MFnBlinnShader tempblinn(matIt.item());
			extractColor(tempmaterial.ambient, tempblinn, aC);
			extractColor(tempmaterial.diffuse, tempblinn, dC);
			extractColor(tempmaterial.specular, tempblinn, sC);
			extractColor(tempmaterial.glow, tempblinn, gC);
			extractColor(tempmaterial.transparency, tempblinn, tC);
		}
		else if (matIt.item().hasFn(MFn::kLambert))
		{
			MFnLambertShader templamb(matIt.item());
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

	//Hitta kamera data
	dag_iter.reset(dag_iter.root(), MItDag::kBreadthFirst, MFn::kCamera);
	while (!dag_iter.isDone())
	{
		
		extractCamera(dag_iter.item());
		dag_iter.next();
	}

	//itererar dag och söker namn för tillgängliga ljus
	dag_iter.reset(dag_iter.root(), MItDag::kBreadthFirst, MFn::kLight);
	while (!dag_iter.isDone())
	{
		//funktion till vår iterator
		MFnLight func (dag_iter.item());

		//namn:
		export_stream_ << "Light: " << func.name ().asChar () << std::endl;

		//kalla på lightOutput function
		extractLight(dag_iter.item ());

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

	MSpace::Space world_space = MSpace::kWorld;

	// DAG-path
	mesh_data.mesh_path = mesh.dagPath();

	// namn och id
	mesh_data.name = mesh.name();
	mesh_data.id = index;

	// triangulera meshen innan man hämtar punkterna
	MString command = "polyTriangulate -ch 1 " + mesh_data.name;
	if (!MGlobal::executeCommand(command))
	{
		return false;
	}

	// hämta icke-indexerade vertexpunkter
	if (!mesh.getPoints(mesh_data.points, world_space))
	{
		return false;
	}

	// hämta icke-indexerade normaler
	if (!mesh.getNormals(mesh_data.normals, world_space))
	{
		return false;
	}

	//variabler för att mellanlagra uvdata och tangenter/bitangenter
	MStringArray uvSets;
	mesh.getUVSetNames(uvSets);

	uvSet tempUVSet;
	// iterera över uvsets och ta ut koordinater, tangenter och bitangenter
	for (int i = 0; i < uvSets.length(); i++)
	{
		MString currentSet = uvSets[i];
		mesh.getUVs(tempUVSet.Us, tempUVSet.Vs, &currentSet);
		mesh_data.uvSets.push_back(tempUVSet);

		mesh.getTangents(mesh_data.uvSets[i].tangents, world_space, &currentSet);
		mesh.getBinormals(mesh_data.uvSets[i].binormals, world_space, &currentSet);
	}

	// lägg till mesh_data i scen-datan
	scene_.meshes.push_back(mesh_data);

	return true;
}


//___________________________________________________________________________________________
//|																							|
//|								FUNKTIONER FÖR ATT EXPORTERA								|
//|_________________________________________________________________________________________|
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

		int j = 0;
		while (j != 2)
		{
			while (!polygon_iter.isDone())
			{
				MIntArray index_array;
				polygon_iter.getVertices(index_array);
				if (index_array.length() == 3)
				{
					for (int i = 0; i < 3; i++)
					{
						if (j == 0)
						{
							vertexIndex = polygon_iter.vertexIndex(2 - i);
							export_stream_ << "\t\t\t\tp " <<
								mesh_iter->points[vertexIndex].x << " " <<
								mesh_iter->points[vertexIndex].y << " " <<
								mesh_iter->points[vertexIndex].z << std::endl;
						}
						else
						{
							export_stream_ << "\t\t\t\tn " <<
								mesh_iter->normals[vertexIndex].x << " " <<
								mesh_iter->normals[vertexIndex].y << " " <<
								mesh_iter->normals[vertexIndex].z << std::endl;
						}
					}
				}
				else
				{
					std::cout << "Error: non-triangular polygon detected. Please only use triangles." << std::endl;
					std::cout << "Attempts to continue export with missing polygon..." << std::endl;
				}
				polygon_iter.next();
			}
			//Reset Polygon iterationen för normalerna
			polygon_iter.reset();
			j++;
		}


		for (int i = 0; i < mesh_iter->uvSets.size(); i++)
		{
			export_stream_ << "\t\t\tUVSet " << i << std::endl;

			for (int x = 0; x < mesh_iter->uvSets[i].Us.length(); x++)
			{
				export_stream_ << "\t\t\t\tuv " << mesh_iter->uvSets[i].Us[x] << " " << mesh_iter->uvSets[i].Vs[x] << std::endl;
			}

			for (int x = 0; x < mesh_iter->uvSets[i].Us.length(); x++)
			{
				export_stream_ << "\t\t\t\tt " << mesh_iter->uvSets[i].tangents[x] << std::endl;
			}

			for (int x = 0; x < mesh_iter->uvSets[i].Us.length(); x++)
			{
				export_stream_ << "\t\t\t\tb " << mesh_iter->uvSets[i].binormals[x] << std::endl;
			}
		}
	}
}