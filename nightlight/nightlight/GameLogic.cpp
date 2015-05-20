#include "GameLogic.h"
#include "Collision.h"

GameLogic::GameLogic(AiModule* AI, InputManager* Input)
{
	this->Input = Input;
	this->AI = AI;
}

GameLogic::~GameLogic()
{

}

bool GameLogic::Update(LevelStates& levelStates, Character* character, CameraObject* camera, LightObject* spotLight, vector<Enemy>& enemies)
{
	if (!UpdatePlayer(levelStates, character, camera, spotLight, enemies))
		return false;

	if (!UpdateAI(enemies, character, spotLight))
		return false;

	if (!ManageLevelStates(levelStates, character, enemies))
		return false;

	Input->Update();

	return true;
}

bool GameLogic::UpdateAI(vector<Enemy>& enemies, Character* player, LightObject* spotlight)
{
	for (auto &e : enemies)
	{
		AI->HandleAI(&e, player, spotlight);
	}

	return true;
}

bool GameLogic::UpdatePlayer(LevelStates& levelStates, Character* character, CameraObject* camera, LightObject* spotlight, vector<Enemy>& enemies)
{
	Level* currentLevel = levelStates.currentLevel;
	XMFLOAT2 oldP = Input->GetMousePos();
	Input->HandleMouse();
	XMFLOAT2 newP = Input->GetMousePos();

	XMFLOAT3 pos = character->GetPosition();
	XMFLOAT3 rot = character->GetRotationDeg();

	int playerMoved = Direction::NONE;

	if (Input->KeyDown('w'))
	{
		if (moveObjectModeAxis == Axis::BOTH || moveObjectModeAxis == Axis::Y) {
			pos = XMFLOAT3(pos.x, pos.y, pos.z + character->GetSpeed());
			playerMoved = Direction::UP;
		}
	}
	if (Input->KeyDown('s'))
	{
		if (moveObjectModeAxis == Axis::BOTH || moveObjectModeAxis == Axis::Y) {
			pos = XMFLOAT3(pos.x, pos.y, pos.z - character->GetSpeed());
			playerMoved = Direction::DOWN;
		}
	}
	if (Input->KeyDown('a'))
	{
		if (moveObjectModeAxis == Axis::BOTH || moveObjectModeAxis == Axis::X) {
			pos = XMFLOAT3(pos.x + character->GetSpeed(), pos.y, pos.z);
			playerMoved = Direction::LEFT;
		}
	}
	if (Input->KeyDown('d'))
	{
		if (moveObjectModeAxis == Axis::BOTH || moveObjectModeAxis == Axis::X) {
			pos = XMFLOAT3(pos.x - character->GetSpeed(), pos.y, pos.z);
			playerMoved = Direction::RIGHT;
		}
	}

	if (playerMoved)
	{
		Coord movableObjectOffset;
		Coord nextMovableTileCoord = Coord();

		if (moveObjectMode) {
			Coord selectedObjectCurrentCoord = selectedObject->GetTileCoord();
			Coord characterCurrentCoord = character->GetTileCoord();

			int offsetX = selectedObjectCurrentCoord.x - characterCurrentCoord.x;
			int offsetY = selectedObjectCurrentCoord.y - characterCurrentCoord.y;

			if (offsetX < 0)
				offsetX = -1;
			else if (offsetX > 0)
				offsetX = 1;

			if (offsetY < 0)
				offsetY = -1;
			else if (offsetY > 0)
				offsetY = 1;

			movableObjectOffset = Coord(offsetX, offsetY);

			switch (playerMoved) {
			case Direction::UP:
				nextMovableTileCoord = Coord(characterCurrentCoord.x, characterCurrentCoord.y - 2);
				break;
			case Direction::DOWN:
				nextMovableTileCoord = Coord(characterCurrentCoord.x, characterCurrentCoord.y + 2);
				break;
			case Direction::RIGHT:
				nextMovableTileCoord = Coord(characterCurrentCoord.x + 2, characterCurrentCoord.y);
				break;
			case Direction::LEFT:
				nextMovableTileCoord = Coord(characterCurrentCoord.x - 2, characterCurrentCoord.y);
				break;
			default:
				break;
			}
			bool result;

			XMFLOAT3 nextMovablePos = pos;
			nextMovablePos.x -= movableObjectOffset.x;
			nextMovablePos.z -= movableObjectOffset.y;

			Tile* nextMovableTile = currentLevel->getTile(nextMovableTileCoord);

			if (!nextMovableTile->IsWalkable(moveObjectMode, selectedObject) || 
				(nextMovableTile->getDoor() && !nextMovableTile->getDoor()->getIsOpen()) || 
				nextMovableTile->getLever()){
				
				XMFLOAT3 currentMovablePos = nextMovablePos;
				nextMovablePos = NextPositionFromCollision(result, currentMovablePos, 0.5f, nextMovableTileCoord);

				if (currentMovablePos.x != nextMovablePos.x || currentMovablePos.z != nextMovablePos.z)
				{
					pos.x += nextMovablePos.x - currentMovablePos.x;
					pos.z += nextMovablePos.z - currentMovablePos.z;
				}
			}

			selectedObject->SetPosition(nextMovablePos);
		}
		
		pos = ManageCollisions(this, currentLevel, character, pos, CollisionTypes::CHARACTER);
		
		if (moveObjectMode)
		{
			XMFLOAT3 movablePos = pos;
			movablePos.x -= movableObjectOffset.x;
			movablePos.z -= movableObjectOffset.y;

			float zOffset = abs(movablePos.z - movableObjectTilePos.z);
			float xOffset = abs(movablePos.x - movableObjectTilePos.x);

			if (zOffset >= 1 || xOffset >= 1) 
			{
				currentLevel->getTile((int)-movableObjectTilePos.x, (int)-movableObjectTilePos.z)->setMovableObject(nullptr);
				currentLevel->getTile(selectedObject->GetTileCoord())->setMovableObject((MovableObject*)selectedObject);
				movableObjectTilePos = movablePos;
			}
		}
	}

	if (Input->LeftMouseClicked())
	{
		Lever* lever = nullptr;
		MovableObject* movable = nullptr;

		if (selectedObject != nullptr) {
			lever = dynamic_cast<Lever*>(selectedObject);
			movable = dynamic_cast<MovableObject*>(selectedObject);

			if (movable != nullptr) {
				moveObjectMode = !moveObjectMode;
			}
			if (lever != nullptr) {
				if (lever->getIsActivated()) {
					lever->DeactivateLever();
				} else {
					lever->ActivateLever();
				}
			}
		}

		if (moveObjectMode)
		{
			movableObjectTilePos = selectedObject->GetPosition();
			Coord movableCoord = selectedObject->GetTileCoord();
			Coord characterCoord = character->GetTileCoord();
			XMFLOAT3 characterRot = character->GetRotationDeg();

			pos = XMFLOAT3(-characterCoord.x - TILE_SIZE / 2, 0.0f, -characterCoord.y - TILE_SIZE / 2);

			if (movableCoord.y > characterCoord.y) {
				characterRot.y = -90.0f;
				moveObjectModeAxis = Axis::Y;
			} else if (movableCoord.y < characterCoord.y) {
				characterRot.y = 90.0f;
				moveObjectModeAxis = Axis::Y;
			} else if (movableCoord.x > characterCoord.x) {
				characterRot.y = 0.0f;
				moveObjectModeAxis = Axis::X;
			} else if (movableCoord.x < characterCoord.x) {
				characterRot.y = 180.0f;
				moveObjectModeAxis = Axis::X;
			}
			character->SetRotationDeg(characterRot);
		}
		else if (movable) {
			Coord lastTileCoord = Coord((int)-movableObjectTilePos.x, (int)-movableObjectTilePos.z);
			Coord currentTileCoord = movable->GetTileCoord();
			Tile* lastTile = currentLevel->getTile(lastTileCoord);
			Tile* currentTile = currentLevel->getTile(currentTileCoord);
			
			if (lastTile->getMovableObject()) {
				lastTile->setMovableObject(nullptr);
				currentTile->setMovableObject(movable);
			}

			movable->SetTilePosition(currentTileCoord);
			pos = ManageCollisions(this, currentLevel, character, pos, CollisionTypes::CHARACTER);
			moveObjectModeAxis = Axis::BOTH;
		}
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
	for (Enemy e : enemies)
	{
		if (sqrt((pow((e.GetPosition().x - pos.x), 2)) + (pow((e.GetPosition().z - pos.z), 2))) < 0.7f && character->GetInvulTimer() <= 0)
		{
			character->SetHitPoints(character->GetHitPoints() - 1);
			if (character->GetHitPoints() <= 0)
				character->GetRenderObject()->diffuseTexture = e.GetRenderObject()->diffuseTexture;//dead//todo
			character->SetInvulTimer(180);
		}
	}
	int invulTimer = character->GetInvulTimer();
	int blinkspeed = 4;
	if (character->GetHitPoints()>0)//temporär ifsats för att förhindra krashar då hp kan gå under 1
	blinkspeed=character->GetHitPoints()*10;
	if (invulTimer > 0)
	{
		int mod = invulTimer % blinkspeed;
		if (mod>((blinkspeed/2)-1))
			character->SetSelected(true);
		if (mod<(blinkspeed / 2))
			character->SetSelected(false);

// 		if (invulTimer > 2.5)
// 			character->SetSelected(true);
// 		if (invulTimer > 2 && invulTimer < 2.5)
// 			character->SetSelected(false);
// 		if (invulTimer > 1.5 && invulTimer < 2)
// 			character->SetSelected(true);
// 		if (invulTimer > 1 && invulTimer < 1.5)
// 			character->SetSelected(false);
// 		if (invulTimer > 0.5 && invulTimer < 1)
// 			character->SetSelected(true);
// 		if (invulTimer > 0 && invulTimer < 0.5)
// 			character->SetSelected(false);

		character->SetInvulTimer(invulTimer - 1);
	}
// 	system("CLS");
// 	printf("%f", character->GetInvulTimer());
// 	int debug = character->GetHitPoints();
	camera->SetPosition(character->GetPosition().x, -12, character->GetPosition().z - 3.5f);
	camera->SetLookAt(character->GetPosition().x, 5.0f, character->GetPosition().z*1.005f);
	character->SetPosition(pos);
	character->SetMoved(playerMoved);
	currentLevel->setPlayerPosition(pos);

	UpdateSpotLight(levelStates, character, camera, spotlight, enemies);

	return true;
}

bool GameLogic::UpdateSpotLight(LevelStates& levelStates, Character* player, CameraObject* camera, LightObject* spotlight, vector<Enemy>& enemies)
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

	//if (levelStates.currentLevelNr != levelStates.menuLevel->GetLevelNr())
	//{
	//	for (auto &e : enemies)
	//	{
	//		if (e.InLight(spotlight) == true)
	//		{
	//			spotlight->setDiffuseColor(0.0f, 1.0f, 0.0f, 1.0f);
	//			spotlight->setAmbientColor(0.2f, 0.01f, 0.8f, 1.0f);
	//		}
	//		else
	//		{
	//			spotlight->setAmbientColor(0.09f, 0.09f, 0.09f, 1.0f);
	//			spotlight->setDiffuseColor(0.55f, 0.45f, 0.2f, 1.0f);
	//		}
	//	}
	//}

	spotlight->generateViewMatrix();
	return true;

}

