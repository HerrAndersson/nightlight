#include "Door.h"

Door::Door(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, bool isOpen, int doorType, std::string activationName)
	: GameObject(position, rotation,  renderObject, coordX, coordY)
{
	this->isOpen = isOpen;
	this->doorType = doorType;
	this->activationName = activationName;
}


Door::~Door()
{

}
