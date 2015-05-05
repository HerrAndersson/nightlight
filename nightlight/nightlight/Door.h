#pragma once
#include "GameObject.h"
class Door : public GameObject 
{

private:
	int doorStatus;
	int doorType;
public:

	Door(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, int doorStatus, int doorType);
	~Door();

};

