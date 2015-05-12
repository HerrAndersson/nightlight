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

	InputManager*  Input;
	AiModule*      AI;
	bool end = true;

	int screenWidth;
	int screenHeight;

	bool leftMouseLastState = false;

	GameObject* selectedObject = nullptr;

	bool UpdatePlayer(Level* currentLevel, Character* player, CameraObject* camera, LightObject* spotLight);
	bool UpdateSpotLight(Character* player, CameraObject* camera, LightObject* spotlight);
	bool UpdateAI(vector<Enemy>* enemies);

	bool inLight(LightObject* spotlight, XMFLOAT3& enemy);
	
	XMFLOAT3 ManagePlayerCollisions(Level* currentLevel, Character* character, XMFLOAT3 pos);
	bool IsTileWalkable(Tile* tile);
	XMFLOAT3 NextPositionFromCollision(bool& result, XMFLOAT3 nextPos, float radius, Coord tileCoord);
	XMFLOAT3 NextPositionFromDoorCollision(bool& result, XMFLOAT3 nextPos, float radius, Coord iteratorTileCoord, Coord nextTileCoord, Door* door);

	float Clamp(float x, float a, float b) {return x < a ? a : (x > b ? b : x);}

public:

	GameLogic(HWND hwnd, int screenWidth, int screenHeight, AiModule* AI);
	~GameLogic();

	bool Update(Level* currentLevel, Character* gameObject, CameraObject* camera, LightObject* spotLight, vector<Enemy>* enemies);
	
	//UpdateObjects(objects)
	//UpdateAI(aiObjects)
	//Etc
};

