#pragma once
#include "AssetUtil.h"
#include <DirectXMath.h>

#define TILE_SIZE 1.0f

using DirectX::XMMATRIX;
using namespace assetUtility;

struct Coord {
	Coord() {
		x = -1;
		y = -1;
	}
	Coord(int x, int y) {
		this->x = x;
		this->y = y;
	}
	bool operator==(const Coord& other) {
		if (x == other.x && y == other.y)
			return true;
		return false;
	}
	int x;
	int y;
};

class GameObject
{
private:

	int id;
	Coord tileCoord;
	XMFLOAT3		position;
	XMFLOAT3		rotation;
	XMVECTOR		forwardVector;
	RenderObject*	renderObject;
	bool			isSelected	= false;

public:

	GameObject(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY);
	virtual ~GameObject();

	XMMATRIX GetWorldMatrix();
	RenderObject* GetRenderObject();

	bool IsSelected();
	void SetSelected(bool selected);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotationDeg();
	XMFLOAT3 GetRotationRad();
	Coord GetTileCoord() { return tileCoord; };
	XMVECTOR GetForwardVector();
	ID3D11ShaderResourceView* GetDiffuseTexture();
	ID3D11ShaderResourceView* GetSpecularTexture();

	void SetPosition(XMFLOAT3 pos);
	void SetTilePosition(Coord coord);
	void SetRotationDeg(XMFLOAT3 rot);

	//Overloading these guarantees 16B alignment of XMMATRIX
	void* operator new(size_t i);
	void operator delete(void* p);

};

