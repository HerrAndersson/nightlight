#include "Game.h"

Game::Game(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight, bool fullscreen)
{

	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	camera = new CameraObject(XM_PI / 3, screenWidth, screenHeight, 0.1f, 1000.0f);
	spotLight = new LightObject();
	spotLight->generateProjMatrix(0.1f, 1000.0f);

	spotLight->setAmbientColor(0.09f, 0.09f, 0.09f, 1.0f);
	spotLight->setDiffuseColor(0.55f, 0.45f, 0.2f, 1.0f);

	Renderer = new RenderModule(hwnd, screenWidth, screenHeight, fullscreen);
	Assets = new AssetManager(Renderer->GetDevice());
	levelParser = new LevelParser(Assets);
	saveLoadManager = SaveLoadManager();
	Input = new InputManager(hwnd, screenWidth, screenHeight);
	
	character = new Character(GameObject::GoTypes::MAINCHARACTER, XMFLOAT3(0, 0, 0), 0, Assets->GetRenderObject(7), 0, 0);
	character->SetUpAnimation(Assets->GetRenderObject(22), 1);

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

	vector<vector<XMINT2>> paths;

	if (debugRenderEnemyPaths)
	{
		for (auto e : enemies)
		{
			paths.push_back(e.GetPath());
		}

		//int i = 0;

		for (auto& path : paths)
		{
			//i++;
			for (auto& p : path)
			{
				Tile* tile = levelStates.currentLevel->getTile(p.x, p.y);
				if (tile)
				{
					GameObject* go = tile->getFloorTile();
					if (!go)
						go = tile->getPressurePlate();

					if (go)
					{
						XMFLOAT3 color = go->GetColorModifier();
						color.y += 0.3f;
						go->SetColorModifier(color);
					}
				}

			}
		}
	}
	XMMATRIX lightView, lightProj;
	spotLight->getViewMatrix(lightView);
	spotLight->getProjMatrix(lightProj);

	Renderer->ActivateShadowRendering(lightView, lightProj);
	
	for (int i = 0; i < toRender->size(); i++) 
	{
		//Depth test seem to be inverted. Objects in the back are rendered in the front. Probably because everything is inverted from maya?
		//Works if lightView + cameraProj is used! No idea why lightProj inverts stuff...
		Renderer->RenderShadow(toRender->at(i));
	}

	Renderer->BeginScene(0.05f, 0.05f, 0.05f, 1.0f);

	Renderer->SetDataPerFrame(lightView, lightProj, camera->GetPosition(), spotLight, &levelStates);
	Renderer->UseDefaultShader();

	
	for (int i = 0; i < (signed)toRender->size(); i++) 
	{
		bool renderThis = true;

		if (debugDisableWallRendering){
			int id = toRender->at(i)->GetId();

			if (id == GameObject::WALL
				|| id == GameObject::CORNER
				|| id == GameObject::CORNERIVERSE)
				renderThis = false;
		}

		if (renderThis)
			Renderer->Render(toRender->at(i), toRender->at(i)->GetWeights());
	}

	if (levelStates.currentLevelNr != levelStates.menuLevel->GetLevelNr())
	{
		for (auto& e : enemies) 
		{
			e.UpdateAnimation();
			Renderer->Render(&e, e.GetWeights());
// 			system("CLS");
// 			printf("%f\n%f\n%f\n%f", e.GetWeights().x, e.GetWeights().y, e.GetWeights().z, e.GetWeights().w);
		}
	}

	character->UpdateCharacterAnimation();

	Renderer->Render(character, character->GetWeights());

	Renderer->EndScene();



	for (auto& path : paths)
	{
		for (auto& p : path)
		{
			Tile* tile = levelStates.currentLevel->getTile(p.x, p.y);
			if (tile)
			{
				GameObject* go = tile->getFloorTile();
				if (!go)
					go = tile->getPressurePlate();

				if (go)
				{
					XMFLOAT3 color = go->GetColorModifier();
					color.y -= 0.3f;
					go->SetColorModifier(color);
				}
			}

		}
	}





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
