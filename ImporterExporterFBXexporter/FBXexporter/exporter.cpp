#include "DataHandler.h"

int main(int argc, char** argv) {

	DataHandler dataHandler;
	std::vector<std::string> binFileList;
	

	//get all the bin files
	dataHandler.getBinFilenamesInDirectory("../Bin/", binFileList);

	//import all the data using our import functions from
	for (int i = 0; i < binFileList.size(); i++)
	{
		cout << "Loading " << binFileList.at(i).c_str() << endl;
		dataHandler.objectData->LoadModel("../Bin/"+binFileList[i]);
		dataHandler.FBXexport(binFileList,i);
	}

	return 0;
}

