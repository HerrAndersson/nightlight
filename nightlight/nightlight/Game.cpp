#include "Game.h"

Game::Game(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight, bool fullscreen)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	//activeGameState = MENU;

	camera = new CameraObject(XM_PI / 3, screenWidth, screenHeight, 0.1f, 1000);
	spotLight = new LightObject();
	spotLight->generateProjMatrix(0.1f, 1000);

	spotLight->setAmbientColor(0.09f, 0.09f, 0.09f, 1.0f);
	spotLight->setDiffuseColor(0.55f, 0.45f, 0.2f, 1.0f);

	Renderer = new RenderModule(hwnd, screenWidth, screenHeight, fullscreen);
	Assets = new AssetManager(Renderer->GetDevice());
	Levels = new LevelParser(Assets);

	character = new Character(XMFLOAT3(0, 0, 0), 0, Assets->GetRenderObject(7), 0, 0);

	
	menuLevel = Levels->LoadLevel(currentLevelNr++, enemies, *character);
	
	//TODO: remove this
	loadedLevel = Levels->LoadLevel(currentLevelNr, enemies, *character);
	SwitchLevel(menuLevel);

	AI = new AiModule(currentLevel);
	Logic = new GameLogic(hwnd, screenWidth, screenHeight, AI, menuLevel);

	///////////////////////////////////// DEBUG FOR PATHFINDING /////////////////////////////////////
	//character->SetPosition(XMFLOAT3(-4 - TILE_SIZE / 2, 0, -4 - TILE_SIZE / 2));
	///////////////////////////////////// DEBUG FOR PATHFINDING /////////////////////////////////////
}

Game::~Game()
{
	delete menuLevel;
	if (loadedLevel != nullptr)
		delete loadedLevel;

	delete Logic;
	delete Renderer;
	delete Assets;
	delete AI;
	delete Levels;

	delete camera;
	delete spotLight;
	delete character;
}

bool Game::Update()
{
	bool result = true;

	//switch (activeGameState)
	//{
	//	case QUIT:
	//	{
	//		result = false;
	//	}
	//	case MENU:
	//	{
	//		//UpdateMenu()
	//		//RenderMenu()
	//		break;
	//	}
	//	case GAME:
	//	{
	//		//UpdateGame()
	//		//RenderGame()
	//		break;
	//	}
	//	case GAME_OVER:
	//	{
	//		//UpdateGameOver()
	//		//RenderGameOver()
	//		break;
	//	}
	//	default:
	//		break;
	//}

	//if (levelNumberToLoad != currentLevelNr)
	//{
	//	SwitchLevel(loadedLevel, levelNumberToLoad);
	//	currentLevelNr = levelNumberToLoad;
	//}
	result = Logic->Update(currentLevel, character, camera, spotLight, &enemies);
	
	return result;
}

void Game::SwitchLevel(Level* newlevel, int newLevelNr)
{
	if (newlevel == nullptr)
	{
		if (newLevelNr != -1)
		{
			loadedLevel = Levels->LoadLevel(newLevelNr, enemies, *character);
			currentLevel = loadedLevel;
		}
	}
	else
	{
		if (newlevel == menuLevel){
			character->SetTilePosition(menuLevel->getStartDoorCoord());
		}
		else{
			character->SetPosition(newlevel->getPlayerPostion());
		}
		currentLevel = newlevel;
	}
}

bool Game::Render()
{
	bool result = true;

	std::vector<GameObject*>* toRender = currentLevel->GetGameObjects();



	///////////////////////////////////// DEBUG FOR PATHFINDING /////////////////////////////////////
	vector<XMINT2> p1;
	vector<XMINT2> p2;
	if (AiUtil_ShowDebugPath)
	{
		Coord c = character->GetTileCoord();
//	vector<XMFLOAT3> p1 = AI->GetPath(currentLevel, XMINT2(4, 2), XMINT2(c.x, c.y));
//	vector<XMFLOAT3> p2 = AI->GetPath(currentLevel, XMINT2(2, 12), XMINT2(c.x, c.y));
	}
	///////////////////////////////////// DEBUG FOR PATHFINDING /////////////////////////////////////



	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	worldMatrix = DirectX::XMMatrixIdentity();
	camera->GetProjectionMatrix(projectionMatrix);
	camera->GetViewMatrix(viewMatrix);

	Renderer->SetDataPerFrame(viewMatrix, projectionMatrix, camera->GetPosition(), spotLight);

	//Renderer->ActivateShadowRendering(viewMatrix, projectionMatrix);
	//
	//for (int i = 0; i < toRender->size(); i++) 
	//{
	//		Renderer->RenderShadow(toRender->at(i));
	//	}

	Renderer->BeginScene(0.05f, 0.05f, 0.05f, 1.0f);

	Renderer->UseDefaultShader();

	
	for (int i = 0; i < (signed)toRender->size(); i++) 
	{
		Renderer->Render(toRender->at(i));
	}

	for (Enemy e : enemies) 
	{
		Renderer->Render(&e);
	}

	Renderer->Render(character);

	Renderer->EndScene();



//	///////////////////////////////////// DEBUG FOR PATHFINDING /////////////////////////////////////
//	for (auto x : p1)
//	{
//		Tile* t = currentLevel->getTile((int)x.x, (int)x.z);
//		if (t)
//		{
//			if (t->getFloorTile())
//				t->getFloorTile()->SetSelected(false);
//			if (t->getPressurePlate())
//				t->getPressurePlate()->SetSelected(false);
//		}	
//	}
//
//	for (auto x : p2)
//	{
//		Tile* t = currentLevel->getTile((int)x.x,(int)x.z);
//		if (t)
//		{
//			if (t->getFloorTile())
//				t->getFloorTile()->SetSelected(false);
//			if (t->getPressurePlate())
//				t->getPressurePlate()->SetSelected(false);
//		}
//	}
	///////////////////////////////////// DEBUG FOR PATHFINDING /////////////////////////////////////



	return result;
}

void* Game::operator new(size_t i)
{
	return _mm_malloc(i, 16);
}

void Game::operator delete(void* p)
{
	_mm_free(p);
}
