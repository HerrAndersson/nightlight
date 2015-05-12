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
#include "Exit.h"

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
	Exit* exit = nullptr;

	std::vector<GameObject*> gameObjects;

	//For pathfinding
	XMINT2 tileCoord;
	int fScore, gScore;
	Tile* parent = nullptr;
	bool tileIsWalkable = false;
	bool inClosed = false;
	bool inOpen = false;
		
public:

	Tile ( );
	~Tile ( );

	Floor* getFloorTile()				const { return floorTile; }
	Wall* getWall()						const { return wall; }
	Corner* getCorner()					const { return corner; }
	Door* getDoor()						const { return door; }
	PressurePlate* getPressurePlate()	const { return pressurePlate; }
	Lever* getLever()					const { return lever; }
	Container* getShadowContainer()		const { return shadowContainer; }
	MovableObject* getMovableObject()	const { return movableObject; }
	StaticObject* getStaticObject()		const { return staticObject; }
	Exit* getExit()						const { return exit; }

	std::vector<GameObject*>* getAllGameObjects(){ return &gameObjects; }

	void createGameObjectFromUnparsedData(AssetManager* assetManager, std::vector<std::string>* gameObjectTypes, std::vector<std::string> unparsedData);

	//For pathfinding
	bool IsWalkable()					{ return tileIsWalkable; }
	XMINT2 GetTileCoord()				{ return tileCoord; };
	int GetF()							{ return fScore; };
	void SetF(int f)					{ fScore = f; };
	int GetG()							{ return gScore; };
	void SetG(int g)					{ gScore = g; };
	bool InOpen()						{ return inOpen; };
	bool InClosed()						{ return inOpen; };
	void SetInOpen(bool inOpen)			{ this->inOpen = inOpen; };
	void SetInClosed(bool inClosed)		{ this->inClosed = inClosed; };
	Tile* GetParent()					{ return parent; };
	void SetParent(Tile* parent)		{ this->parent = parent; };
};

