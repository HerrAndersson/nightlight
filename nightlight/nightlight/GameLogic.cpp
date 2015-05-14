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

bool GameLogic::Update(LevelStates& levelStates, Character* character, CameraObject* camera, LightObject* spotLight, vector<Enemy>* enemies)
{
	if (!UpdatePlayer(levelStates.currentLevel, character, camera, spotLight))
		return false;

	if (!UpdateAI(enemies, character))
		return false;

	if (!ManageLevelStates(levelStates, character, enemies))
		return false;

	Input->Update();

	return true;
}

bool GameLogic::UpdateAI(vector<Enemy>* enemies, Character* player)
{
	for each (Enemy e in *enemies)
	{
		AI->HandleAI(&e, player);
	}
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
	float movementOffset = 0.0f;
	Coord movementOffsetCoord;

	if (Input->KeyDown('w'))
	{
		if (!moveObjectMode || (moveObjectMode && moveObjectModeAxis == Axis::Y))
		{
			movementOffsetCoord = Coord(0, 1);
			pos = XMFLOAT3(pos.x, pos.y, pos.z + character->GetSpeed());
			playerMoved = true;
		}
	}
	if (Input->KeyDown('s'))
	{
		if (!moveObjectMode || (moveObjectMode && moveObjectModeAxis == Axis::Y))
		{
			movementOffsetCoord = Coord(0, -1);
			pos = XMFLOAT3(pos.x, pos.y, pos.z - character->GetSpeed());
			playerMoved = true;
		}
	}
	if (Input->KeyDown('a'))
	{
		if (!moveObjectMode || (moveObjectMode && moveObjectModeAxis == Axis::X))
		{
			movementOffsetCoord = Coord(1, 0);
			pos = XMFLOAT3(pos.x + character->GetSpeed(), pos.y, pos.z);
			playerMoved = true;
		}
	}
	if (Input->KeyDown('d'))
	{
		if (!moveObjectMode || (moveObjectMode && moveObjectModeAxis == Axis::X))
		{
			movementOffsetCoord = Coord(-1, 0);
			pos = XMFLOAT3(pos.x - character->GetSpeed(), pos.y, pos.z);
			playerMoved = true;
		}
	}

	if (playerMoved)
	{
		pos = ManageCollisions(currentLevel, character, pos, CollisionTypes::CHARACTER);

		//if (selectedObject != nullptr)
		//{
		//	XMFLOAT3 movablePos = selectedObject->GetPosition();
		//	movablePos.x += movementOffsetCoord.x;
		//	movablePos.y += movementOffsetCoord.y;
		//	movablePos = ManageCollisions(currentLevel, selectedObject, movablePos, CollisionTypes::MOVABLEOBJECT);
		//}
	}

	if (Input->LeftMouseDown() && leftMouseLastState == false)
	{
		leftMouseLastState = true;

		if (!moveObjectMode)
		{
			Lever* lever = dynamic_cast<Lever*>(selectedObject);
			MovableObject* movable = dynamic_cast<MovableObject*>(selectedObject);

			if (movable != nullptr)
			{
				moveObjectMode = true;
				Coord movableCoord = selectedObject->GetTileCoord();
				Coord characterCoord = character->GetTileCoord();
				XMFLOAT3 characterRot = character->GetRotationDeg();

				pos = XMFLOAT3(-characterCoord.x - TILE_SIZE / 2, 0.0f, -characterCoord.y - TILE_SIZE / 2);

				if (movableCoord.y > characterCoord.y)
				{
					characterRot.y = -90.0f;
					moveObjectModeAxis = Axis::Y;
				}
				else if (movableCoord.y < characterCoord.y)
				{
					characterRot.y = 90.0f;
					moveObjectModeAxis = Axis::Y;
				}
				else if (movableCoord.x > characterCoord.x)
				{
					characterRot.y = 0.0f;
					moveObjectModeAxis = Axis::X;
				}
				else if (movableCoord.x < characterCoord.x)
				{
					characterRot.y = 180.0f;
					moveObjectModeAxis = Axis::X;
				}
				character->SetRotationDeg(characterRot);


			}
		}
		else
		{
			moveObjectMode = false;
		}

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
	}
	if (!Input->LeftMouseDown() && leftMouseLastState == true)
	{
		leftMouseLastState = false;
	}

	if (!moveObjectMode)
	{
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
	}

	camera->SetPosition(character->GetPosition().x, -12, character->GetPosition().z - 3.5f);
	camera->SetLookAt(character->GetPosition().x, 5.0f, character->GetPosition().z*1.005f);
	character->SetPosition(pos);
	currentLevel->setPlayerPosition(pos);

	UpdateSpotLight(character, camera, spotlight);

	return true;
}

