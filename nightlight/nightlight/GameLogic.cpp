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

bool GameLogic::Update(GameObject* gameObject, CameraObject* camera, LightObject* spotLight)
{
	return UpdatePlayer(gameObject, camera, spotLight);

}

bool GameLogic::UpdatePlayer(GameObject* player, CameraObject* camera, LightObject* spotlight)
{
	XMFLOAT2 oldP = Input->GetMousePos();
	Input->HandleMouse();
	XMFLOAT2 newP = Input->GetMousePos();

	XMFLOAT3 pos = player->GetPosition();
	XMFLOAT3 rot = player->GetRotation();

	if (Input->KeyDown('w'))
		pos = XMFLOAT3(pos.x, pos.y, pos.z + 0.1f);

	if (Input->KeyDown('s'))
		pos = XMFLOAT3(pos.x, pos.y, pos.z - 0.1f);

	if (Input->KeyDown('a'))
		pos = XMFLOAT3(pos.x - 0.1f, pos.y, pos.z);

	if (Input->KeyDown('d'))
		pos = XMFLOAT3(pos.x + 0.1f, pos.y, pos.z);

	if (oldP.x != newP.x && oldP.y != newP.y)
	{
		XMMATRIX v, p, vp;
		camera->getViewMatrix(v);
		camera->getProjectionMatrix(p);
		vp = v * p;

		XMVECTOR worldPosVector;
		worldPosVector = XMLoadFloat3(&pos);
		worldPosVector = XMVector3Transform(worldPosVector, vp);
		XMFLOAT3 screenSpacePos;
		XMStoreFloat3(&screenSpacePos, worldPosVector);


		float dx = (newP.x - screenSpacePos.x);
		float dy = (newP.y - screenSpacePos.y);

		double angle = atan2(dx, dy) * (180 / XM_PI);

		rot = XMFLOAT3(0.0f, (float)angle, 0.0f);

		if (rot.y < 0)
			rot.y += 360;
		else if (rot.y > 360)
			rot.y = 0;

		std::cout << "Pos:              " << "X: " << pos.x << " Z: " << pos.z << std::endl;
		std::cout << "ScreenSpacePos:   " << "SX: " << screenSpacePos.x << " SY: " << screenSpacePos.y << " SZ: " << screenSpacePos.z << std::endl;
		std::cout << "dxdy:             " << dx << " " << dy << std::endl;
		std::cout << "angle:            " << angle << std::endl;
		std::cout << "roty:             " << rot.y << std::endl;
		std::cout << std::endl;

		player->SetRotation(rot);
		

	}

	player->SetPosition(pos);
	updateSpotLight(player, camera, spotlight);
	

	return !Input->Esc();
}

bool GameLogic::updateSpotLight(GameObject* player, CameraObject* camera, LightObject* spotlight)
{
	Input->HandleMouse();
	spotlight->SetPosition(player->GetPosition().x, player->GetPosition().y, player->GetPosition().z);
	//spotlight->SetDirection(player->GetPosition().x, player->GetPosition().y, player->GetPosition().z);
	spotlight->SetDirection(Input->GetMousePos().x, 1, Input->GetMousePos().y);
	return true;

}


