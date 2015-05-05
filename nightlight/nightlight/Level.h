#pragma once
#include <vector>
#include "Tile.h"
#include "AiUtil.h"

class Level 
{
private:
	std::vector<std::vector<Tile>> tileGrid;
	std::vector<GameObject*> gameObjects;
public:
	Level ( );
	~Level ( );
	std::vector<GameObject*> GetGameObjects(){ return gameObjects; };
	void PushGameObjectToGrid ( int coordX, int coordY, GameObject& content );
	void SetGameObjects();
	Tile* getTile (int x, int y);
};

