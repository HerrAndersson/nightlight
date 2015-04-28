#pragma once
#include "GameObject.h"
class StaticObject :
	public GameObject {
public:
	StaticObject ( XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 rotation );
	~StaticObject ( );
};

