#include "PressurePlate.h"


PressurePlate::PressurePlate(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, std::string activatesName)
	: GameObject(position, rotation, renderObject, coordX, coordY)
{
	this->activatesName = activatesName;
}


PressurePlate::~PressurePlate()
{
}

void PressurePlate::ActivatePressurePlate() {
	isActivated = true;
	colorModifier = XMFLOAT3(0.1, 0.1, 0.2);

	if (activatesLever != nullptr) {
		activatesLever->setIsPowered(true);
	}
	if (activatesDoor != nullptr) {
		activatesDoor->setIsOpen(true);
	}
}

void PressurePlate::DeactivatePressurePlate() {
	isActivated = false;
	colorModifier = XMFLOAT3(0, 0, 0);
	
	if (activatesLever != nullptr) {
		activatesLever->DeactivateLever();
		activatesLever->setIsPowered(false);
	}
	if (activatesDoor != nullptr) {
		activatesDoor->setIsOpen(false);
	}
	
}
