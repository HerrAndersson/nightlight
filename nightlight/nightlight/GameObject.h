#pragma once
#include "AssetUtil.h"
#include <DirectXMath.h>

using DirectX::XMMATRIX;

class GameObject
{
private:

	int id;
	XMMATRIX worldMatrix;
	RenderObject* renderObject;

public:

	GameObject(XMMATRIX& worldMatrix, RenderObject* renderObject);
	virtual ~GameObject();

	XMMATRIX* GetWorldMatrix();
	RenderObject* GetRenderObject();

	//Overloading these guarantees 16B alignment of XMMATRIX
	void* operator new(size_t i);
	void operator delete(void* p);

};

