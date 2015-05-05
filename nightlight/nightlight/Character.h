#pragma once
#include "GameObject.h"
class Character :
	public GameObject {
public:
	Character ( XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY );
	~Character ( );

	float getCharacterRadius() const { return characterRadius; }

private:
	float characterRadius = 1.0f;
	
};

