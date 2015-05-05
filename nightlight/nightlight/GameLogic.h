#pragma once
#include "InputManager.h"
#include <Windows.h>
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

public:

	GameLogic(HWND hwnd, int screenWidth, int screenHeight);
	~GameLogic();

	bool Update(Level* currentLevel, Character* gameObject, CameraObject* camera, LightObject* spotLight);
	
	//UpdateObjects(objects)
	//UpdateAI(aiObjects)
	//Etc
};

