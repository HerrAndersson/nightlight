#pragma once
#include <vector>
#include "GameObject.h"
#include "Container.h"
#include "Corner.h"
#include "Door.h"
#include "Floor.h"
#include "Lever.h"
#include "MovableObject.h"
#include "PressurePlate.h"
#include "StaticObject.h"
#include "Wall.h"

class Tile 
{
private:

	bool tileIsEmpty = true;
	bool tileIsWalkable = false;
	std::vector<GameObject> gameObjects;
	
public:

	Tile ( );
	~Tile ( );

	void pushGameObjectToTile ( GameObject& content );

	GameObject* GetGameObject(int i){ return& gameObjects[i]; };
	int GetNumberOfGameObjects(){ return gameObjects.size(); };
	bool getTileIsEmpty ( ) const { return tileIsEmpty; }
	bool getTileIsWalkable() const { return tileIsWalkable; }
};

