#pragma once
#include "Tile.h"

using std::vector;

class Level 
{
private:
	std::vector<std::vector<Tile*>> tileGrid;
	std::vector<GameObject*> gameObjects;
	Coord startDoor;
	Coord endDoor;
	XMFLOAT3 currentPlayerPosition;
	int levelNr = -1;

public:
	Level (int levelNr );
	~Level ( );
	std::vector<GameObject*>* GetGameObjects(){ return &gameObjects; };
	void updateGameObjets();

	Tile* getTile (int x, int y);
	Tile* getTile (Coord coord);
	void setTile(Tile* tile, int x, int y);
	Coord getStartDoorCoord() const { return startDoor; }
	void setStartDoor(Coord val) { startDoor = val; }
	Coord getEndDoor() const { return endDoor; }
	void setEndDoor(Coord val) { endDoor = val; }
	XMFLOAT3 getPlayerPostion() const { return currentPlayerPosition; }
	void setPlayerPosition(XMFLOAT3 PlayerPosition) { currentPlayerPosition = PlayerPosition; }
	int GetLevelNr() const { return levelNr; }

	vector<vector<Tile*>>* getTileGrid() { return &tileGrid; }

	int sizeX();
	int sizeY(int x);

	bool withinBounds(int x, int y);
};

