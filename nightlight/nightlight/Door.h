#pragma once
#include "GameObject.h"
class Door : public GameObject {
public:
	Door ( XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 rotation );
	~Door ( );
};

