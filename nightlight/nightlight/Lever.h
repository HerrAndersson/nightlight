#pragma once
#include "GameObject.h"
class Lever :
	public GameObject {
public:
	Lever ( XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 rotation );
	~Lever ( );
};

