#include "LevelParser.h"

LevelParser::LevelParser(AssetManager* assetManager)
{
	if (assetManager == nullptr)
	{
		OutputDebugString("Error in LevelParser constructor: assetManager == nullptr");
	}
	else
	{
		this->assetManager = assetManager;
	}
	assetUtility::fileToStrings("Assets/levels.txt", levelNames);
	assetUtility::fileToStrings("Assets/gameObjectTypes.txt", gameObjectTypes);
}


LevelParser::~LevelParser()
{

}

Level LevelParser::LoadLevel(int levelID, std::vector<Enemy> &enemies, Character &character)
{
	Level level = Level();

	if (levelID <= -1 || levelID > (signed)levelNames.size() - 1) {
		OutputDebugString("Error on LoadLevel: levelID out of bounds.");
		return level;
	}

	std::string pathToLevel = "Assets/Levels/" + levelNames.at(levelID);
	std::vector<std::string> unparsedLevel;
	assetUtility::fileToStrings(pathToLevel, unparsedLevel);

	std::vector<std::string> unparsedLine;
	for (int i = 0; i < (int)(unparsedLevel.size() - 1); i++)
	{
		unparsedLine.clear();
		assetUtility::splitStringToVector(unparsedLevel[i], unparsedLine, ",");

		GameObject go = CreateGameObjectFromLevelData(unparsedLine);
		level.PushGameObjectToGrid(go.GetTileXCoord(), go.GetTileYCoord(), go);
	}
	return level;
}

GameObject LevelParser::CreateGameObjectFromLevelData(std::vector<std::string> unparsedData)
{

	int i = 0;
	int renderObjectRef = std::stoi(unparsedData.at(i++));

	int gameObjectTypeRef = std::stoi(unparsedData.at(i++));

	float rotation;
	XMFLOAT3 position;
	position.x = std::stof(unparsedData.at(i++));
	position.y = std::stof(unparsedData.at(i++));
	position.z = std::stof(unparsedData.at(i++));
	rotation = std::stof(unparsedData.at(i++));

	int tileCoordX = std::stoi(unparsedData.at(i++));
	int tileCoordY = std::stoi(unparsedData.at(i++));

	GameObject gameObject(position, rotation, assetManager->GetRenderObject(renderObjectRef), tileCoordX, tileCoordY);
	
	std::string gameObjectType = gameObjectTypes.at(gameObjectTypeRef);
	if (gameObjectType == "floor") {

	}
	else if (gameObjectType == "wall") {

	}
	else if (gameObjectType == "corner") {

	}
	else if (gameObjectType == "door") {

	}
	else if (gameObjectType == "static") {

	}
	else if (gameObjectType == "movable") {

	}
	else if (gameObjectType == "lever") {

	}
	else if (gameObjectType == "pressure") {

	}
	else if (gameObjectType == "container") {

	}
	else if (gameObjectType.find("enemy") != std::string::npos) {

	}
	else if (gameObjectType == "sidec") {

	}
	else if (gameObjectType == "mainc") {

	}

	return gameObject;
}