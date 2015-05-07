#include "Game.h"

Game::Game(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight, bool fullscreen)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	//activeGameState = MENU;

	camera = new CameraObject(XM_PI / 3, screenWidth, screenHeight, 0.1f, 1000);
	spotLight = new LightObject();

	InitManagers(hwnd, fullscreen);
	LoadAssets();
}

void Game::InitManagers(HWND hwnd, bool fullscreen)
{
	Logic = new GameLogic(hwnd, screenWidth, screenHeight);
	Renderer = new RenderModule(hwnd, screenWidth, screenHeight, fullscreen);
	Assets = new AssetManager(Renderer->GetDevice());
	Levels = new LevelParser(Assets);

	//AI = new AiModule(currentLevel->getTileGrid());
}

void Game::LoadAssets()
{

	character = new Character(XMFLOAT3(0, 0, 0), 0, Assets->GetRenderObject(7), 0, 0);
	if (currentLevel != nullptr){
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

	result = Logic->Update(currentLevel, character, camera, spotLight);

	//cout << character->GetPosition().x << " " << character->GetPosition().z << endl;

	return result;
}

bool Game::Render()
{
	bool result = true;

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	worldMatrix = DirectX::XMMatrixIdentity();
	camera->GetProjectionMatrix(projectionMatrix);
	camera->GetViewMatrix(viewMatrix);

	Renderer->BeginScene(1.0f, 0.0f, 0.0f, 1.0f, viewMatrix, projectionMatrix, camera->GetPosition() ,spotLight);

	Renderer->UseDefaultShader();

	std::vector<GameObject*>* toRender = currentLevel->GetGameObjects();
	for (int i = 0; i < toRender->size(); i++) {
		Renderer->Render(toRender->at(i));
	}

	for (Enemy e : enemies) {
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
