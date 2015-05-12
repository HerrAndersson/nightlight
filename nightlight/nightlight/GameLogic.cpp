#include "GameLogic.h"

GameLogic::GameLogic(HWND hwnd, int screenWidth, int screenHeight, AiModule* AI)
{
	Input = new InputManager(hwnd, screenWidth, screenHeight);
	this->AI = AI;
}

GameLogic::~GameLogic()
{
	delete Input;
}

bool GameLogic::Update(Level* currentLevel, Character* character, CameraObject* camera, LightObject* spotLight, vector<Enemy>* enemies)
{
	bool result = false;

	result = UpdatePlayer(currentLevel, character, camera, spotLight);
	if (!result) return result;

	result = UpdateAI(enemies);
	if (!result) return result;

	if (Input->Esc()) return false;

	return result;
}

bool GameLogic::UpdateAI(vector<Enemy>* enemies)
{
	//TOFO: Update enemies here
	return true;
}

bool GameLogic::UpdatePlayer(Level* currentLevel, Character* character, CameraObject* camera, LightObject* spotlight)
{
	XMFLOAT2 oldP = Input->GetMousePos();
	Input->HandleMouse();
	XMFLOAT2 newP = Input->GetMousePos();

	XMFLOAT3 pos = character->GetPosition();
	XMFLOAT3 rot = character->GetRotationDeg();

	bool playerMoved = false;

	if (Input->KeyDown('w'))
	{
		pos = XMFLOAT3(pos.x, pos.y, pos.z + 0.1f);
		playerMoved = true;
	}
	if (Input->KeyDown('s'))
	{
		pos = XMFLOAT3(pos.x, pos.y, pos.z - 0.1f);
		playerMoved = true;
	}
	if (Input->KeyDown('a'))
	{
		pos = XMFLOAT3(pos.x + 0.1f, pos.y, pos.z);
		playerMoved = true;
	}
	if (Input->KeyDown('d'))
	{
		pos = XMFLOAT3(pos.x - 0.1f, pos.y, pos.z);
		playerMoved = true;
	}

	if (playerMoved)
	{
		pos = ManagePlayerCollisions(currentLevel, character, pos);
	}

	if (Input->LeftMouse() && leftMouseLastState == false)
	{
		leftMouseLastState = true;

		if (selectedObject != nullptr) {
			Lever* lever = dynamic_cast<Lever*>(selectedObject);
			MovableObject* movable = dynamic_cast<MovableObject*>(selectedObject);

			if (movable != nullptr) {
				//TODO: Enable move mode.
			}
			if (lever != nullptr) {
				if (lever->getIsActivated()) {
					lever->DeactivateLever();
				}
				else {
					lever->ActivateLever();
				}
			}
		}
		/*if (selectedObject != nullptr)
		{
			Button* button = dynamic_cast<Button*>(selectedObject);
			
			if (button != nullptr)
			{
				if (button->getIsActivated()) { button->DeactivateButton(); }

				else{ button->ActivateButton(); }
			}
		}*/
	}
	else if (!Input->LeftMouse() && leftMouseLastState == true)
	{
		leftMouseLastState = false;
	}

	//Only update if mouse or player moved
	if (oldP.x != newP.x || oldP.y != newP.y || playerMoved)
	{
		XMMATRIX v, p, vp;
		camera->GetViewMatrix(v);
		camera->GetProjectionMatrix(p);
		vp = v * p;

		XMVECTOR localSpace = XMVector3Transform(XMLoadFloat3(&pos), vp);
		float screenX = XMVectorGetX(localSpace) / XMVectorGetZ(localSpace);
		float screenY = XMVectorGetY(localSpace) / XMVectorGetZ(localSpace);

		XMFLOAT2 msp = Input->GetMousePosScreenSpace();
		double dx = (msp.x - screenX);
		double dy = (msp.y - screenY) / camera->GetAspectRatio();
		double angle = atan2(dy, dx) * (180 / XM_PI);

		rot = XMFLOAT3(0.0f, (float)angle, 0.0f);
		character->SetRotationDeg(rot);
	}

	camera->SetPosition(character->GetPosition().x, -15, character->GetPosition().z);
	camera->SetLookAt(character->GetPosition().x * 0.9f, 0.0f, character->GetPosition().z * 0.9f);
	character->SetPosition(pos);

	UpdateSpotLight(character, camera, spotlight);

	return true;
}

bool GameLogic::UpdateSpotLight(Character* player, CameraObject* camera, LightObject* spotlight)
{
	XMFLOAT3 pForward;
	XMStoreFloat3(&pForward, player->GetForwardVector());
	spotlight->setDirection(pForward.x, pForward.y, pForward.z);

	XMFLOAT3 pPos = player->GetPosition();
	//offset light
	pPos.x += pForward.x / 100;
	pPos.z += pForward.z / 100;
	pPos.y -= 0.7f;

	spotlight->setPosition(pPos.x, pPos.y, pPos.z);
	spotlight->generateViewMatrix();
	return true;

}

