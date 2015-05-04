#pragma once
#include "GameObject.h"
class PressurePlate :
	public GameObject {
public:
	PressurePlate(XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 rotation, int plateStatus);
	~PressurePlate ( );
private:
	int plateStatus;
};

