#pragma once
#include "GameObject.h"
class PressurePlate :
	public GameObject {
public:
	PressurePlate(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, int plateStatus);
	~PressurePlate ( );
private:
	int plateStatus;
};

