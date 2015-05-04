#include "Lever.h"


Lever::Lever(XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 rotation, int leverStatus)
	: GameObject ( worldMatrix, renderObject, position, rotation ) {

}


Lever::~Lever ( ) {
}
