#include "StaticObject.h"


StaticObject::StaticObject ( XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY )
	: GameObject ( position, rotation, renderObject, coordX, coordY ) {

}


StaticObject::~StaticObject ( ) {
}
