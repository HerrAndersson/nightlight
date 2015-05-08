#include "DataHandler.h"

int main(int argc, char** argv) {

	DataHandler dataHandler;
	std::vector<std::string> binFileList;

	dataHandler.getBinFilenamesInDirectory("../Bin/", binFileList);

	dataHandler.importBinData(binFileList);

	dataHandler.FBXexport();

	return 0;
}

