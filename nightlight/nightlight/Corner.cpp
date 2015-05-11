#include "Corner.h"


Corner::Corner(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY)
	: GameObject(position, rotation, renderObject, coordX, coordY)
{

}

Corner::~Corner()
{
}
