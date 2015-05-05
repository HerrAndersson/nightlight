#pragma once
#include "GameObject.h"
class Wall : public GameObject 
{

private:



public:
	Wall ( XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY );
	~Wall ( );
};

