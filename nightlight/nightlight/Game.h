#pragma once
#include <Windows.h>
#include <DirectXMath.h>

#include "RenderModule.h"
#include "GameLogic.h"

#include "D3DManager.h"
#include "InputManager.h"
#include "AssetManager.h"
#include "AssetUtil.h"

using namespace DirectX;

class Game
{
private:

	RenderObject* asset;//single test asset

	enum ObjectIDs
	{
		PLAYER = 1,

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

	XMMATRIX world; //needed?

	//Object** objects; use vector?.
	//World* world; this is the game world consisting of tiles in either Tile** or vector<vector<Tile>> format.  world[2][5] gives tile on position (2,5) in the grid.


	GameLogic*              Logic;
	RenderModule*           Renderer;

	InputManager*           Input;
	AssetManager *			Assets;


	/*
	To be implemented:

	SoundManager
	SaveLoadManager
	Timer
	Camera? Or should we have Player object containing a camera?

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

