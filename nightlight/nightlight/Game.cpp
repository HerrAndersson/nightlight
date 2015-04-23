#include "Game.h"
#include <iostream>


Game::Game(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight, bool fullscreen)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	//activeGameState = MENU;

	camera = new CameraObject();

	InitManagers(hwnd, fullscreen);
	LoadAssets();
//	OutputDebugString("test\n");
}

void Game::InitManagers(HWND hwnd, bool fullscreen)
{
	Logic = new GameLogic();
	Renderer = new RenderModule(hwnd, screenWidth, screenHeight, fullscreen);
	Input = new InputManager(hwnd, screenWidth, screenHeight);
	Assets = new AssetManager(Renderer->GetDevice());
}

void Game::LoadAssets()
{
	gameObject = new GameObject(XMMatrixIdentity(), &Assets->assets.at(0));

}

Game::~Game()
{
	delete Logic;
	delete Renderer;
	delete Input;
	delete camera;
	delete Assets;
	delete gameObject;
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

	Input->HandleMouse();
	Input->KeyDown('w');
	//Input->KeyDown(VK_LBUTTON);
	//Input->W();

	//std::cout << "Update" << std::endl;

	result = !Input->Esc();
	return result;
}

bool Game::Render()
{
	bool result = true;

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	worldMatrix = DirectX::XMMatrixIdentity();
	camera->getProjectionMatrix(projectionMatrix);
	camera->getViewMatrix(viewMatrix);

	//std::cout << "Render" << std::endl;

	Renderer->BeginScene(Input->KeyDown('w'), Input->KeyDown('a'), Input->KeyDown('d'), 1.0f, viewMatrix, projectionMatrix);

	Renderer->UseDefaultShader();
	Renderer->Render(gameObject);

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
