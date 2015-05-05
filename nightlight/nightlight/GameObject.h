#pragma once
#include "AssetUtil.h"
#include <DirectXMath.h>

using DirectX::XMMATRIX;
using namespace assetUtility;

class GameObject
{
private:

	int id;
	int tileCoordX, tileCoordY;
	XMFLOAT3		position;
	XMFLOAT3		rotation;
	XMVECTOR		forwardVector;
	RenderObject*	renderObject;

public:

	GameObject(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY);
	virtual ~GameObject();

	XMMATRIX GetWorldMatrix();
	RenderObject* GetRenderObject();

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();
	int GetTileXCoord(){ return tileCoordX; };
	int GetTileYCoord(){ return tileCoordY; };
	XMVECTOR GetForwardVector();
	ID3D11ShaderResourceView* GetDiffuseTexture();
	ID3D11ShaderResourceView* GetSpecularTexture();

	void SetPosition(XMFLOAT3 pos);
	void SetRotation(XMFLOAT3 rot);

	//Overloading these guarantees 16B alignment of XMMATRIX
	void* operator new(size_t i);
	void operator delete(void* p);

};

