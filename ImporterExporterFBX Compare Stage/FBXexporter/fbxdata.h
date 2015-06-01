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

	std::vector<FbxVector4> LightPos;
	std::vector<const char*> LightType;
	std::vector<FbxDouble3> LightColor;

	std::vector<std::string> materialtype;
	std::vector<FbxColor> ambient;
	std::vector<FbxColor> diffuse;
	std::vector<FbxColor> specular;
	std::vector<FbxColor> emissive;
	std::vector<FbxDouble> shininess;
	std::vector<FbxDouble> transparency;
	std::vector<FbxDouble> reflectionfactor;

	std::vector<FbxDouble3> cameraUpVector;
	std::vector<FbxDouble3> cameraPosition;
	std::vector<FbxDouble> fieldofviewX;
	std::vector<FbxDouble> fieldofviewY;
};
