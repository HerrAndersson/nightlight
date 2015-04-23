#pragma once
#include "AssetUtil.h"
#include <DirectXMath.h>

using DirectX::XMMATRIX;

class GameObject
{
private:

	int id;
	XMMATRIX		worldMatrix;
	XMFLOAT3		position;
	XMFLOAT3		rotation;
	RenderObject*	renderObject;

	void UpdateWorldMatrix();

public:

	GameObject(XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 rotation);
	virtual ~GameObject();

	XMMATRIX* GetWorldMatrix();
	RenderObject* GetRenderObject();

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void SetPosition(XMFLOAT3 pos);
	void SetRotation(XMFLOAT3 rot);

	//Overloading these guarantees 16B alignment of XMMATRIX
	void* operator new(size_t i);
	void operator delete(void* p);

};

