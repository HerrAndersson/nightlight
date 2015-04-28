#pragma once
#include "Level.h"
#include "AssetManager.h"

class LevelParser {
public:
	LevelParser ( AssetManager* assetManager );
	~LevelParser ( );

	Level LoadLevel (int levelID );
private:
	AssetManager* assetManager = nullptr;
	std::vector<std::string> levelNames;
};