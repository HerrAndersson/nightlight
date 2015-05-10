#pragma once
#include "GameObject.h"
class Character : public GameObject 
{
private:

	float radius = 0.5f;

public:

	Character ( XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY );
	~Character ( );

	float getRadius() const { return radius; }
};

