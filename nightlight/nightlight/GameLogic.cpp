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

bool GameLogic::Update(Level* currentLevel, Character* character, CameraObject* camera, LightObject* spotLight)
{
	return UpdatePlayer(currentLevel, character, camera, spotLight);
}

bool GameLogic::UpdatePlayer(Level* currentLevel, Character* character, CameraObject* camera, LightObject* spotlight)
{
	XMFLOAT2 oldP = Input->GetMousePos();
	Input->HandleMouse();
	XMFLOAT2 newP = Input->GetMousePos();

	XMFLOAT3 pos = character->GetPosition();
	XMFLOAT3 rot = character->GetRotation();

	bool playerMoved = false;

	if ( Input->KeyDown ('w') ) {
		pos = XMFLOAT3 (pos.x, pos.y, pos.z + 0.1f);
		playerMoved = true;
	}

	if ( Input->KeyDown ('s') ) {
		pos = XMFLOAT3 (pos.x, pos.y, pos.z - 0.1f);
		playerMoved = true;
	}

	if ( Input->KeyDown ('a') ) {
		pos = XMFLOAT3 (pos.x + 0.1f, pos.y, pos.z);
		playerMoved = true;
	}

	if ( Input->KeyDown ('d') ) {
		pos = XMFLOAT3 (pos.x - 0.1f, pos.y, pos.z);
		playerMoved = true;
	}

	if (playerMoved) {
		pos = ManageStaticPlayerCollisions(currentLevel, character, pos);
	}

	//Only update if mouse or player moved
	if (oldP.x != newP.x || oldP.y != newP.y || playerMoved)
	{
		XMMATRIX v, p, vp;
		camera->GetViewMatrix(v);
		camera->GetProjectionMatrix(p);
		vp = v * p;

		XMVECTOR localSpace = XMVector3Transform ( XMLoadFloat3 ( &pos ), vp );
		float screenX = XMVectorGetX ( localSpace ) / XMVectorGetZ ( localSpace );
		float screenY = XMVectorGetY ( localSpace ) / XMVectorGetZ ( localSpace );

		XMFLOAT2 msp = Input->GetMousePosScreenSpace();
		double dx = ( msp.x - screenX );
		double dy = ( msp.y - screenY ) / camera->GetAspectRatio();
		double angle = atan2(dy, dx) * (180 / XM_PI);

		rot = XMFLOAT3(0.0f, (float)angle, 0.0f);
		character->SetRotation(rot);
		
		//std::cout << "Player pos:        " << "X: " << pos.x << " Y: " << pos.y << " Z: " << pos.z << std::endl;
		//std::cout << "Mouse pos:         " << "X: " << newP.x << " Y: " << newP.y << std::endl;
		//std::cout << "Mouse screenspace: " << msp.x << " " << msp.y << std::endl;
		//std::cout << "ScreenSpacePos:   " << "SX: " << screenSpacePos.x << " SY: " << screenSpacePos.y << " SZ: " << screenSpacePos.z << std::endl;
		//std::cout << "angle:            " << angle << std::endl;
		//std::cout << "roty:             " << rot.y << std::endl;
		//std::cout << std::endl;

	}

	camera->SetPosition(character->GetPosition().x, -15, character->GetPosition().z );
	camera->SetLookAt(character->GetPosition().x * 0.9, 0, character->GetPosition().z * 0.9);
	character->SetPosition(pos);

	UpdateSpotLight ( character, camera, spotlight );

	return !Input->Esc();
}

bool GameLogic::UpdateSpotLight (Character* player, CameraObject* camera, LightObject* spotlight)
{
	XMFLOAT3 pForward;
	XMStoreFloat3 ( &pForward, player->GetForwardVector ( ) );
	spotlight->setDirection ( pForward.x, pForward.y, pForward.z );
	
	/*XMFLOAT3 lightDirFinal;
	XMStoreFloat3(&lightDirFinal, player->GetForwardVector());*/
	
	XMFLOAT3 pPos = player->GetPosition ( );
	//offset light
	pPos.x += pForward.x/100;
	pPos.z += pForward.z/100;
	pPos.y -= 0.7f;
	spotlight->setPosition ( pPos.x, pPos.y, pPos.z );
	return true;

}

XMFLOAT3 GameLogic::ManageStaticPlayerCollisions(Level* currentLevel, Character* character, XMFLOAT3 nextPos) {
	float tileOffset = TILE_SIZE / 2;

	XMFLOAT3 currentPos = character->GetPosition();
	Tile* currentTile = currentLevel->getTile((int)(-currentPos.x + tileOffset), (int)(-currentPos.z + tileOffset));
	
	if (currentTile != nullptr) {
		
		int nextTileCoordX = (int)(-nextPos.x + tileOffset);
		int nextTileCoordY = (int)(-nextPos.z + tileOffset);
		Tile* nextTile = currentLevel->getTile(nextTileCoordX, nextTileCoordY);
		if (nextTile != nullptr)
		{
			for (int x = nextTileCoordX - 1; x < nextTileCoordX + 1; x++)
			{
				for (int y = nextTileCoordY - 1; y < nextTileCoordY + 1; y++) 
				{
					if (x != nextTileCoordX && y != nextTileCoordY) {


					}
				}
			}



			cout << to_string(nextPos.x + tileOffset) + " " + to_string(nextPos.z + tileOffset) + "\n";
		}
	}



	return nextPos;
}