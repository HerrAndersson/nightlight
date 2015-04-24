#include "Game.h"

Game::Game(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight, bool fullscreen)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	//activeGameState = MENU;

	camera = new CameraObject();

	InitManagers(hwnd, fullscreen);
	LoadAssets();
}

void Game::InitManagers(HWND hwnd, bool fullscreen)
{
	Logic = new GameLogic(hwnd, screenWidth, screenHeight);
	Renderer = new RenderModule(hwnd, screenWidth, screenHeight, fullscreen);
	Assets = new AssetManager(Renderer->GetDevice());
}

void Game::LoadAssets()
{
	gameObject.push_back(new GameObject(XMMatrixIdentity(), &Assets->assets.at(0), XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0)));
	gameObject.push_back(new GameObject(XMMatrixIdentity(), &Assets->assets.at(1), XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0)));
}

Game::~Game()
{
	delete Logic;
	delete Renderer;
	delete camera;
	delete Assets;
	//delete gameObject;
	delete asset;
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

	result = Logic->Update(gameObject.at(0), camera);

	return result;
}

bool Game::Render()
{
	bool result = true;

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	worldMatrix = DirectX::XMMatrixIdentity();
	camera->getProjectionMatrix(projectionMatrix);
	camera->getViewMatrix(viewMatrix);

	Renderer->BeginScene(0.1f, 0.1f, 0.1f, 1.0f, viewMatrix, projectionMatrix);

	Renderer->UseDefaultShader();
	Renderer->Render(gameObject.at(0));
	Renderer->Render(gameObject.at(1));
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
