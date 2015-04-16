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

bool InputManager::Key(int vKey)
{
	bool result = false;

	if (GetAsyncKeyState(vKey))
		result = true;

	//std::cout << "Key: " << vKey << " " << result << std::endl;

	return result;
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
		return Key(keyToCheck);
	else
		return false;
}
bool InputManager::Esc()
{
	return Key(VK_ESCAPE);
}
bool InputManager::LeftMouse()
{
	return Key(VK_LBUTTON);
}
bool InputManager::RightMouse()
{
	return Key(VK_RBUTTON);
}

//bool InputManager::W()
//{
//	return KeyDown(0x57);
//}
