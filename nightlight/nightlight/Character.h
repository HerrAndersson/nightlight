#pragma once
#include "GameObject.h"
class Character :
	public GameObject {
public:
	Character ( XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY );
	~Character ( );
};

