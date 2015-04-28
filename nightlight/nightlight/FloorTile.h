#pragma once
#include "GameObject.h"
class FloorTile: public GameObject 
{
public:
	FloorTile ( XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 rotation );
	~FloorTile ( );
};

