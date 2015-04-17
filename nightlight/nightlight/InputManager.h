#pragma once
#include <Windows.h>
#include <DirectXMath.h>
using namespace DirectX;
class InputManager
{
private:

	HWND hwnd;
	int screenWidth, screenHeight;

	XMFLOAT2 pos;

public:

	InputManager(HWND hwnd, int screenWidth, int screenHeight);
	~InputManager();

	void HandleMouse();

	bool KeyDown(char key);
	bool Esc();
	bool LeftMouse();
	bool RightMouse();
};

