#pragma once
#include "GameObject.h"
class Corner :
	public GameObject {
public:
	Corner ( XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 rotation );
	~Corner ( );
};

