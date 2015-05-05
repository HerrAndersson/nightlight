#include "Container.h"


Container::Container ( XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY )
	: GameObject ( position, rotation,  renderObject, coordX, coordY ) {

}


Container::~Container ( ) {
}
