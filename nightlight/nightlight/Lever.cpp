#include "Lever.h"


Lever::Lever(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, int leverStatus)
	: GameObject ( position, rotation,  renderObject, coordX, coordY ) {

}


Lever::~Lever ( ) {
}
