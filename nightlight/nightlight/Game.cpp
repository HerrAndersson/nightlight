#include "Game.h"


Game::Game(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight, bool fullscreen)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	//activeGameState = MENU;

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
	asset = Assets->LoadRenderObject("Assets/Models/simple_cube.bin");

}

Game::~Game()
{
	delete Logic;
	delete Renderer;
	delete Input;
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

	Render();

	result = !Input->Esc();
	return result;
}

bool Game::Render()
{
	bool result = true;

	//D3D->BeginScene(0.0f, 0.4f, 0.0f, 1.0f);
	Renderer->BeginScene(Input->KeyDown('w'), Input->KeyDown('a'), Input->KeyDown('d'), 1.0f);

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	worldMatrix = DirectX::XMMatrixIdentity();
	
	Renderer->Render(worldMatrix, worldMatrix, worldMatrix, );
	
	//example 
	//D3D->SetCullingState(2);
	//render shadowmap
	//D3D->SetCullingState(3);
	//Renderer->Render(objects using our own culling technique)
	//D3D->SetCullingState(1);
	//Renderer->Render(normal objects)
	//D3D->DisableDepth();
	//Renderer->Render(2d stuff)

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
