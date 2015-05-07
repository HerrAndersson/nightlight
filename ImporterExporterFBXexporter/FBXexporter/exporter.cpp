#include "DataHandler.cpp"

int main(int argc, char** argv) {

	DataHandler dataHandler;
	std::vector<std::string> binFileList;

	dataHandler.getBinFilenamesInDirectory("../", binFileList);

	dataHandler.importBinData(binFileList);

	dataHandler.FBXexport();

	return 0;
}

