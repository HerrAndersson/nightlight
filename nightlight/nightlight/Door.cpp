#include "Door.h"

Door::Door(XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 rotation, int doorStatus, int doorType)
	: GameObject(worldMatrix, renderObject, position, rotation)
{
	this->doorStatus = doorStatus;
	this->doorType = doorType;
}


Door::~Door()
{

}
