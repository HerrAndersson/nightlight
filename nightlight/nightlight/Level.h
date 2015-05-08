#pragma once
#include "Tile.h"

using std::vector;

struct Coord
{
	Coord(){
		x = -1;
		y = -1;
	}
	Coord(int x, int y){
		this->x = x;
		this->y = y;
	}
	int x;
	int y;
};

class Level 
{
private:
	std::vector<std::vector<Tile*>> tileGrid;
	std::vector<GameObject*> gameObjects;
	Coord startDoor;
	Coord endDoor;
	
public:
	Level ( );
	~Level ( );
	std::vector<GameObject*>* GetGameObjects(){ return &gameObjects; };
	void updateGameObjets();

	Tile* getTile (int x, int y);
	void setTile(Tile* tile, int x, int y);
	Coord getStartDoor() const { return startDoor; }
	void setStartDoor(Coord val) { startDoor = val; }
	Coord getEndDoor() const { return endDoor; }
	void setEndDoor(Coord val) { endDoor = val; }

	vector<vector<Tile*>>* getTileGrid() { return &tileGrid; }

	int sizeX();
	int sizeY(int x);

	bool withinBounds(int x, int y);
};