bool GameLogic::UpdateSpotLight(Character* player, CameraObject* camera, LightObject* spotlight)
{
	XMFLOAT3 pForward;
	XMStoreFloat3(&pForward, player->GetForwardVector());
	spotlight->setDirection(pForward.x, pForward.y, pForward.z);

	float length = sqrt((pForward.x*pForward.x) + (pForward.y*pForward.y) + (pForward.z*pForward.z));

	pForward.x = pForward.x / length;
	pForward.y = pForward.y / length;
	pForward.z = pForward.z / length;

	XMFLOAT3 pPos = player->GetPosition();
	//offset light
	pPos.x -= pForward.x / 2;
	pPos.z -= pForward.z / 2;
	pPos.y -= 0.7f;
	spotlight->setPosition(pPos.x, pPos.y, pPos.z);



	if (inLight(spotlight, XMFLOAT3(0, 0, 0)) == true)
	{
		spotlight->setDiffuseColor(0.0f, 1.0f, 0.0f, 1.0f);
		spotlight->setAmbientColor(0.2f, 0.01f, 0.8f, 1.0f);
	}
	else
	{
		spotlight->setAmbientColor(0.09f, 0.09f, 0.09f, 1.0f);
		spotlight->setDiffuseColor(0.55f, 0.45f, 0.2f, 1.0f);
	}

	spotlight->generateViewMatrix();
	return true;

}

XMFLOAT3 GameLogic::ManageCollisions(Level* currentLevel, GameObject* gameObject, XMFLOAT3 nextPos, CollisionTypes type)
{
	XMFLOAT3 currentPos = gameObject->GetPosition();
	float characterRadius = 1.0f;

	if (type == CollisionTypes::CHARACTER)
		characterRadius = ((Character*)gameObject)->getRadius();

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
					Tile* iteratorTile = currentLevel->getTile(x, y);
					Coord iteratorTileCoord = Coord(x, y);
					nextPos = ManagePlayerCollision(iteratorTile, iteratorTileCoord, nextTileCoord, gameObject->GetTileCoord(), characterRadius, nextPos);
				}
			}
		}
	}
	return nextPos;
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


bool GameLogic::inLight(LightObject* spotlight, XMFLOAT3& enemy)
{

	XMFLOAT3 lightEnemyVec = XMFLOAT3((enemy.x - spotlight->getPosition().x), (enemy.y - spotlight->getPosition().y), (enemy.z - spotlight->getPosition().z));

	float vecLenght = sqrt((lightEnemyVec.x * lightEnemyVec.x) + (lightEnemyVec.y * lightEnemyVec.y) + (lightEnemyVec.z * lightEnemyVec.z));


	if ((spotlight->getRange() / 2) > vecLenght)
	{


		XMFLOAT3 spotDirection = spotlight->getDirection();

		float dot = spotDirection.x*lightEnemyVec.x + spotDirection.y*lightEnemyVec.y + spotDirection.z*lightEnemyVec.z;
		float lenSq1 = spotDirection.x*spotDirection.x + spotDirection.y*spotDirection.y + spotDirection.z*spotDirection.z;
		float lenSq2 = lightEnemyVec.x*lightEnemyVec.x + lightEnemyVec.y*lightEnemyVec.y + lightEnemyVec.z*lightEnemyVec.z;
		float angle = acos(dot / sqrt(lenSq1 * lenSq2));

		float angleInRads = (180 / XM_PI) * angle;


		if (spotlight->getCone() < angleInRads)
		{
			return false;
		}
		return true;

	}


	return false;

}

