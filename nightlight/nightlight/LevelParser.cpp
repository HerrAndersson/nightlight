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

		

	}
	return level;
}
