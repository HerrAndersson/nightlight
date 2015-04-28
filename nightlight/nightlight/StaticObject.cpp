#include "StaticObject.h"


StaticObject::StaticObject ( XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 rotation )
	: GameObject ( worldMatrix, renderObject, position, rotation ) {

}


StaticObject::~StaticObject ( ) {
}