GameObject* GameLogic::GetSelectedObject()
{
	return selectedObject;
}
bool GameLogic::GetMoveObjectMode()
{
	return moveObjectMode;
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

void GameLogic::SelectButton(Button* newSelectedButton)
{
	if (newSelectedButton != nullptr)
	{
		SelectButton(nullptr);

		selectedButton = newSelectedButton;
		selectedButton->SetSelected((true));
	}
	else
	{
		if (selectedButton != nullptr)
		{
			selectedButton->SetSelected(false);
			selectedButton = nullptr;
		}
	}
}

bool GameLogic::ManageLevelStates(LevelStates &levelStates, Character* character, vector<Enemy>& enemies)
{
	Level* currentLevel = levelStates.currentLevel;
	Level* menuLevel = levelStates.menuLevel;
	Level* loadedLevel = levelStates.loadedLevel;

	bool restart = false;

	Coord characterTileCoord = character->GetTileCoord();
	Tile* currentTile = currentLevel->getTile(characterTileCoord.x, characterTileCoord.y);
	Button* button = currentTile->getButton();

	if (button)
		SelectButton(button);
	else
		SelectButton(nullptr);

	//Choose the correct level
	if (Input->LeftMouseClicked())
	{
		if (currentLevel == menuLevel)
		{
			if (button != nullptr)
			{
				int buttonType = button->getButtonType();
				switch (buttonType)
				{
				case Button::ButtonTypes::START:
					levelStates.currentLevelNr = 1;
					SelectObject(nullptr);
					restart = true;
					break;
				case Button::ButtonTypes::CONTINUE:
					if (loadedLevel)
						levelStates.currentLevelNr = loadedLevel->GetLevelNr();
					else if (levelStates.savedLevelNr != -1)
						levelStates.currentLevelNr = levelStates.savedLevelNr;
					SelectObject(nullptr);
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
	else if (Input->EscClicked())
	{
		if (currentLevel->GetLevelNr() != menuLevel->GetLevelNr())
			levelStates.currentLevelNr = menuLevel->GetLevelNr();
		else
			if (loadedLevel)
				levelStates.currentLevelNr = loadedLevel->GetLevelNr();
	}

	//Check for end door. If found, go to next level
	Door* door = currentTile->getDoor();
	if (door && door->getDoorType() == door->DoorTypes::END_DOOR && door->getIsOpen())
		levelStates.currentLevelNr++;

	//Set the correct level
	if (currentLevel->GetLevelNr() != levelStates.currentLevelNr)
	{
		if (levelStates.currentLevelNr == menuLevel->GetLevelNr())
		{
			loadedLevelCharacterRot = character->GetRotationDeg();
			loadedLevelMoveObjectMode = moveObjectMode;
			moveObjectMode = false;
			loadedLevelMoveObjectModeAxis = moveObjectModeAxis;
			moveObjectModeAxis = Axis::BOTH;

			currentLevel = menuLevel;
			character->SetTilePosition(currentLevel->getStartDoorCoord());
		}
		else if (!loadedLevel || loadedLevel->GetLevelNr() != levelStates.currentLevelNr || restart)
		{
			SelectObject(nullptr);
			moveObjectMode = false;
			moveObjectModeAxis = Axis::BOTH;

			if (loadedLevel){
				delete loadedLevel;
				enemies.clear();
			}
			int debug = levelStates.currentLevelNr;
			debug = levelStates.levelParser->GetLevelCount();
			if (levelStates.currentLevelNr > levelStates.levelParser->GetLevelCount()-1)
				throw std::runtime_error("Slut :^)");
			else
			{
				loadedLevel = levelStates.levelParser->LoadLevel(levelStates.currentLevelNr, enemies, *character);
				currentLevel = loadedLevel;
				character->SetTilePosition(currentLevel->getStartDoorCoord());
			}
		}
		else
		{		
			moveObjectMode = loadedLevelMoveObjectMode;
			moveObjectModeAxis = loadedLevelMoveObjectMode;
			currentLevel = loadedLevel;
			character->SetRotationDeg(loadedLevelCharacterRot);
			character->SetPosition(currentLevel->getPlayerPostion());
		}

		AI->ChangeLevel(currentLevel);
	}

	levelStates.currentLevel = currentLevel;
	levelStates.loadedLevel = loadedLevel;

	return true;
}