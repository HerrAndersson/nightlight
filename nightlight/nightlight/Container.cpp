#include "Container.h"


Container::Container(int id, XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, std::string activatesName)
	: GameObject(id, position, rotation, renderObject, coordX, coordY) {
	this->activatesName = activatesName;
}


Container::~Container ( ) {
}
