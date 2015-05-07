#pragma once
#include <vector>
#include "Container.h"
#include "Corner.h"
#include "Door.h"
#include "Floor.h"
#include "Lever.h"
#include "MovableObject.h"
#include "PressurePlate.h"
#include "StaticObject.h"
#include "Wall.h"
#include "AssetManager.h"

#define TILE_SIZE 1.0f

class Tile 
{
private:
	Floor* floorTile = nullptr;
	Wall* wall = nullptr;
	Corner* corner = nullptr;
	Door* door = nullptr;
	PressurePlate* pressurePlate = nullptr;
	Lever* lever = nullptr;
	Container* shadowContainer = nullptr;
	MovableObject* movableObject = nullptr;
	StaticObject* staticObject = nullptr;

	std::vector<GameObject*> gameObjects;

	bool tileIsWalkable = false;
	
	
public:

	Tile ( );
	~Tile ( );

	Floor* getFloorTile() const { return floorTile; }
	Wall* getWall() const { return wall; }
	Corner* getCorner() const { return corner; }
	Door* getDoor() const { return door; }
	PressurePlate* getPressurePlate() const { return pressurePlate; }
	Lever* getLever() const { return lever; }
	Container* getShadowContainer() const { return shadowContainer; }
	MovableObject* getMovableObject() const { return movableObject; }
	StaticObject* getStaticObject() const { return staticObject; }

	std::vector<GameObject*>* getAllGameObjects(){ return &gameObjects; }

	void createGameObjectFromUnparsedData(AssetManager* assetManager, std::vector<std::string>* gameObjectTypes, std::vector<std::string> unparsedData);

	bool getTileIsWalkable() { return tileIsWalkable; }
};

