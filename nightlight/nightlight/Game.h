#pragma once
#include <Windows.h>
#include <DirectXMath.h>

#include "RenderModule.h"
#include "GameLogic.h"
#include "AiModule.h"
#include "AssetManager.h"
#include "LevelParser.h"
#include "SaveLoadManager.h"

#include "AssetUtil.h"
#include "GameObject.h"
#include "cameraObject.h"
#include "lightObject.h"

using namespace DirectX;
using std::vector;

class Game
{
private:

	int screenWidth, screenHeight;
	std::string saveFilePath = "nightlight.sav";

	LevelStates             levelStates;

	vector<Enemy>		    enemies;

	CameraObject*			camera;
	LightObject*			spotLight;
	Character*				character;

	GameLogic*              Logic;
	RenderModule*           Renderer;
	AiModule*               AI;
	AssetManager*			Assets;
	LevelParser*			levelParser;
	SaveLoadManager			saveLoadManager;
	InputManager*			Input;


public:

	Game(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight, bool fullscreen);
	~Game();

	bool Update(); //determines which objects to update and then calls Logic->Update(objectsToUpdate);
	bool Render(); //determines which objects to render and then calls Render->Render(objectsToRender);

	//Overloading these guarantees 16B alignment of XMMATRIX
	void* operator new(size_t i);
	void operator delete(void* p);
	void UpdateCharacterAnimation();
};

