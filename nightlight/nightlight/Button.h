#pragma once
#include "GameObject.h"

class Button : public GameObject
{
public:
	Button(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, std::string activatesName);
	~Button();

	std::string getActivatesName()  const { return activatesName; }

	void ActivateButton();
	void DeactivateButton();


	void ActivateStartButton();
	void DeactivateStartButton();

	void setIsStartActivated(bool choice){ isActivated = choice; }
	bool getIsStartActivated()const { return isActivated; }

	void ActivateContinueButton();
	void DeactivateContinueButton();

	void setIsContinueActivated(bool choice){ isActivated = choice; }
	bool getIsContinueActivated()const { return isActivated; }

	void ActivateExitButton();
	void DeactivateExitButton();

	void setIsExitActivated(bool choice){ isActivated = choice; }
	bool getIsExitActivated()const { return isActivated; }


	int getCoordX() { return coordX; }
	int getCoordY() { return coordY; }

private:
	std::string activatesName = "";
	bool isActivated = false;
	Button* activatesStartButton = nullptr;
	Button* activatesContinueButton = nullptr;
	Button* activatesExitButton = nullptr;
	int coordX, coordY;
};