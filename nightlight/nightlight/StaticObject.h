#pragma once
#include "GameObject.h"
class StaticObject :
	public GameObject {
public:
	StaticObject ( XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY );
	~StaticObject ( );
};

