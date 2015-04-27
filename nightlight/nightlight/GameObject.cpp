#include "GameObject.h"


GameObject::GameObject(XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 rotation)
{
	this->worldMatrix = worldMatrix;
	this->renderObject = renderObject;
	this->position = position;
	this->rotation = rotation;
	forwardVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
}


GameObject::~GameObject()
{
	//Should not delete RenderObject, handled from AssetManager
}

void GameObject::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = this->worldMatrix;
}
RenderObject* GameObject::GetRenderObject()
{
	return renderObject;
}

void GameObject::UpdateWorldMatrix()
{
	worldMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z)) * XMMatrixTranslation(position.x, position.y, position.z);
	

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

void GameObject::SetRotation(XMFLOAT3 rot)
{
	rotation = rot;
	UpdateWorldMatrix();
}

XMVECTOR GameObject::getForwardVector()
{
	forwardVector = XMVector3TransformCoord(forwardVector, worldMatrix);
	XMVector3Normalize(forwardVector);

	XMVECTOR pos = XMVectorSet(position.x, position.y, position.z, 0);
	forwardVector = pos + forwardVector;
	return forwardVector;
}

XMFLOAT3 GameObject::GetRotation()
{
	return rotation;
}

void* GameObject::operator new(size_t i)
{
	return _mm_malloc(i, 16);
}

void GameObject::operator delete(void* p)
{
	_mm_free(p);
}
