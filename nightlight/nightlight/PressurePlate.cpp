#include "PressurePlate.h"


PressurePlate::PressurePlate(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, std::string activatesName)
	: GameObject(position, rotation, renderObject, coordX, coordY)
{
	this->activatesName = activatesName;
}


PressurePlate::~PressurePlate()
{
}
