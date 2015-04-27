#pragma once
#include "InputManager.h"
#include <Windows.h>
#include "GameObject.h"
#include "cameraObject.h"
class GameLogic
{
private:

	InputManager* Input;

public:

	GameLogic(HWND hwnd, int screenWidth, int screenHeight);
	~GameLogic();

	bool Update(GameObject* gameObject, CameraObject* camera);
	bool UpdatePlayer(GameObject* player, CameraObject* camera);

	//UpdateObjects(objects)
	//UpdateAI(aiObjects)
	//Etc
};

