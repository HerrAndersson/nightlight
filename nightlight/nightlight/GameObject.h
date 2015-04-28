#pragma once
#include "AssetUtil.h"
#include <DirectXMath.h>

using DirectX::XMMATRIX;
using namespace assetUtility;

class GameObject
{
private:

	int id;
	XMMATRIX		worldMatrix;
	XMMATRIX		scaleMatrix = XMMatrixIdentity ( );
	XMMATRIX		rotationMatrix = XMMatrixIdentity ( );
	XMMATRIX		translationMatrix = XMMatrixIdentity ( );
	XMFLOAT3		position;
	XMFLOAT3		rotation;
	XMVECTOR		forwardVector;
	RenderObject*	renderObject;

	void UpdateWorldMatrix();

public:

	GameObject(XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 rotation);
	virtual ~GameObject();

	void GetWorldMatrix(XMMATRIX& worldMatrix);
	RenderObject* GetRenderObject();

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();
	XMVECTOR GetForwardVector();
	

	void SetPosition(XMFLOAT3 pos);
	void SetRotation(XMFLOAT3 rot);

	//Overloading these guarantees 16B alignment of XMMATRIX
	void* operator new(size_t i);
	void operator delete(void* p);

};

