#ifndef _CAMERAOBJECT_H_
#define _CAMERAOBJECT_H_

#include <DirectXMath.h>
using namespace DirectX;

class CameraObject
{

private:

	float positionX, positionY, positionZ;
	float rotationX, rotationY, rotationZ;

	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;
	XMVECTOR camUp;

public:

	CameraObject();
	~CameraObject();
	void setPosition(float x, float y, float z);
	void setRotation(float x, float y, float z);

	XMFLOAT3 getPosition();
	XMFLOAT3 getRotation();
	XMVECTOR getLookAt();
	XMVECTOR getCamUp();

	void updateCamera();
	void getViewMatrix(XMMATRIX& viewMatrix);
	void getProjectionMatrix(XMMATRIX& projectionMatrix);

	//Overloading these guarantees 16B alignment of XMMATRIX
	void* operator new(size_t i);
	void operator delete(void* p);
};


#endif