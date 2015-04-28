#pragma once
//#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdexcept>
#include "Game.h"
#include "Timer.h"
#include "Cpu.h"

class System
{

private:

	const double MS_PER_FRAME = 16.66666667; //60FPS
	//const double MS_PER_FRAME =  8.33333333; //120FPS
	//const double MS_PER_FRAME =  6.94444444; //144FPS

	LPCSTR		applicationName;
	HINSTANCE	hinstance;
	HWND		hwnd;

	Game*		game;
	Timer*		timer;
	Cpu*        cpuUsage;

	int windowWidth, windowHeight;
	int screenWidth, screenHeight;

	bool fullscreen;
	bool showCursor;

	bool Update();
	void InitializeWindows();
	void ShutdownWindows();

public:

	System(bool fullscreen = false, bool showCursor = true, int windowWidth = 1000, int windowHeight = 800);
	~System();

	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static System* gameHandle = nullptr;