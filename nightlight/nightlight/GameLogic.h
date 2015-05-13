#pragma once
#include <Windows.h>
#include <iostream>
#include <math.h>
#include "InputManager.h"
#include "cameraObject.h"
#include "lightObject.h"
#include "Character.h"
#include "Level.h"
#include "AiModule.h"

class GameLogic
{
private:

	enum Axis { X, Y };
	//enum SelectionTypes {NONE, BUTTON, LEVER, MOVABLEOBJECT};
	enum CollisionTypes { CHARACTER, ENEMY, MOVABLEOBJECT };

	InputManager*  Input;
	AiModule*      AI;
	Level* menuLevel;
	bool end = true;

	int screenWidth;
	int screenHeight;

	bool leftMouseLastState = false;
	bool moveObjectMode = false;
	int moveObjectModeAxis = -1;

	GameObject* selectedObject = nullptr;

	bool UpdatePlayer(Level* currentLevel, Character* player, CameraObject* camera, LightObject* spotLight, int& currentLevelNr);
	bool UpdateSpotLight(Character* player, CameraObject* camera, LightObject* spotlight);
	bool UpdateAI(vector<Enemy>* enemies);

	bool inLight(LightObject* spotlight, XMFLOAT3& enemy);
	
	XMFLOAT3 ManageCollisions(Level* currentLevel, GameObject* gameObject, XMFLOAT3 pos, CollisionTypes type);
	XMFLOAT3 ManagePlayerCollision(Tile* iteratorTile, Coord iteratorTileCoord, Coord nextTileCoord, Coord currentTileCoord, float characterRadius, XMFLOAT3 nextPos);

	void SelectObject(GameObject* selectedObject);

	XMFLOAT3 NextPositionFromCollision(bool& result, XMFLOAT3 nextPos, float radius, Coord tileCoord);
	XMFLOAT3 NextPositionFromDoorCollision(bool& result, XMFLOAT3 nextPos, float radius, Coord iteratorTileCoord, Coord nextTileCoord, Door* door);

	float Clamp(float x, float a, float b) {return x < a ? a : (x > b ? b : x);}

public:

	GameLogic(HWND hwnd, int screenWidth, int screenHeight, AiModule* AI, Level* menuLevel);
	~GameLogic();

	bool Update(Level* currentLevel, Character* gameObject, CameraObject* camera, LightObject* spotLight, vector<Enemy>* enemies, int& levelNumberToLoad);
	
	//UpdateObjects(objects)
	//UpdateAI(aiObjects)
	//Etc
};

