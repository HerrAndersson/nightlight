#pragma once
#include "GameObject.h"
class Character : public GameObject 
{
private:

	float radius = 0.5f;
	float speed = 0.1f;
	float dragSpeed = 0.08f;

public:

	Character ( XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY );
	~Character ( );

	float getRadius() const { return radius; }

	float GetSpeed() const { return speed; }
	void SetSpeed(float val) { speed = val; }
};