XMFLOAT3 GameLogic::ManagePlayerCollision(Tile* iteratorTile, Coord iteratorTileCoord, Coord nextTileCoord, Coord currentTileCoord, float characterRadius, XMFLOAT3 nextPos)
{
	bool result = false;

	if (!iteratorTile->IsWalkable())
	{
		nextPos = NextPositionFromCollision(result, nextPos, characterRadius, iteratorTileCoord);
		if (iteratorTile != nullptr)
		{
			MovableObject* movableObject = iteratorTile->getMovableObject();
			if (movableObject != nullptr)
			{
				if (result && (currentTileCoord.x == iteratorTileCoord.x || currentTileCoord.y == iteratorTileCoord.y))
				{
					SelectObject(movableObject);
				}
				else
				{
					SelectObject(nullptr);
					//moveObjectMode = false;
				}
			}
		}
	}
	else
	{
		Door* door = iteratorTile->getDoor();
		nextPos = NextPositionFromDoorCollision(result, nextPos, characterRadius, iteratorTileCoord, nextTileCoord, door);

		Lever* lever = iteratorTile->getLever();
		if (lever != nullptr)
		{
			nextPos = NextPositionFromCollision(result, nextPos, 0.15f, iteratorTileCoord);

			if (result)
				SelectObject(lever);
			else
				SelectObject(nullptr);
		}

		PressurePlate* pressurePlate = iteratorTile->getPressurePlate();
		if (pressurePlate != nullptr)
		{
			if (iteratorTile->getMovableObject() != nullptr || nextTileCoord == iteratorTileCoord)
			{
				if (!pressurePlate->getIsActivated())
					pressurePlate->ActivatePressurePlate();
			}
			else
			{
				if (pressurePlate->getIsActivated())
					pressurePlate->DeactivatePressurePlate();
			}
		}
	}
	return nextPos;
}

void GameLogic::SelectObject(GameObject* newSelectedObject)
{
	if (newSelectedObject != nullptr)
	{
		SelectObject(nullptr);

		selectedObject = newSelectedObject;
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

bool GameLogic::ManageLevelStates(LevelStates &levelStates, Character* character, vector<Enemy>* enemies)
{
	Level* currentLevel = levelStates.currentLevel;
	Level* menuLevel = levelStates.menuLevel;
	Level* loadedLevel = levelStates.loadedLevel;

	bool restart = false;

	if (Input->LeftMouseClicked())
	{
		//levelNumberToLoad = 1;
		if (currentLevel == menuLevel){
			Coord characterTileCoord = character->GetTileCoord();
			Tile* currentTile = currentLevel->getTile(characterTileCoord.x, characterTileCoord.y);
			Button* button = currentTile->getButton();

			if (button != nullptr)
			{
				int buttonType = button->getButtonType();
				switch (buttonType)
				{
				case Button::ButtonTypes::START:
					levelStates.currentLevelNr = 1;
					restart = true;
					break;
				case Button::ButtonTypes::CONTINUE:
					if (loadedLevel)
						levelStates.currentLevelNr = loadedLevel->GetLevelNr();
					else if (levelStates.savedLevelNr != -1)
						levelStates.currentLevelNr = levelStates.savedLevelNr;
					break;
				case Button::ButtonTypes::EXIT:
					return false;
					break;
				default:
					break;
				}
			}
		}
	}


	if (Input->EscClicked())
	{
		if (currentLevel->GetLevelNr() != menuLevel->GetLevelNr())
			levelStates.currentLevelNr = menuLevel->GetLevelNr();
		else
			levelStates.currentLevelNr = loadedLevel->GetLevelNr();
	}


	if (currentLevel->GetLevelNr() != levelStates.currentLevelNr)
	{
		if (levelStates.currentLevelNr == menuLevel->GetLevelNr())
		{
			loadedLevelCharacterRot = character->GetRotationDeg();
			loadedLevelMoveObjectMode = moveObjectMode;
			moveObjectMode = false;

			currentLevel = menuLevel;
			character->SetTilePosition(currentLevel->getStartDoorCoord());
		}
		else if (!loadedLevel || loadedLevel->GetLevelNr() != levelStates.currentLevelNr || restart)
		{
			selectedObject = nullptr;
			moveObjectMode = false;

			if (loadedLevel)
				delete loadedLevel;

			loadedLevel = levelStates.levelParser->LoadLevel(levelStates.currentLevelNr, *enemies, *character);
			currentLevel = loadedLevel;
			character->SetTilePosition(currentLevel->getStartDoorCoord());
		}
		else
		{
			character->SetRotationDeg(loadedLevelCharacterRot);
			moveObjectMode = loadedLevelMoveObjectMode;
			currentLevel = loadedLevel;
			character->SetPosition(currentLevel->getPlayerPostion());
		}
	}

	levelStates.currentLevel = currentLevel;
	levelStates.loadedLevel = loadedLevel;

	return true;
}
