#include "Tile.h"


Tile::Tile ( ) 
{
}


Tile::~Tile ( ) 
{
}

void Tile::pushGameObjectToTile ( GameObject& content ) 
{
	gameObjects.push_back ( content );
	tileIsEmpty = false;
}
