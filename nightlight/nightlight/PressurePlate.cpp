#include "PressurePlate.h"


PressurePlate::PressurePlate(XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 rotation, int plateStatus)
	: GameObject ( worldMatrix, renderObject, position, rotation )
{
	
}


PressurePlate::~PressurePlate ( ) {
}