XMFLOAT3 GameLogic::ManagePlayerCollisions(Level* currentLevel, Character* character, XMFLOAT3 nextPos)
{
	float tileOffset = TILE_SIZE / 2;
	bool result = false;

	XMFLOAT3 currentPos = character->GetPosition();

	Tile* currentTile = currentLevel->getTile((int)(abs(currentPos.x)), (int)(abs(currentPos.z)));
	if (currentTile != nullptr)
	{

		Coord nextTileCoord = Coord((int)(abs(nextPos.x)), (int)(abs(nextPos.z)));
		Tile* nextTile = currentLevel->getTile(nextTileCoord.x, nextTileCoord.y);
		if (nextTile != nullptr)
		{

			for (int x = nextTileCoord.x - 1; x <= nextTileCoord.x + 1; x++)
			{
				for (int y = nextTileCoord.y - 1; y <= nextTileCoord.y + 1; y++)
				{
					Tile* tile = currentLevel->getTile(x, y);
					Coord iteratorTileCoord = Coord(x, y);

					if (tile != nullptr && tile->getPressurePlate() != nullptr)
					{
						if (tile->getMovableObject() != nullptr || nextTileCoord == iteratorTileCoord)
						{
							if (!tile->getPressurePlate()->getIsActivated())
							{
								tile->getPressurePlate()->ActivatePressurePlate();
							}
						}
						else
						{
							if (tile->getPressurePlate()->getIsActivated())
							{
								tile->getPressurePlate()->DeactivatePressurePlate();
							}
						}
					}

					//for when walking on the "buttons" in the menu
					if (tile != nullptr && tile->getButton() != nullptr)
					{
						if ((tile->getButton()->getCoordX() == 6) && (tile->getButton()->getCoordY() <= 4) && (tile->getButton()->getCoordY() >= 3))
						{
							//start button

							if (tile->getMovableObject() != nullptr || nextTileCoord == iteratorTileCoord)
							{
								if (!tile->getButton()->getIsStartActivated())
								{
									tile->getButton()->ActivateStartButton();
								}
							}
							else
							{
								if (tile->getButton()->getIsStartActivated())
								{
									tile->getButton()->DeactivateStartButton();
								}
							}
						}
					
					}

					if (tile != nullptr && tile->getButton() != nullptr)
					{
						if ((tile->getButton()->getCoordX() == 6) && (tile->getButton()->getCoordY() > 4) && (tile->getButton()->getCoordY() <= 6))
						{
							//continue button

							if (tile->getMovableObject() != nullptr || nextTileCoord == iteratorTileCoord)
							{
								if (!tile->getButton()->getIsContinueActivated())
								{
									tile->getButton()->ActivateContinueButton();
								}
							}
							else
							{
								if (tile->getButton()->getIsContinueActivated())
								{
									tile->getButton()->DeactivateContinueButton();
								}
							}
						}

					}

					if (tile != nullptr && tile->getButton() != nullptr)
					{
						if ((tile->getButton()->getCoordX() == 6) && (tile->getButton()->getCoordY() > 6) && (tile->getButton()->getCoordY() <= 7))
						{
							//exit button

							if (tile->getMovableObject() != nullptr || nextTileCoord == iteratorTileCoord)
							{
								if (!tile->getButton()->getIsExitActivated())
								{
									tile->getButton()->ActivateExitButton();
								}
							}
							else
							{
								if (tile->getButton()->getIsExitActivated())
								{
									tile->getButton()->DeactivateExitButton();
								}
							}
						}

					}


					if (!IsTileWalkable(tile))
					{
						nextPos = NextPositionFromCollision(result, nextPos, character->getRadius(), iteratorTileCoord);

						if (result && tile != nullptr && tile->getMovableObject() != nullptr)
						{
							if (selectedObject != nullptr)
							{
								selectedObject->SetSelected(false);
							}
							selectedObject = tile->getMovableObject();
							if (!selectedObject->IsSelected())
								selectedObject->SetSelected((true));

						}
						else if (!result && tile != nullptr && tile->getMovableObject() != nullptr)
						{
							if (selectedObject != nullptr)
							{
								selectedObject->SetSelected(false);
								selectedObject = nullptr;
							}
						}
					}
					else
					{
						Door* door = tile->getDoor();
						nextPos = NextPositionFromDoorCollision(result, nextPos, character->getRadius(), iteratorTileCoord, nextTileCoord, door);
						if (tile->getLever() != nullptr){
							nextPos = NextPositionFromCollision(result, nextPos, 0.15f, iteratorTileCoord);

							if (result)
							{
								selectedObject = tile->getLever();
								if (!selectedObject->IsSelected())
									selectedObject->SetSelected((true));
							}
							else
							{
								if (selectedObject != nullptr)
								{
									selectedObject->SetSelected(false);
									selectedObject = nullptr;
								}
							}
						}
					}
				}
			}
		}
	}
	return nextPos;
}

