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
	int x, z;

	Position(int x, int z)
	{
		this->x = x;
		this->z = z;
	}
};

Position GetPositionFromCoord(int x, int z, int tileWidth)
{
	return Position(floor(x / (int)tileWidth), floor(z / (int)tileWidth))
}

vector<Position*> aStar(Level* levelGrid, int tileWidth, XMFLOAT2 startPosXZ, XMFLOAT2 endPosXZ)
{
	//Implement A* here
	//XMFLOAT2 holds x-position at .x and z-position at pos.y

	Position start = GetPositionFromCoord(startPosXZ.x, startPosXZ.y, tileWidth);
	Position end = GetPositionFromCoord(endPosXZ.x, endPosXZ.y, tileWidth);
}