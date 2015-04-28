#include "Wall.h"



Wall::Wall ( XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 rotation )
	: GameObject ( worldMatrix, renderObject, position, rotation ) 
{

}


Wall::~Wall ( )
{

}
