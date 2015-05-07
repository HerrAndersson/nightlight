#include "GameObject.h"

GameObject::GameObject(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY)
{
	this->position = position;
	this->rotation.y = XMConvertToDegrees(rotation);
	this->renderObject = renderObject;
	this->tileCoordX = coordX;
	this->tileCoordY = coordY;
	forwardVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
}

GameObject::~GameObject()
{
	//Should not delete RenderObject, handled from AssetManager
}

XMMATRIX GameObject::GetWorldMatrix()
{
	return XMMatrixRotationRollPitchYaw(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z))*XMMatrixTranslation(position.x, position.y, position.z);
}
RenderObject* GameObject::GetRenderObject()
{
	return renderObject;
}

void GameObject::SetPosition(XMFLOAT3 pos)
{
	position = pos;
}

XMFLOAT3 GameObject::GetPosition()
{
	return position;
}

void GameObject::SetRotation(XMFLOAT3 rot)
{
	rotation = rot;
}

XMFLOAT3 GameObject::GetRotation()
{
	return rotation;
}

XMVECTOR GameObject::GetForwardVector()
{
	XMMATRIX rotmat = XMMatrixRotationRollPitchYaw(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z));
	XMFLOAT4X4 rotationMatrixF;
	XMStoreFloat4x4(&rotationMatrixF, rotmat);
	forwardVector = -XMVectorSet(rotationMatrixF._11, rotationMatrixF._12, rotationMatrixF._13, rotationMatrixF._14);
	return XMVector3Normalize ( forwardVector );
}

ID3D11ShaderResourceView* GameObject::GetDiffuseTexture()
{
	return renderObject->diffuseTexture;
}

ID3D11ShaderResourceView* GameObject::GetSpecularTexture()
{
	return renderObject->specularTexture;
}

void* GameObject::operator new(size_t i)
{
	return _mm_malloc(i, 16);
}

void GameObject::operator delete(void* p)
{
	_mm_free(p);
}
