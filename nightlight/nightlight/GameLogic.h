#pragma once
#include <Windows.h>
#include <iostream>
#include <math.h>
#include "InputManager.h"
#include "cameraObject.h"
#include "lightObject.h"
#include "Character.h"
#include "Level.h"

class GameLogic
{
private:

	InputManager* Input;
	int screenWidth;
	int screenHeight;

	bool UpdatePlayer(Level* currentLevel, Character* player, CameraObject* camera, LightObject* spotLight);
	bool UpdateSpotLight (Character* player, CameraObject* camera, LightObject* spotlight);
	
	XMFLOAT3 ManageStaticPlayerCollisions(Level* currentLevel, Character* character, XMFLOAT3 pos);
	bool IsTileWalkable(Tile* tile);
	XMFLOAT3 NextPositionFromCollision(XMFLOAT3 nextPos, float radius, Coord tileCoord);
	float Clamp(float x, float a, float b) {return x < a ? a : (x > b ? b : x);}

public:

	GameLogic(HWND hwnd, int screenWidth, int screenHeight);
	~GameLogic();

	bool Update(Level* currentLevel, Character* gameObject, CameraObject* camera, LightObject* spotLight);
	
	//UpdateObjects(objects)
	//UpdateAI(aiObjects)
	//Etc
};

