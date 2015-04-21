#ifndef _CAMERAOBJECT_H_
#define _CAMERAOBJECT_H_

#include <DirectXMath.h>
using namespace DirectX;

class CameraObject
{
public:
	CameraObject();
	~CameraObject();
	void setPosition(float, float, float);
	void setRotation(float, float, float);

	XMFLOAT3 getPosition();
	XMFLOAT3 getRotation();
	XMVECTOR getLookAt();
	XMVECTOR getCamUp();

	void updateCamera();
	void getViewMatrix(XMMATRIX&);
	void getProjectionMatrix(XMMATRIX&);

	//Overloading these guarantees 16B alignment of XMMATRIX
	void* operator new(size_t i);
	void operator delete(void* p);

private:

	float positionX, positionY, positionZ;
	float rotationX, rotationY, rotationZ;

	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;
	XMVECTOR camUp;



};


#endif