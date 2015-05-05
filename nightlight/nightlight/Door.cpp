#include "Door.h"

Door::Door(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, int doorStatus, int doorType)
	: GameObject(position, rotation,  renderObject, coordX, coordY)
{
	this->doorStatus = doorStatus;
	this->doorType = doorType;
}


Door::~Door()
{

}
