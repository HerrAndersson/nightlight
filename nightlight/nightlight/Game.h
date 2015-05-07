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

class Game
{
private:

	enum ObjectIDs
	{
		PLAYER		= 1,
		ENEMY		= 2,
		TILE_FLOOR	= 3,
		TILE_WALL   = 4,
		TILE_DOOR   = 5
		//ETC?
	};

	enum GameStates
	{
		QUIT		= 0,
		MENU		= 1,
		GAME		= 2,
		GAME_OVER	= 3
	};

	int activeGameState;
	int screenWidth, screenHeight;

	Level* currentLevel = nullptr;

	std::vector<Enemy>		enemies;

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

	void InitManagers(HWND hwnd, bool fullscreen);
	void LoadAssets();

public:

	Game(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight, bool fullscreen);
	~Game();

	bool Update(); //determines which objects to update and then calls Logic->Update(objectsToUpdate);
	bool Render(); //determines which objects to render and then calls Render->Render(objectsToRender);

	//Overloading these guarantees 16B alignment of XMMATRIX
	void* operator new(size_t i);
	void operator delete(void* p);
};

