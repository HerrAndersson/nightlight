#pragma once
#include "GameObject.h"
class Lever :
	public GameObject {
public:
	Lever(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, int leverStatus);
	~Lever ( );
private:
	int leverStatus;
};