bool GameLogic::IsTileWalkable(Tile* tile)
{
	if (tile == nullptr){
		return false;
	}

	if (tile->getPressurePlate() != nullptr)
	{
		return true;
	}

	if (tile->getMovableObject() != nullptr ||
		tile->getShadowContainer() != nullptr ||
		tile->getStaticObject() != nullptr ||
		tile->getFloorTile() == nullptr)
	{
		return false;
	}
	return true;
}

XMFLOAT3 GameLogic::NextPositionFromCollision(bool& result, XMFLOAT3 nextPos, float radius, Coord tileCoord)
{
	//Calculate the tiles edge coordinates.
	float xMin = -(tileCoord.x + TILE_SIZE);
	float xMax = (float)-tileCoord.x;
	float yMin = -(tileCoord.y + TILE_SIZE);
	float yMax = (float)-tileCoord.y;

	//Find the closest point to the character on the perimiter of the tile.
	float closestX = Clamp(nextPos.x, xMin, xMax);
	float closestY = Clamp(nextPos.z, yMin, yMax);

	float distanceX = nextPos.x - closestX;
	float distanceY = nextPos.z - closestY;
	float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);

	if (distanceSquared < (radius * radius))
	{
		float length = sqrt(distanceSquared);

		nextPos.x = (distanceX / length) * radius + closestX;
		nextPos.z = (distanceY / length) * radius + closestY;

		result = true;
	}
	else
	{
		result = false;
	}
	return nextPos;
}

XMFLOAT3 GameLogic::NextPositionFromDoorCollision(bool& result, XMFLOAT3 nextPos, float radius, Coord iteratorTileCoord, Coord nextTileCoord, Door* door)
{
	if (door != nullptr && !door->getIsOpen()) {
		XMFLOAT3 doorRot = door->GetRotationRad();
		int doorRotX = (int)round(cos(doorRot.y));
		int doorRotY = (int)round(sin(doorRot.y));

		if (iteratorTileCoord.y == nextTileCoord.y){
			if (doorRotY < 0){
				nextPos = NextPositionFromCollision(result, nextPos, radius, Coord(iteratorTileCoord.x, iteratorTileCoord.y - 1));
			}
			else if (doorRotY > 0){
				nextPos = NextPositionFromCollision(result, nextPos, radius, Coord(iteratorTileCoord.x, iteratorTileCoord.y + 1));
			}
		}
		else if (iteratorTileCoord.y < nextTileCoord.y){
			if (doorRotY > 0){
				nextPos = NextPositionFromCollision(result, nextPos, radius, Coord(iteratorTileCoord.x, iteratorTileCoord.y));
			}
		}
		else if (iteratorTileCoord.y > nextTileCoord.y){
			if (doorRotY < 0){
				nextPos = NextPositionFromCollision(result, nextPos, radius, Coord(iteratorTileCoord.x, iteratorTileCoord.y));
			}
		}


		if (iteratorTileCoord.x == nextTileCoord.x){
			if (doorRotX > 0){
				nextPos = NextPositionFromCollision(result, nextPos, radius, Coord(iteratorTileCoord.x - 1, iteratorTileCoord.y));
			}
			else if (doorRotX < 0){
				nextPos = NextPositionFromCollision(result, nextPos, radius, Coord(iteratorTileCoord.x + 1, iteratorTileCoord.y));
			}
		}
		else if (iteratorTileCoord.x < nextTileCoord.x){
			if (doorRotX < 0){
				nextPos = NextPositionFromCollision(result, nextPos, radius, Coord(iteratorTileCoord.x, iteratorTileCoord.y));
			}
		}
		else if (iteratorTileCoord.x > nextTileCoord.x){
			if (doorRotX > 0){
				nextPos = NextPositionFromCollision(result, nextPos, radius, Coord(iteratorTileCoord.x, iteratorTileCoord.y));
			}
		}
	}

	return nextPos;
}


bool GameLogic::inLight(LightObject* spotlight, GameObject* enemyObject)
{

	XMFLOAT3 lightEnemyVec = XMFLOAT3((spotlight->getPosition().x - enemyObject->GetPosition().x), (spotlight->getPosition().y - enemyObject->GetPosition().y), (spotlight->getPosition().z - enemyObject->GetPosition().z));

	float vecLenght = sqrt((lightEnemyVec.x * lightEnemyVec.x) + (lightEnemyVec.y * lightEnemyVec.y) + (lightEnemyVec.z * lightEnemyVec.z));

	if (spotlight->getRange() < vecLenght)
	{
		return false;
	}


	XMVECTOR spotDirection = XMLoadFloat3(&spotlight->getDirection());
	XMVECTOR lightEnemyVector = XMLoadFloat3(&lightEnemyVec);



	//if (spotlight->getCone() < XMVector3AngleBetweenVectors(spotDirection, lightEnemyVector))
	//{
	//	return false;
	//}



	return true;

}