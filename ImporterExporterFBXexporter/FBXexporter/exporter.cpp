#include "DataHandler.h"

int main(int argc, char** argv) {

	DataHandler dataHandler;
	std::vector<std::string> binFileList;
	std::vector<Model>modelList;

	//get all the bin files
	dataHandler.getBinFilenamesInDirectory("../Bin/", binFileList);

	//import all the data using our import functions from nightlight
	dataHandler.importBinData(binFileList, modelList);

	//export using FBX sdk
	dataHandler.FBXexport(binFileList, modelList);

	return 0;
}

