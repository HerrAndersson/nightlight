#include "InputManager.h"
#include <iostream>

InputManager::InputManager(HWND hwnd, int screenWidth, int screenHeight)
{
	this->hwnd = hwnd;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
}

InputManager::~InputManager()
{

}

void InputManager::HandleMouse()
{
	LPPOINT point = new POINT();
	GetCursorPos(point);

	ScreenToClient(hwnd, point);

	pos.x = (float)point->x;
	pos.y = (float)point->y;

	if (pos.x < 0)  { pos.x = 0; }
	if (pos.y < 0)  { pos.y = 0; }
	if (pos.x  > screenWidth)  { pos.x = (float)screenWidth; }
	if (pos.y  > screenHeight) { pos.y = (float)screenHeight; }

	//std::cout << "Mouse: " << pos.x << " " << pos.y << std::endl;

	delete point;
}

bool InputManager::KeyDown(char key)
{
	int keyToCheck = -1;

	switch (key)
	{
		case 'w':
		{
			keyToCheck = 0x57;
			break;
		}
		case 'a':
		{
			keyToCheck = 0x41;
			break;
		}
		case 's':
		{
			keyToCheck = 0x53;
			break;
		}
		case 'd':
		{
			keyToCheck = 0x44;
			break;
		}
		default:
			break;
	}

	if (keyToCheck != -1)
		return GetAsyncKeyState(keyToCheck);
	else
		return false;
}
bool InputManager::Esc()
{
	return GetAsyncKeyState(VK_ESCAPE);
}
bool InputManager::LeftMouse()
{
	return GetAsyncKeyState(VK_LBUTTON);
}
bool InputManager::RightMouse()
{
	return GetAsyncKeyState(VK_RBUTTON);
}

//bool InputManager::W()
//{
//	return KeyDown(0x57);
//}
