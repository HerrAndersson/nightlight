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
	//worldMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z)) * XMMatrixTranslation(position.x, position.y, position.z);
	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
}

void GameObject::SetPosition(XMFLOAT3 pos)
{
	position = pos;
	translationMatrix = XMMatrixTranslation ( pos.x, pos.y, pos.z );
	UpdateWorldMatrix ( );
}

XMFLOAT3 GameObject::GetPosition()
{
	return position;
}

void GameObject::SetRotation(XMFLOAT3 rot)
{
	rotation = rot;
	rotationMatrix = XMMatrixRotationRollPitchYaw ( XMConvertToRadians ( -rotation.x ), XMConvertToRadians ( -rotation.y ), XMConvertToRadians ( -rotation.z ) );
	UpdateWorldMatrix ( );
}

XMVECTOR GameObject::GetForwardVector()
{
	forwardVector = XMVector3TransformCoord ( forwardVector, worldMatrix );
	XMFLOAT4X4 rotationMatrixF;
	XMStoreFloat4x4 ( &rotationMatrixF, rotationMatrix );
	forwardVector = XMVectorSet ( rotationMatrixF._11, rotationMatrixF._12, rotationMatrixF._13, rotationMatrixF._14 );

	/*forwardVector = XMVector3TransformCoord(forwardVector, worldMatrix);
	XMVector3Normalize(forwardVector);

	XMVECTOR pos = XMVectorSet(position.x, position.y, position.z, 0);
	forwardVector = pos + forwardVector;
	return forwardVector;
	forwardVector = pos + forwardVector;*/

	return XMVector3Normalize ( forwardVector );
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
