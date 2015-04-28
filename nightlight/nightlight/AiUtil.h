#pragma once
#include "GameObject.h"
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Level.h"

using namespace std;
using DirectX::XMFLOAT2;
using DirectX::XMINT2;

struct Position
{
	int x, y;

	Position(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
};

XMINT2 GetPointFromCoord(int x, int y, int tileWidth)
{
	return XMINT2(floor(x / (int)tileWidth), floor(y / (int)tileWidth))
}

vector<Position*> aStar(Level* levelGrid, float startX, float startY, float endX, float endY)
{
	//Implement A* here
}