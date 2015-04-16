#pragma once

#include <windows.h>
#include <stdexcept>
#include "Game.h"

class System
{

private:

	LPCSTR applicationName;
	HINSTANCE hinstance;
	HWND hwnd;

	Game* game;

	int windowWidth, windowHeight;
	int screenWidth, screenHeight;

	bool fullscreen;
	bool showCursor;

	bool Update();
	void InitializeWindows();
	void ShutdownWindows();

public:

	System(bool fullscreen = false, bool showCursor = true, int screenWidth = 1000, int screenHeight = 800);
	~System();

	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static System* gameHandle = nullptr;