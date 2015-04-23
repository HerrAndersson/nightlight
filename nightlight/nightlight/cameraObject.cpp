#include "cameraObject.h"

CameraObject::CameraObject()
{
	positionX = 0.0f;
	positionY = 10.0f;
	positionZ = 0.0f;

	rotationX = 0.0f;
	rotationY = 0.0f;
	rotationZ = 0.0f;

	//Ugly aspect ratio fix (2/1.33333)
	projectionMatrix = XMMatrixPerspectiveFovLH(XM_PI / 2, 2 / 1.333333, 0.1f, 1000);
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
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	//Set the default up vector.

	up = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);

	//up.m128_f32[0] = 0.0f;
	//up.m128_f32[1] = 1.0f;
	//up.m128_f32[2] = 0.0f;
	//up.m128_f32[3] = 1.0f;

	//Set the default world position.
	position = XMVectorSet(positionX, positionY, positionZ, 1.0f);
	//position.m128_f32[0] = positionX;
	//position.m128_f32[1] = positionY;
	//position.m128_f32[2] = positionZ;
	//position.m128_f32[3] = 1.0f;

	//Set where the camera is looking by default.
	lookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	//lookAt.m128_f32[0] = 0.0f;
	//lookAt.m128_f32[1] = 0.0f;
	//lookAt.m128_f32[2] = 1.0f;
	//lookAt.m128_f32[3] = 1.0f;

	//Set the yaw, pitch, and roll in radians´.
	pitch = rotationX * 0.0174532925f;
	yaw = rotationY * 0.0174532925f;
	roll = rotationZ * 0.0174532925f;

	//Create the rotation matrix from the above values.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

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
