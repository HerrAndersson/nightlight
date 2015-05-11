#pragma once
#include "GameObject.h"

class Exit :
	public GameObject {
public:

	Exit(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, std::string activatesName);
	~Exit();

	std::string getActivatesName() const { return activatesName; }

	bool getIsActivated() const { return isActivated; }
	void ActivateExit();
	void DeactivateExit();

private:
	std::string activatesName = "";
	bool isActivated = false;
};

