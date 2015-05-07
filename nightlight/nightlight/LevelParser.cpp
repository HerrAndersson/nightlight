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

Level* LevelParser::LoadLevel(int levelID, std::vector<Enemy> &enemies, Character &character)
{
	Level* level = nullptr;

	if (levelID <= -1 || levelID > (signed)levelNames.size() - 1)
	{
		OutputDebugString("Error on LoadLevel: levelID out of bounds.");
		return level;
	}

	level = new Level();

	std::string pathToLevel = "Assets/Levels/" + levelNames.at(levelID);
	std::vector<std::string> unparsedLevel;
	assetUtility::fileToStrings(pathToLevel, unparsedLevel);

	std::vector<std::string> unparsedLine;
	for (int i = 0; i < (int)(unparsedLevel.size() - 1); i++)
	{
		try
		{
			unparsedLine.clear();
			assetUtility::splitStringToVector(unparsedLevel[i], unparsedLine, ",");

			int gameObjectTypeRef = std::stoi(unparsedLine.at(1));
			std::string gameObjectType = gameObjectTypes.at(gameObjectTypeRef);
			
			if (gameObjectType.find("enemy") != std::string::npos) 
			{
				int i = 0;
				int renderObjectRef = std::stoi(unparsedLine.at(i++));
				int gameObjectTypeRef = std::stoi(unparsedLine.at(i++));
				float rotation;
				XMFLOAT3 position;
				position.x = std::stof(unparsedLine.at(i++));
				position.y = std::stof(unparsedLine.at(i++));
				position.z = std::stof(unparsedLine.at(i++));
				rotation = std::stof(unparsedLine.at(i++));
				int tileCoordX = std::stoi(unparsedLine.at(i++));
				int tileCoordY = std::stoi(unparsedLine.at(i++));
				int enemyType = std::stoi(unparsedLine.at(i++));

				enemies.push_back(Enemy(position, rotation, assetManager->GetRenderObject(renderObjectRef), tileCoordX, tileCoordY, enemyType));
			}
			else
			{
				int tileCoordX = std::stoi(unparsedLine.at(6));
				int tileCoordY = std::stoi(unparsedLine.at(7));
				Tile* tile = level->getTile(tileCoordX, tileCoordY);
				if (tile == nullptr)
				{
					throw;
				}
				tile->createGameObjectFromUnparsedData(assetManager, &gameObjectTypes, unparsedLine);
			}
		}
		catch (...)
		{
			cout << "Error in LevelParser::LoadLevel: " + unparsedLevel[i] + " is not a valid gameObject.\n";
			delete level;
			return nullptr;
		}
	}

	//traverse levelGrid and find start/end pos, bind levers/pressurePlates/containers/doors.
	//randomize enemy starting rotations
	return level;
}