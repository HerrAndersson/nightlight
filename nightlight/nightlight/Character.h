#pragma once
#include "GameObject.h"
class Character : public GameObject 
{
private:

	float radius = 0.5f;
	float speed = 0.1f;
	float dragSpeed = 0.08f;


	XMFLOAT4 characterWeights = { 1, 0, 0, 0 };
	float frame = 0;

public:

	Character ( XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY );
	~Character ( );

	float getRadius() const { return radius; }

	float GetSpeed() const { return speed; }
	XMFLOAT4 GetBlendWeights() { return characterWeights; }
	void SetSpeed(float val) { speed = val; }
	void UpdateCharacterAnimation(bool moving);
};

