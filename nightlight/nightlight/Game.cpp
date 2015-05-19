#include "Game.h"

Game::Game(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight, bool fullscreen)
{

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
	Input = new InputManager(hwnd, screenWidth, screenHeight);
	
	character = new Character(XMFLOAT3(0, 0, 0), 0, Assets->GetRenderObject(7), 0, 0);

	levelStates.menuLevel = levelParser->LoadLevel(0, enemies, *character);
	levelStates.currentLevel = levelStates.menuLevel;
	levelStates.currentLevelNr = levelStates.menuLevel->GetLevelNr();
	levelStates.levelParser = levelParser;
	saveLoadManager.Load(saveFilePath, levelStates.savedLevelNr);

	AI = new AiModule(levelStates.currentLevel);
	Logic = new GameLogic(AI, Input);

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
	delete Input;

	delete camera;
	delete spotLight;
	delete character;
}

bool Game::Update()
{
	bool result = true;
	
	result = Logic->Update(levelStates, character, camera, spotLight, enemies);
	
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

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	worldMatrix = DirectX::XMMatrixIdentity();
	camera->GetProjectionMatrix(projectionMatrix);
	camera->GetViewMatrix(viewMatrix);

	vector<XMINT2> p;
	if (enemies.size() > 0)
		p = AI->GetPath(levelStates.currentLevel, XMINT2(enemies.at(0).GetTileCoord().x, enemies.at(0).GetTileCoord().y), XMINT2(character->GetTileCoord().x, character->GetTileCoord().y));

	Renderer->SetDataPerFrame(viewMatrix, projectionMatrix, camera->GetPosition(), spotLight, &levelStates);

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

	if (levelStates.currentLevelNr != levelStates.menuLevel->GetLevelNr())
	{
		for (Enemy e : enemies) 
		{
			XMFLOAT4 weight;
			e.UpdateWeights(weight);
			Renderer->Render(&e, weight);
		}
	}

	character->UpdateCharacterAnimation(character->GetMoved());

	Renderer->Render(character, character->GetBlendWeights());

	Renderer->EndScene();

	for (auto x : p)
	{
		Coord c = Coord(x.x, x.y);
		Tile* t = levelStates.currentLevel->getTile(c);

		if (t)
		{
			if (t->getFloorTile())
				t->getFloorTile()->SetSelected(false);
			if (t->getPressurePlate())
				t->getPressurePlate()->SetSelected(false);
		}
	}
	p.clear();

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
