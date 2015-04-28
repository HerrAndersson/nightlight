#pragma once
#include "GameObject.h"
class Character :
	public GameObject {
public:
	Character ( XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 rotation );
	~Character ( );
};

