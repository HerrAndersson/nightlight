#pragma once
#include "GameObject.h"
class Container :
	public GameObject {
public:
	Container ( XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY );
	~Container ( );
};

