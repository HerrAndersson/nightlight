#pragma once
#include "GameObject.h"
class Floor: public GameObject 
{

private:



public:
	Floor(XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 rotation);
	~Floor();
};

