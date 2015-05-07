#pragma once
#include "GameObject.h"
#include "Lever.h"
#include "Door.h"

class PressurePlate :
	public GameObject {
public:
	PressurePlate(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, std::string activatesName);
	~PressurePlate ( );

	std::string getActivatesName() const { return activatesName; }

	Lever* getActivatesLever() const { return activatesLever; }
	void setActivatesLever(Lever* val) { activatesLever = val; }
	Door* getActivatesDoor() const { return activatesDoor; }
	void setActivatesDoor(Door* val) { activatesDoor = val; }

private:
	std::string activatesName = "";
	Lever* activatesLever = nullptr;
	Door* activatesDoor = nullptr;
};

