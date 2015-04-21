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

private:

	float positionX, positionY, positionZ;
	float rotationX, rotationY, rotationZ;

	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;
	XMVECTOR camUp;



};


#endif