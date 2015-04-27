#pragma once
#include "InputManager.h"
#include <Windows.h>
#include "GameObject.h"
#include "cameraObject.h"
#include "lightObject.h"
class GameLogic
{
private:

	InputManager* Input;

public:

	GameLogic(HWND hwnd, int screenWidth, int screenHeight);
	~GameLogic();

	bool Update(GameObject* gameObject, CameraObject* camera, LightObject* spotLight);
	bool UpdatePlayer(GameObject* player, CameraObject* camera, LightObject* spotLight);
	bool updateSpotLight(GameObject* player, CameraObject* camera, LightObject* spotlight);
	//UpdateObjects(objects)
	//UpdateAI(aiObjects)
	//Etc
};

