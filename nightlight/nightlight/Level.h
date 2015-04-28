#pragma once
#include <vector>
#include "Tile.h"

class Level 
{
private:
	std::vector<std::vector<Tile>> tileGrid;
public:
	Level ( );
	~Level ( );
	void PushGameObjectToGrid ( int coordX, int coordY, GameObject &content );
};

