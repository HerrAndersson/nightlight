#include "Tile.h"


Tile::Tile ( ) 
{
}


Tile::~Tile ( ) 
{
	//if (floorTile != nullptr)
	//	delete floorTile;
	//if (wall != nullptr)
	//	delete wall;
	//if (corner != nullptr)
	//	delete corner;
	//if (door != nullptr)
	//	delete door;
	//if (pressurePlate != nullptr)
	//	delete pressurePlate;
	//if (lever != nullptr)
	//	delete lever;
	//if (shadowContainer != nullptr)
	//	delete shadowContainer;
	//if (movableObject != nullptr)
	//	delete movableObject;
	//if (staticObject != nullptr)
	//	delete staticObject;
}

void Tile::createGameObjectFromUnparsedData(AssetManager* assetManager, std::vector<std::string>* gameObjectTypes, std::vector<std::string> unparsedData)
{
	int i = 0;
	int renderObjectRef = std::stoi(unparsedData.at(i++));

	int gameObjectTypeRef = std::stoi(unparsedData.at(i++));

	float rotation;
	XMFLOAT3 position;
	position.x = std::stof(unparsedData.at(i++)) - TILE_SIZE / 2;
	//position.x = std::stof(unparsedData.at(i++));
	position.y = std::stof(unparsedData.at(i++));
	position.z = std::stof(unparsedData.at(i++)) - TILE_SIZE / 2;
	//position.z = std::stof(unparsedData.at(i++));
	rotation = std::stof(unparsedData.at(i++));

	int tileCoordX = std::stoi(unparsedData.at(i++));
	int tileCoordY = std::stoi(unparsedData.at(i++));

	tileCoord = XMINT2(tileCoordX, tileCoordY);

	std::string gameObjectType = gameObjectTypes->at(gameObjectTypeRef);
	if (gameObjectType == "floor") 
	{
		floorTile = new Floor(position, rotation, assetManager->GetRenderObject(renderObjectRef), tileCoordX, tileCoordY);
		gameObjects.push_back(floorTile);
		tileIsWalkable = true;
	}
	else if (gameObjectType == "wall") 
	{
		wall = new Wall(position, rotation, assetManager->GetRenderObject(renderObjectRef), tileCoordX, tileCoordY);
		gameObjects.push_back(wall);
	}
	else if (gameObjectType == "corner") 
	{
		corner = new Corner(position, rotation, assetManager->GetRenderObject(renderObjectRef), tileCoordX, tileCoordY);
		gameObjects.push_back(corner);
	}
	else if (gameObjectType == "door") 
	{
		bool isOpen = std::stoi(unparsedData.at(i++)) == 1;
		int doorType = std::stoi(unparsedData.at(i++));
		std::string activationName = unparsedData.at(i++);
		door = new Door(position, rotation, assetManager->GetRenderObject(renderObjectRef), tileCoordX, tileCoordY, isOpen, doorType, activationName);
		gameObjects.push_back(door);
	}
	else if (gameObjectType == "static") 
	{
		staticObject = new StaticObject(position, rotation, assetManager->GetRenderObject(renderObjectRef), tileCoordX, tileCoordY);
		gameObjects.push_back(staticObject);
		tileIsWalkable = false;
	}
	else if (gameObjectType == "movable") 
	{
		movableObject = new MovableObject(position, rotation, assetManager->GetRenderObject(renderObjectRef), tileCoordX, tileCoordY);
		gameObjects.push_back(movableObject);
		tileIsWalkable = false;
	}
	else if (gameObjectType == "lever") 
	{
		bool isPowered = std::stoi(unparsedData.at(i++)) == 1;
		bool isActivated = std::stoi(unparsedData.at(i++)) == 1;
		std::string activationName = unparsedData.at(i++);
		std::string activates = unparsedData.at(i++);
		lever = new Lever(position, rotation, assetManager->GetRenderObject(renderObjectRef), tileCoordX, tileCoordY, isPowered, isActivated, activationName, activates);
		gameObjects.push_back(lever);
	}
	else if (gameObjectType == "pressure") 
	{
		std::string activates = unparsedData.at(i++);
		pressurePlate = new PressurePlate(position, rotation, assetManager->GetRenderObject(renderObjectRef), tileCoordX, tileCoordY, activates);
		gameObjects.push_back(pressurePlate);
	}
	else if (gameObjectType == "container") 
	{
		std::string activates = unparsedData.at(i++);
		shadowContainer = new Container(position, rotation, assetManager->GetRenderObject(renderObjectRef), tileCoordX, tileCoordY, activates);
		gameObjects.push_back(shadowContainer);
		tileIsWalkable = false;
	}
	else if (gameObjectType == "exitcube")
	{
		std::string activates = unparsedData.at(i++);
		button = new Button(position, rotation, assetManager->GetRenderObject(renderObjectRef), tileCoordX, tileCoordY, activates);
		gameObjects.push_back(button);
	}
	else if (gameObjectType == "continuecube")
	{
		std::string activates = unparsedData.at(i++);
		button = new Button(position, rotation, assetManager->GetRenderObject(renderObjectRef), tileCoordX, tileCoordY, activates);
		gameObjects.push_back(button);
	}
	else if (gameObjectType == "startcube")
	{
		std::string activates = unparsedData.at(i++);
		button = new Button(position, rotation, assetManager->GetRenderObject(renderObjectRef), tileCoordX, tileCoordY, activates);
		gameObjects.push_back(button);
	}
}
