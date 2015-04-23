#include "GameObject.h"


GameObject::GameObject(XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 rotation)
{
	this->worldMatrix = worldMatrix;
	this->renderObject = renderObject;
	this->position = position;
	this->rotation = rotation;
}


GameObject::~GameObject()
{
	//Should not delete RenderObject, handled from AssetManager
}

XMMATRIX* GameObject::GetWorldMatrix()
{
	return &worldMatrix;
}
RenderObject* GameObject::GetRenderObject()
{
	return renderObject;
}

void GameObject::UpdateWorldMatrix()
{
	worldMatrix = XMMatrixTranslation(position.x, position.y, position.z);
}

void GameObject::SetPosition(XMFLOAT3 pos)
{
	position = pos;
	UpdateWorldMatrix();
}

XMFLOAT3 GameObject::GetPosition()
{
	return position;
}

void* GameObject::operator new(size_t i)
{
	return _mm_malloc(i, 16);
}

void GameObject::operator delete(void* p)
{
	_mm_free(p);
}
