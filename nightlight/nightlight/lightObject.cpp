#include "LightObject.h"


LightObject::LightObject()
{
}


LightObject::~LightObject()
{
}

//Setters
void LightObject::SetAmbientColor(float red, float green, float blue, float alpha)
{
	ambientColor = XMFLOAT4(red, green, blue, alpha);
}


void LightObject::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	diffuseColor = XMFLOAT4(red, green, blue, alpha);
}


void LightObject::SetDirection(float x, float y, float z)
{
	direction = XMFLOAT3(x, y, z);
}

void LightObject::SetPosition(float x, float y, float z)
{
	position = XMFLOAT3(x, y, z);
}


//Getters
XMFLOAT4 LightObject::GetAmbientColor()
{
	return ambientColor;
}


XMFLOAT4 LightObject::GetDiffuseColor()
{
	return diffuseColor;
}


XMFLOAT3 LightObject::GetDirection()
{
	
	return direction;
}


XMFLOAT3 LightObject::GetPosition()
{
	return position;
}


void LightObject::GenerateViewMatrix()
{
	//setting up a perspective-based view matrix
	XMFLOAT3 up;

	// sets up our up-vector
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&position);
	DirectX::XMVECTOR upVec = DirectX::XMLoadFloat3(&up);
	DirectX::XMVECTOR lookVec = DirectX::XMLoadFloat3(&lookAt);

	// Use the three loaded float3s as vectors for the view matrix
	viewMatrix = XMMatrixLookAtLH(posVec, lookVec, upVec);
}

void LightObject::GenerateOrthoMatrix(float width, float screenDepth, float screenNear)
{
	//setting up a orthographic-based view matrix
	orthoMatrix = XMMatrixOrthographicLH(width, width, screenNear, screenDepth);
}

void LightObject::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = viewMatrix;
}

void LightObject::GetOrthoMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = orthoMatrix;
}

void* LightObject::operator new(size_t i)
{
	return _mm_malloc(i, 16);
}

void LightObject::operator delete(void* p)
{
	_mm_free(p);
}
