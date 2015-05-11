#include "Exit.h"


Exit::Exit(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, std::string activatesName)
	: GameObject(position, rotation, renderObject, coordX, coordY)
{
	this->activatesName = activatesName;
}


Exit::~Exit()
{
}

void Exit::ActivateExit() {
	isActivated = true;

	XMFLOAT3 rot = this->GetRotationDeg();
	rot.x += 45.0f;
	this->SetRotationDeg(rot);
}

void Exit::DeactivateExit() {
	isActivated = false;

	XMFLOAT3 rot = this->GetRotationDeg();
	rot.x -= 45.0f;
	this->SetRotationDeg(rot);
}
