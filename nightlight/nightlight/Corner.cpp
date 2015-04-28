#include "Corner.h"


Corner::Corner ( XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 rotation )
	: GameObject ( worldMatrix, renderObject, position, rotation ) 
{

}


Corner::~Corner ( ) {
}
