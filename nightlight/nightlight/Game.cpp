#include "Game.h"

Game::Game(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight, bool fullscreen)
{
	srand(1);
	weights[0] = 1;//todo
	weights[1] = 0;
	weights[2] = 0;
	weights[3] = 0;

	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	camera = new CameraObject(XM_PI / 3, screenWidth, screenHeight, 0.1f, 1000);
	spotLight = new LightObject();
	spotLight->generateProjMatrix(0.1f, 1000);

	spotLight->setAmbientColor(0.09f, 0.09f, 0.09f, 1.0f);
	spotLight->setDiffuseColor(0.55f, 0.45f, 0.2f, 1.0f);

	Renderer = new RenderModule(hwnd, screenWidth, screenHeight, fullscreen);
	Assets = new AssetManager(Renderer->GetDevice());
	levelParser = new LevelParser(Assets);
	saveLoadManager = SaveLoadManager();
	
	character = new Character(XMFLOAT3(0, 0, 0), 0, Assets->GetRenderObject(7), 0, 0);

	levelStates.menuLevel = levelParser->LoadLevel(0, enemies, *character);
	levelStates.currentLevel = levelStates.menuLevel;
	levelStates.currentLevelNr = levelStates.menuLevel->GetLevelNr();
	levelStates.levelParser = levelParser;
	saveLoadManager.Load(saveFilePath, levelStates.savedLevelNr);

	AI = new AiModule(levelStates.currentLevel);
	Logic = new GameLogic(hwnd, screenWidth, screenHeight, AI);

	///////////////////////////////////// DEBUG FOR PATHFINDING /////////////////////////////////////
	//character->SetPosition(XMFLOAT3(-4 - TILE_SIZE / 2, 0, -4 - TILE_SIZE / 2));
	///////////////////////////////////// DEBUG FOR PATHFINDING /////////////////////////////////////
}

Game::~Game()
{
	delete levelStates.menuLevel;
	if (levelStates.loadedLevel != nullptr)
		delete levelStates.loadedLevel;

	delete Logic;
	delete Renderer;
	delete Assets;
	delete AI;
	delete levelParser;

	delete camera;
	delete spotLight;
	delete character;
}

bool Game::Update()
{
	bool result = true;
	
	result = Logic->Update(levelStates, character, camera, spotLight, &enemies);
	
	if (!result){
		Level* loadedLevel = levelStates.loadedLevel;
		if (loadedLevel)
			saveLoadManager.Save(saveFilePath, loadedLevel->GetLevelNr());
	}

	return result;
}

bool Game::Render()
{
	bool result = true;

	std::vector<GameObject*>* toRender = levelStates.currentLevel->GetGameObjects();



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


	int weightchange[4] = { rand() % 100, rand() % 100, rand() % 100, rand() % 100 };//todo
	int totalweightchange = weightchange[0] + weightchange[1] + weightchange[2] + weightchange[3];
	float finalweightchange[4];
	finalweightchange[0] = (float)(weightchange[0]) / totalweightchange * 4 - 1;
	finalweightchange[1] = (float)(weightchange[1]) / totalweightchange * 4 - 1;
	finalweightchange[2] = (float)(weightchange[2]) / totalweightchange * 4 - 1;
	finalweightchange[3] = (float)(weightchange[3]) / totalweightchange * 4 - 1;
	float totalweight=0;
	for (int i = 0; i < 4; i++){
		weights[i] += (finalweightchange[i]/80);
		totalweight += weights[i];
		if (weights[i] < 0)
			weights[i] =0;
		if (weights[i] > 1)
			weights[i] =1;
	}
	weights[0] /= totalweight;
	weights[1] /= totalweight;
	weights[2] /= totalweight;
	weights[3] /= totalweight;
	system("CLS");
	cout << weights[0] << endl << weights[1] << endl << weights[2] << endl << weights[3] << endl << totalweight;
	
	
	Renderer->Render(character, weights);

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
