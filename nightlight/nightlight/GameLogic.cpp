#include "GameLogic.h"
#include <iostream>

GameLogic::GameLogic(HWND hwnd, int screenWidth, int screenHeight)
{
	Input = new InputManager(hwnd, screenWidth, screenHeight);
}

GameLogic::~GameLogic()
{
	delete Input;
}

bool GameLogic::Update(GameObject* gameObject)
{
	Input->HandleMouse();

	if (Input->KeyDown('w'))
	{
		XMFLOAT3 pos = gameObject->GetPosition();
		gameObject->SetPosition(XMFLOAT3(pos.x, pos.y + 0.1f, pos.z));
	}

	if (Input->KeyDown('s'))
	{
		XMFLOAT3 pos = gameObject->GetPosition();
		gameObject->SetPosition(XMFLOAT3(pos.x, pos.y - 0.1f, pos.z));
	}

	if (Input->KeyDown('a'))
	{
		XMFLOAT3 pos = gameObject->GetPosition();
		gameObject->SetPosition(XMFLOAT3(pos.x - 0.1f, pos.y, pos.z));
	}

	if (Input->KeyDown('d'))
	{
		XMFLOAT3 pos = gameObject->GetPosition();
		gameObject->SetPosition(XMFLOAT3(pos.x + 0.1f, pos.y, pos.z));
	}

	return !Input->Esc();
}
