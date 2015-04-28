#pragma once
#include "Level.h"
#include "AssetManager.h"

class LevelParser 
{
private:
	AssetManager* assetManager = nullptr;
	std::vector<std::string> levelNames;

public:
	LevelParser ( AssetManager* assetManager );
	~LevelParser ( );

	Level LoadLevel (int levelID );
};