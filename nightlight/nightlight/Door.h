#pragma once
#include "GameObject.h"
class Door : public GameObject 
{

private:
	bool isOpen;
	int doorType;
	std::string activationName;

public:

	enum DoorTypes { NORMAL_DOOR, START_DOOR, END_DOOR };

	Door(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, bool isOpen, int doorType, std::string activationName);
	~Door();

	int getDoorType() const { return doorType; }
	bool getIsOpen() const { return isOpen; }
	void setIsOpen(bool val);
	std::string getActivationName() const { return activationName; }
};

