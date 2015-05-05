#include "MovableObject.h"


MovableObject::MovableObject ( XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY )
	: GameObject ( position,  rotation,  renderObject,  coordX,  coordY ) {

}


MovableObject::~MovableObject ( ) {
}
