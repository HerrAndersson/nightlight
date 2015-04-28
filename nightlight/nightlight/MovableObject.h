#pragma once
#include "GameObject.h"
class MovableObject :
	public GameObject {
public:
	MovableObject ( XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 rotation );
	~MovableObject ( );
};

