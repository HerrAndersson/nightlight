#include "Container.h"


Container::Container(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, std::string activatesName)
	: GameObject ( position, rotation,  renderObject, coordX, coordY ) {
	this->activatesName = activatesName;
}


Container::~Container ( ) {
}
