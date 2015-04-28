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

	bool playerMoved = false;

	if ( Input->KeyDown ( 'w' ) ) {
		pos = XMFLOAT3 ( pos.x, pos.y, pos.z + 0.1f );
		playerMoved = true;
	}
	
	if ( Input->KeyDown ( 's' ) ) {
		pos = XMFLOAT3 ( pos.x, pos.y, pos.z - 0.1f );
		playerMoved = true;
	}

	if ( Input->KeyDown ( 'a' ) ) {
		pos = XMFLOAT3 ( pos.x - 0.1f, pos.y, pos.z );
		playerMoved = true;
	}

	if ( Input->KeyDown ( 'd' ) ) {
		pos = XMFLOAT3 ( pos.x + 0.1f, pos.y, pos.z );
		playerMoved = true;
	}
	
	//Only update if mouse or player moved
	if (oldP.x != newP.x || oldP.y != newP.y || playerMoved)
	{
		XMMATRIX v, p, vp;
		camera->getViewMatrix(v);
		camera->getProjectionMatrix(p);
		vp = v * p;

		//Converting the players position from world to screen space
		//XMVECTOR worldPosVector;
		//worldPosVector = XMLoadFloat3(&pos);
		//worldPosVector = XMVector3Transform(worldPosVector, vp);
		//XMFLOAT3 screenSpacePos;
		//XMStoreFloat3(&screenSpacePos, worldPosVector);

		XMVECTOR localSpace = XMVector3Transform ( XMLoadFloat3 ( &pos ), vp );
		float screenX = XMVectorGetX ( localSpace ) / XMVectorGetZ ( localSpace );
		float screenY = XMVectorGetY ( localSpace ) / XMVectorGetZ ( localSpace );

		XMFLOAT2 msp = Input->GetMousePosScreenSpace();

		/*double dx = (msp.x - pos.x);
		double dy = (msp.y - pos.y);*/

		double dx = ( msp.x - screenX );
		double dy = ( msp.y - screenY );

		double angle = atan2(dy, dx) * (180 / XM_PI);

		printf ( ( std::to_string ( msp.x ) + " " + std::to_string ( msp.y ) + "\n" + std::to_string ( screenX) + " " + std::to_string ( screenY) + "\n\n" ).c_str ( ) );

		rot = XMFLOAT3(0.0f, (float)angle, 0.0f);

		//if (rot.y < 0)
		//	rot.y += 360;
		//else if (rot.y > 360)
		//	rot.y = 0;

		player->SetRotation(rot);
		


		//std::cout << "Player pos:        " << "X: " << pos.x << " Y: " << pos.y << " Z: " << pos.z << std::endl;
		//std::cout << "Mouse pos:         " << "X: " << newP.x << " Y: " << newP.y << std::endl;
		//std::cout << "Mouse screenspace: " << msp.x << " " << msp.y << std::endl;
		//std::cout << "ScreenSpacePos:   " << "SX: " << screenSpacePos.x << " SY: " << screenSpacePos.y << " SZ: " << screenSpacePos.z << std::endl;
		//std::cout << "angle:            " << angle << std::endl;
		//std::cout << "roty:             " << rot.y << std::endl;
		//std::cout << std::endl;
	}

	player->SetPosition(pos);
	UpdateSpotLight ( player, camera, spotlight );

	return !Input->Esc();
}

bool GameLogic::UpdateSpotLight(GameObject* player, CameraObject* camera, LightObject* spotlight)
{
	XMFLOAT3 pForward;
	XMStoreFloat3 ( &pForward, player->GetForwardVector ( ) );
	spotlight->setDirection ( pForward.x, pForward.y, pForward.z );
	
	/*XMFLOAT3 lightDirFinal;
	XMStoreFloat3(&lightDirFinal, player->GetForwardVector());*/
	
	XMFLOAT3 pPos = player->GetPosition ( );
	spotlight->setPosition ( pPos.x, pPos.y, pPos.z );
	return true;

}


