#pragma once
#include "GameObject.h"
class Corner :
	public GameObject {
public:
	Corner ( XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY );
	~Corner ( );
};

