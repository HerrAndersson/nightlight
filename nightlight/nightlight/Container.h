#pragma once
#include "GameObject.h"
class Container :
	public GameObject {
public:
	Container ( XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 rotation );
	~Container ( );
};

