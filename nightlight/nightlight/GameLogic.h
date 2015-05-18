#pragma once
#include <Windows.h>
#include <iostream>
#include <math.h>
#include "InputManager.h"
#include "cameraObject.h"
#include "lightObject.h"
#include "Character.h"
#include "Level.h"
#include "AiModule.h"
#include "LevelParser.h"

struct LevelStates
{
	int currentLevelNr = -1;
	int savedLevelNr = -1;
	Level* currentLevel = nullptr;
	Level* loadedLevel = nullptr;
	Level* menuLevel = nullptr;
	LevelParser* levelParser = nullptr;
};

class GameLogic
{
private:

	enum Axis { X, Y };
	enum Direction {NONE, RIGHT, UP, LEFT, DOWN};
	//enum SelectionTypes {NONE, BUTTON, LEVER, MOVABLEOBJECT};
	enum CollisionTypes { CHARACTER, ENEMY, MOVABLEOBJECT };

	InputManager*  Input;
	AiModule*      AI;

	bool quitGame = true;

	int screenWidth;
	int screenHeight;

	bool leftMouseLastState = false;
	bool moveObjectMode = false;
	bool loadedLevelMoveObjectMode = false;
	XMFLOAT3 loadedLevelCharacterRot;
	int moveObjectModeAxis = -1;
	XMFLOAT3 movableObjectTilePos;

	GameObject* selectedObject = nullptr;
	Button* selectedButton = nullptr;

	bool ManageLevelStates(LevelStates &levelStates, Character* character, vector<Enemy>& enemies);

	bool UpdatePlayer(LevelStates& levelStates, Character* player, CameraObject* camera, LightObject* spotLight, vector<Enemy>& enemies);
	bool UpdateSpotLight(LevelStates& levelStates, Character* player, CameraObject* camera, LightObject* spotlight, vector<Enemy>& enemies);
	bool UpdateAI(vector<Enemy>& enemies, Character* player);
	
	XMFLOAT3 ManageCollisions(Level* currentLevel, GameObject* gameObject, XMFLOAT3 pos, CollisionTypes type);
	XMFLOAT3 ManagePlayerCollision(Tile* iteratorTile, Coord iteratorTileCoord, Coord nextTileCoord, Coord currentTileCoord, float characterRadius, XMFLOAT3 nextPos);

	void SelectObject(GameObject* newSelectedObject);
	void SelectButton(Button* newSelectedButton);

	XMFLOAT3 NextPositionFromCollision(bool& result, XMFLOAT3 nextPos, float radius, Coord tileCoord);
	XMFLOAT3 NextPositionFromDoorCollision(bool& result, XMFLOAT3 nextPos, float radius, Coord iteratorTileCoord, Coord nextTileCoord, Door* door);

	float Clamp(float x, float a, float b) { return x < a ? a : (x > b ? b : x); };

public:

	GameLogic(HWND hwnd, int screenWidth, int screenHeight, AiModule* AI);
	~GameLogic();

	bool Update(LevelStates& levelStates, Character* gameObject, CameraObject* camera, LightObject* spotLight, vector<Enemy>& enemies);
	
	//UpdateObjects(objects)
	//UpdateAI(aiObjects)
	//Etc
};

