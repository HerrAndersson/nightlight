#include "GameObject.h"


GameObject::GameObject(XMMATRIX& worldMatrix, RenderObject* renderObject)
{
	this->worldMatrix = worldMatrix;
	this->renderObject = renderObject;
}


GameObject::~GameObject()
{
}

void GameObject::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = this->worldMatrix;
}
RenderObject* GameObject::GetRenderObject()
{
	return renderObject;
}

void* GameObject::operator new(size_t i)
{
	return _mm_malloc(i, 16);
}

void GameObject::operator delete(void* p)
{
	_mm_free(p);
}
