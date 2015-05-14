#pragma once
#include <Windows.h>
#include <DirectXMath.h>

#include "RenderModule.h"
#include "GameLogic.h"
#include "AiModule.h"

#include "AssetManager.h"
#include "AssetUtil.h"
#include "LevelParser.h"
#include "GameObject.h"
#include "cameraObject.h"
#include "lightObject.h"

using namespace DirectX;
using std::vector;

class Game
{
private:

	enum GameStates { QUIT, MENU, GAME, GAME_OVER };

	int activeGameState;
	int screenWidth, screenHeight;

	Level* currentLevel = nullptr;
	Level* loadedLevel = nullptr;
	Level* menuLevel = nullptr;

	int currentLevelNr = 0;

	vector<Enemy>		    enemies;

	CameraObject*			camera;
	LightObject*			spotLight;
	Character*				character;

	GameLogic*              Logic;
	RenderModule*           Renderer;
	AiModule*               AI;
	AssetManager*			Assets;
	LevelParser*			Levels;

	/*
	To be implemented:

	SoundManager
	SaveLoadManager

	*/

public:

	Game(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight, bool fullscreen);
	~Game();

	bool Update(); //determines which objects to update and then calls Logic->Update(objectsToUpdate);
	bool Render(); //determines which objects to render and then calls Render->Render(objectsToRender);
	void SwitchLevel(Level* newlevel, int newLevelNr = -1);

	//Overloading these guarantees 16B alignment of XMMATRIX
	void* operator new(size_t i);
	void operator delete(void* p);
};

