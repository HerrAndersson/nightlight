#include "CameraObject.h"

CameraObject::CameraObject()
{
	positionX = 0.0f;
	positionY = 20.0f;
	positionZ = 0.0f;

	rotationX = 0.0f;
	rotationY = 0.0f;
	rotationZ = 0.0f;

	//Ugly aspect ratio fix (2/1.33333)
	projectionMatrix = XMMatrixPerspectiveFovLH(XM_PI / 3,float(2 / 1.33333), 0.1f, 1000);
	updateCamera();
}

CameraObject::~CameraObject()
{}


void CameraObject::setPosition(float x, float y, float z)
{
	positionX = x;
	positionY = y;
	positionZ = z;
	return;
}

void CameraObject::setRotation(float x, float y, float z)
{
	rotationX = x;
	rotationY = y; 
	rotationZ = z;
	return;
}

XMFLOAT3 CameraObject::getPosition()
{
	return XMFLOAT3(positionX, positionY, positionZ);
}


XMFLOAT3 CameraObject::getRotation()
{
	return XMFLOAT3(rotationX, rotationY, rotationZ);
}

void CameraObject::updateCamera()
{
	XMVECTOR up, position, lookAt;
	XMMATRIX rotationMatrix;

	//Set the default up vector.

	up = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);

	//Set the default world position.
	position = XMVectorSet(positionX, positionY, positionZ, 1.0f);

	//Set where the camera is looking by default.
	lookAt = XMVectorSet(0.0f, 0.0f, -1.0f, 1.0f);

	//Create the rotation matrix from the above values.
	rotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(rotationX), XMConvertToRadians(rotationY), XMConvertToRadians(rotationZ));

	//Transfoorm the lookAt and upp vector by the rotation matrix.
	lookAt = XMVector3TransformCoord(lookAt, rotationMatrix);
	up = XMVector3TransformCoord(up, rotationMatrix);

	camUp = up;

	//Create the view matrix from the updated vectors.
	viewMatrix = XMMatrixLookAtLH(position, lookAt, up);

	return;
}


XMVECTOR CameraObject::getCamUp()
{

	return camUp;
}

void CameraObject::getProjectionMatrix(XMMATRIX& projectionMatrixIn)
{
	projectionMatrixIn = projectionMatrix;

}

void CameraObject::getViewMatrix(XMMATRIX& viewMatrixIn)
{
	viewMatrixIn = viewMatrix;

}

void* CameraObject::operator new(size_t i)
{
	return _mm_malloc(i, 16);
}

void CameraObject::operator delete(void* p)
{
	_mm_free(p);
}
