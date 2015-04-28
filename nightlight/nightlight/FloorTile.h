#pragma once
#include "GameObject.h"
class FloorTile: public GameObject 
{

private:



public:
	FloorTile(XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 rotation);
	~FloorTile();
};

