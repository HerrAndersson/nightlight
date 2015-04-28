#include "MovableObject.h"


MovableObject::MovableObject ( XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 rotation )
	: GameObject ( worldMatrix, renderObject, position, rotation ) {

}


MovableObject::~MovableObject ( ) {
}
