#pragma once
#include "AssetManager.h"
#include <fbxsdk.h>
#include <fbxsdk\fileio\fbxiosettings.h>
#include <fstream>
#include <iostream>

#pragma comment(lib, "libfbxsdk.lib")

class DataHandler
{

public:

	DataHandler();
	~DataHandler();
	
	int FBXexport(std::vector<std::string>& binFileList, std::vector<Model>&modelList, std::vector<MaterialData>&MaterialList);
	void importBinData(std::vector<std::string>& binFileList, std::vector<Model>&modelList, std::vector<MaterialData>&MaterialList);
	bool getBinFilenamesInDirectory(char *folder_path, std::vector<std::string> &list_to_fill);

private:

	AssetManager*	objectData;

	
};
