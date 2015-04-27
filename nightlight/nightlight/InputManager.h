#pragma once
#include <Windows.h>
#include <DirectXMath.h>
using namespace DirectX;
class InputManager
{
private:

	HWND hwnd;
	int screenWidth, screenHeight;

	XMFLOAT2 mousePos;

public:

	InputManager(HWND hwnd, int screenWidth, int screenHeight);
	~InputManager();

	void HandleMouse();
	XMFLOAT2 GetMousePos();
	XMFLOAT2 GetMousePosScreenSpace();

	bool KeyDown(char key);
	bool Esc();
	bool LeftMouse();
	bool RightMouse();
};

