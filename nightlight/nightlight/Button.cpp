#include "Button.h"

Button::Button(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, std::string buttonType, int buttonWidth)
	: GameObject(position, rotation, renderObject, coordX, coordY)
{
	if (buttonType == "start"){
		this->buttonType = ButtonTypes::START;
	}
	else if (buttonType == "continue"){
		this->buttonType = ButtonTypes::CONTINUE;
	}
	else if(buttonType == "end"){
		this->buttonType = ButtonTypes::EXIT;
	}
	this->buttonWidth = buttonWidth;
}

Button::~Button()
{
}