#pragma once
#include "InputManager.h"
#include <Windows.h>
#include "GameObject.h"
class GameLogic
{
private:

	InputManager* Input;

public:

	GameLogic(HWND hwnd, int screenWidth, int screenHeight);
	~GameLogic();

	bool Update(GameObject* gameObject);
	bool UpdatePlayer(GameObject* player);

	//UpdateObjects(objects)
	//UpdateAI(aiObjects)
	//UpdatePlayer(playerobject)
	//Etc
};

