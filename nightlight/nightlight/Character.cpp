#include "Character.h"


Character::Character(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY)
	: GameObject(position, rotation, renderObject, coordX, coordY)
{

}

Character::~Character ( ) 
{
}


void Character::UpdateCharacterAnimation(int moving)
{
	if(moving)
	{
		frame += 0.1f;
		float framelength = 1;
		if (frame > (framelength * 2))
			frame = 0;

		if (frame < (framelength * 2))
		{
			characterWeights.x = 0;
			characterWeights.y = 0;
			characterWeights.z = (-frame + (framelength * 2)) / framelength;
			characterWeights.w = (frame - (framelength)) / framelength;
		}
		if (frame < (framelength))
		{
			characterWeights.x = 0;
			characterWeights.y = (-frame + (framelength)) / framelength;
			characterWeights.z = frame / framelength;
			characterWeights.w = 0;
		}
	}
	else{
		frame = 0;
		characterWeights = { 1, 0, 0, 0 };
	}
}