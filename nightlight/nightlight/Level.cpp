#include "Level.h"


Level::Level ( ) {
}


Level::~Level ( ) {
}

void Level::updateGameObjets()
{
	gameObjects.clear();
	for (int x = 0; x < (signed)tileGrid.size(); x++)
		for (int y = 0; y < (signed)tileGrid[x].size(); y++){
			try
			{
				if (tileGrid[x][y] != nullptr){
					std::vector<GameObject*>* tileGameObjects = tileGrid[x][y]->getAllGameObjects();
					for (int i = 0; i < tileGameObjects->size(); i++)
					{
						gameObjects.push_back(tileGameObjects->at(i));
					}
				}
			}
			catch (...) {}
		}
}

Tile* Level::getTile (int x, int y) {
	Tile* tile = nullptr;
	if (x >= 0 && y >= 0){
		try {
			tile = tileGrid.at(x).at(y);
		}
		catch (...) {}
	}
	else{
		cout << "Error in Level::getTile: x=" + std::to_string(x) + ", y=" + std::to_string(y) + " is not a valid coordinate.\n";
		return tile;
	}
	if (tile == nullptr){
		tile = new Tile();
		setTile(tile, x, y);
	}
	return tile;
}

//If there is already tile at x,y it will be deleted.
void Level::setTile(Tile* tile, int x, int y)
{
	try
	{
		delete tileGrid.at(x).at(y);
	}
	catch (...) {}

	if (x > (int)(tileGrid.size() - 1)) {
		tileGrid.resize(x + 1);
	}
	if (y > (int)(tileGrid.at(x).size() - 1)) {
		tileGrid.at(x).resize(y + 1);
	}
	tileGrid.at(x).at(y) = tile;
}

vector<vector<Tile*>> Level::getTileGrid()
{
	return tileGrid;
}
