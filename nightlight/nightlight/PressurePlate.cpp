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

	XMFLOAT3 rot = this->GetRotationDeg();
	rot.y += 45.0f;
	this->SetRotationDeg(rot);

	if (activatesLever != nullptr) {
		activatesLever->setIsPowered(true);
	}
	if (activatesDoor != nullptr) {
		activatesDoor->setIsOpen(true);
	}
}

void PressurePlate::DeactivatePressurePlate() {
	isActivated = false;

	XMFLOAT3 rot = this->GetRotationDeg();
	rot.y -= 45.0f;
	this->SetRotationDeg(rot);

	if (activatesLever != nullptr) {
		activatesLever->DeactivateLever();
		activatesLever->setIsPowered(false);
	}
	if (activatesDoor != nullptr) {
		activatesDoor->setIsOpen(false);
	}
	
}
