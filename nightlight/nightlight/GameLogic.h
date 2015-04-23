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

	//UpdateObjects(objects)
	//UpdateAI(aiObjects)
	//Etc
};

