#include "Lever.h"


Lever::Lever(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, bool isPowered, bool isActivated, std::string activationName, std::string activatesName)
	: GameObject ( position, rotation,  renderObject, coordX, coordY ) {
	this->isPowered = isPowered;
	this->isActivated = isActivated;
	this->activationName = activationName;
	this->activatesName = activatesName;
}


Lever::~Lever ( ) {
}
