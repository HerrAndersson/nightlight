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
	currentLevel->updateGameObjets();
}

Game::~Game()
{
	delete Logic;
	delete Renderer;
	delete camera;
	delete Assets;
	delete AI;
	delete Levels;


	delete spotLight;
	delete character;

	//for (auto e : enemies) delete e;
	//enemies.clear();
/*

	for (auto g : gameObject) delete g;
	gameObject.clear();*/
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

	std::vector<GameObject*> toRender = currentLevel->GetGameObjects();
	for (int i = 0; i < toRender.size(); i++){
		Renderer->Render(toRender[i]);
	}
	Renderer->Render(character);
/*
	Renderer->Render(gameObject.at(0));
	Renderer->Render(gameObject.at(1));
	for (int j = 0; j < 20; j++)
	{
		for (int i = 0; i < 20; i++)
		{
			gameObject.at(2)->SetPosition(XMFLOAT3(-15 + i, 0, -15 + j));
			Renderer->Render(gameObject.at(2));

			if (i == 0)
			{
				gameObject.at(3)->SetPosition(XMFLOAT3(-15 + i, 0, -15 + j));

				Renderer->Render(gameObject.at(3));
			}

			if (j == 0)
			{
				gameObject.at(3)->SetPosition(XMFLOAT3(-15 + i, 0, -15 + j));
				gameObject.at(3)->SetRotation(XMFLOAT3(0, 90, 0));
				Renderer->Render(gameObject.at(3));
				gameObject.at(3)->SetRotation(XMFLOAT3(0, 0, 0));
			}

			if (j == 19)
			{
				if (!(i >10 && i < 14))
					gameObject.at(3)->SetPosition(XMFLOAT3(-15 + i, 0, -15 + j));

				gameObject.at(3)->SetRotation(XMFLOAT3(0, 270, 0));
				Renderer->Render(gameObject.at(3));
				gameObject.at(3)->SetRotation(XMFLOAT3(0, 0, 0));
			}

			if (i == 19)
			{
				gameObject.at(3)->SetPosition(XMFLOAT3(-15 + i, 0, -15 + j));

				gameObject.at(3)->SetRotation(XMFLOAT3(0, 180, 0));
				Renderer->Render(gameObject.at(3));
				gameObject.at(3)->SetRotation(XMFLOAT3(0, 0, 0));
			}

			if (i == 19 && j == 5)
			{
				gameObject.at(4)->SetPosition(XMFLOAT3(-15 + i, 0, -15 + j));
				gameObject.at(4)->SetRotation(XMFLOAT3(0, 180, 0));
				Renderer->Render(gameObject.at(4));
				gameObject.at(4)->SetRotation(XMFLOAT3(0, 0, 0));
			}
		}
	}
*/
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
