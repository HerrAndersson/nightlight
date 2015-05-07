#pragma once

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
	
	int FBXexport();
	void importBinData();
	
private:

	

	
};

