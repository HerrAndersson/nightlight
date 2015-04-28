#pragma once
#include "GameObject.h"
class Wall : public GameObject {
public:
	Wall ( XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 rotation );
	~Wall ( );
};

