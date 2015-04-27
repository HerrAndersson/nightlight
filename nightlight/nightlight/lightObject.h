#ifndef _LIGHTCLASS_H_
#define _LIGHTCLASS_H_

#include <DirectXMath.h>
using namespace DirectX;

class LightObject
{

private:

	XMMATRIX viewMatrix;
	XMMATRIX orthoMatrix;

	XMFLOAT4 ambientColor;
	XMFLOAT4 diffuseColor;
	XMFLOAT3 position;
	XMFLOAT3 direction;
	XMFLOAT3 lookAt;

public:

	LightObject();
	~LightObject();

	void SetAmbientColor(float r, float g, float b, float a);
	void SetDiffuseColor(float r, float g, float b, float a);
	void SetDirection(float x, float y, float z);
	void SetPosition(float x, float y, float z);

	XMFLOAT4 GetAmbientColor();
	XMFLOAT4 GetDiffuseColor();
	XMFLOAT3 GetDirection();
	XMFLOAT3 GetPosition();

	//Different types of view matrices; a regular perspective and an orthographic one.
	//These could in a later case be applied to create shadow maps.

	void GenerateViewMatrix();
	void GetViewMatrix(XMMATRIX& viewMatrix);

	void GenerateOrthoMatrix(float width, float screenDepth, float screenNear);
	void GetOrthoMatrix(XMMATRIX& orthoMatrix);

	//Overloading these guarantees 16B alignment of XMMATRIX
	void* operator new(size_t i);
	void operator delete(void* p);

};

#endif