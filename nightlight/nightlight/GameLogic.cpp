#include "GameLogic.h"
#include <iostream>
#include <math.h>

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
	return UpdatePlayer(gameObject);
}

bool GameLogic::UpdatePlayer(GameObject* player)
{
	XMFLOAT2 oldP = Input->GetMousePos();
	Input->HandleMouse();
	XMFLOAT2 newP = Input->GetMousePos();

	XMFLOAT3 pos = player->GetPosition();
	XMFLOAT3 rot = player->GetRotation();

	if (Input->KeyDown('w'))
		pos = XMFLOAT3(pos.x, pos.y + 0.1f, pos.z);

	if (Input->KeyDown('s'))
		pos = XMFLOAT3(pos.x, pos.y - 0.1f, pos.z);

	if (Input->KeyDown('a'))
		pos = XMFLOAT3(pos.x - 0.1f, pos.y, pos.z);

	if (Input->KeyDown('d'))
		pos = XMFLOAT3(pos.x + 0.1f, pos.y, pos.z);


	int dx = (int)(newP.x - oldP.x);
	int dy = (int)(newP.y - oldP.y);

	rot = XMFLOAT3(0.0f, rot.y + dy, 0.0f);

	if (rot.x < 0)
		rot.x += 360;
	else if (rot.x > 360)
		rot.x = 0;

	player->SetPosition(pos);
	player->SetRotation(rot);

	//double angleRadians = atan2(dy, dx);

	//double angleDegrees = (angleRadians * 180) / XM_PI;

	return !Input->Esc();
}
