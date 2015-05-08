#include "DataHandler.h"

int main(int argc, char** argv) {

	DataHandler dataHandler;
	std::vector<std::string> binFileList;
	std::vector<Model>modelList;

	dataHandler.getBinFilenamesInDirectory("../Bin/", binFileList);

	dataHandler.importBinData(binFileList, modelList);

	dataHandler.FBXexport(binFileList, modelList);

	return 0;
}

