#include "LevelParser.h"

LevelParser::LevelParser ( AssetManager* assetManager ) 
{
	if (assetManager == nullptr)
	{
		OutputDebugString ( "Error in LevelParser constructor: assetManager == nullptr" );
	}
	else
	{
		this->assetManager = assetManager;
	}
	assetUtility::fileToStrings ( "Levels.txt", levelNames );
	assetUtility::fileToStrings ( "gameObjectTypes.txt", gameObjectTypes );
}


LevelParser::~LevelParser ( ) 
{

}

Level LevelParser::LoadLevel ( int levelID, std::vector<Enemy> &enemies, Character &character )
{
	Level level = Level();

	if ( levelID <= 0 || levelID > levelNames.size ( ) - 1 ) {
		OutputDebugString ( "Error on LoadLevel: levelID out of bounds." );
		return level;
	}

	std::string pathToLevel = "Levels/" + levelNames.at ( levelID );
	std::vector<std::string> unparsedLevel;
	assetUtility::fileToStrings ( pathToLevel, unparsedLevel );

	std::vector<std::string> unparsedLine;
	for each ( std::string line in unparsedLevel)
	{
		unparsedLine.clear ( );
		assetUtility::splitStringToVector ( line, unparsedLine, "," );

		GameObject go = CreateGameObjectFromLevelData ( unparsedLine );


	}
	return level;
}

GameObject LevelParser::CreateGameObjectFromLevelData ( std::vector<std::string> unparsedData )
{
	int i = 0;
	int renderObjectRef = std::stoi ( unparsedData.at ( i++ ) );

	int gameObjectTypeRef = std::stoi ( unparsedData.at ( i++ ) );

	XMMATRIX worldMatrix = XMMATRIX ( 
		std::stof ( unparsedData.at ( i++ ) ), std::stof ( unparsedData.at ( i++ ) ), std::stof ( unparsedData.at ( i++ ) ), std::stof ( unparsedData.at ( i++ ) ),
		std::stof ( unparsedData.at ( i++ ) ), std::stof ( unparsedData.at ( i++ ) ), std::stof ( unparsedData.at ( i++ ) ), std::stof ( unparsedData.at ( i++ ) ), 
		std::stof ( unparsedData.at ( i++ ) ), std::stof ( unparsedData.at ( i++ ) ), std::stof ( unparsedData.at ( i++ ) ), std::stof ( unparsedData.at ( i++ ) ), 
		std::stof ( unparsedData.at ( i++ ) ), std::stof ( unparsedData.at ( i++ ) ), std::stof ( unparsedData.at ( i++ ) ), std::stof ( unparsedData.at ( i++ ) ));
	
	int tileCoordX = std::stoi ( unparsedData.at ( i++ ) );
	int tileCoordY = std::stoi ( unparsedData.at ( i++ ) );

	std::string gameObjectType = gameObjectTypes.at ( gameObjectTypeRef );
	if ( gameObjectType == "floor" ) {

	} else if ( gameObjectType == "wall" ) {

	} else if ( gameObjectType == "corner"  ) {

	} else if ( gameObjectType == "door" ) {

	} else if ( gameObjectType == "static" ) {

	} else if ( gameObjectType == "movable" ) {

	} else if ( gameObjectType == "lever" ) {

	} else if ( gameObjectType == "pressure" ) {

	} else if ( gameObjectType == "container" ) {

	} else if ( gameObjectType.find ( "enemy" ) != std::string::npos ) {

	} else if ( gameObjectType == "sidec" ) {

	} else if ( gameObjectType == "mainc" ) {

	}

	}
	
}
