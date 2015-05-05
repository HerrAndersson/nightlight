#pragma once
#include "GameObject.h"
class MovableObject :
	public GameObject {
public:
	MovableObject ( XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY );
	~MovableObject ( );
};

