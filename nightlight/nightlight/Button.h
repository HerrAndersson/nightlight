#pragma once
#include "GameObject.h"

class Button : public GameObject
{
public:
	enum ButtonTypes {START,CONTINUE,EXIT};

	Button(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, std::string buttonType, int buttonWidth);
	~Button();

	int getButtonType()  const { return buttonType; }
	int getButtonWidth() const { return buttonWidth; }

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

	void ClickedStart();
	void ClickedContinue();
	bool ClickedEnd();

	void setClickID(int ID) { clickID = ID; }
	int getClickID()const { return clickID; }

	void setEndGame(bool end) { endgame = end; }
	bool getEndGame()const { return endgame; }

	//void setMouseClicked(bool choice){ clicked = choice; }
	//bool getMouseclicked()const { return isActivated;  }
	//
	//
	//int getCoordX() { return coordX; }
	//int getCoordY() { return coordY; }

private:
	int buttonType = -1;
	bool isActivated = false;
	Button* activatesStartButton = nullptr;
	Button* activatesContinueButton = nullptr;
	Button* activatesExitButton = nullptr;
	Button* mouseclickedd = nullptr;
	int clickID, buttonWidth;
	bool endgame;
};