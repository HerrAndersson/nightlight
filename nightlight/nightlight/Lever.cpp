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

void Lever::ActivateLever() {
	if (isPowered) {
		isActivated = true;

		XMFLOAT3 rot = this->GetRotationDeg();
		Weights = { 0, 1, 0, 0 };
		this->SetRotationDeg(rot);

		if (activatesLever != nullptr) {
			activatesLever->setIsPowered(true);
		}
		if (activatesDoor != nullptr) {
			activatesDoor->setIsOpen(true);
		}
	}
}

void Lever::DeactivateLever() {
	if (isPowered) {
		isActivated = false;

		XMFLOAT3 rot = this->GetRotationDeg();
		Weights = { 1, 0, 0, 0 };
		this->SetRotationDeg(rot);

		if (activatesLever != nullptr) {
			activatesLever->DeactivateLever();
			activatesLever->setIsPowered(false);
		}
		if (activatesDoor != nullptr) {
			activatesDoor->setIsOpen(false);
		}
	}
}
