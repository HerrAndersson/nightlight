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

	InitManagers(hwnd, fullscreen);
	LoadAssets();
}

void Game::InitManagers(HWND hwnd, bool fullscreen)
{
	AI = new AiModule(currentLevel);
	Logic = new GameLogic(hwnd, screenWidth, screenHeight, AI);
	Renderer = new RenderModule(hwnd, screenWidth, screenHeight, fullscreen);
	Assets = new AssetManager(Renderer->GetDevice());
	Levels = new LevelParser(Assets);
}

void Game::LoadAssets()
{
	character = new Character(XMFLOAT3(0, 0, 0), 0, Assets->GetRenderObject(7), 0, 0);
	if (currentLevel != nullptr)
	{
		delete currentLevel;
		currentLevel = nullptr;
	}

	currentLevel = Levels->LoadLevel(0, enemies, *character);
}

Game::~Game()
{
	delete currentLevel;

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

	result = Logic->Update(currentLevel, character, camera, spotLight, &enemies);

	return result;
}

bool Game::Render()
{
	bool result = true;

	std::vector<GameObject*>* toRender = currentLevel->GetGameObjects();

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
