#pragma once
#include "GameObject.h"
class Door : public GameObject 
{

private:
	int doorStatus;
	int doorType;
public:

	Door(XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 rotation, int doorStatus, int doorType);
	~Door();

};

