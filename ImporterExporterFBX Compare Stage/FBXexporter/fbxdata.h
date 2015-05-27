#include <fbxsdk.h>
#include <fbxsdk\fileio\fbxiosettings.h>
#include <vector>
#include <iostream>


class fbxData
{
public:
	std::vector<FbxVector4> vtx;
	std::vector<FbxVector4> norm;
	std::vector<FbxVector2> uv;
};
