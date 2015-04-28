#include "Character.h"


Character::Character ( XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 rotation )
	: GameObject ( worldMatrix, renderObject, position, rotation ) 
{

}


Character::~Character ( ) {
}
