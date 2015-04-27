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
	m_ambientColor = XMFLOAT4(red, green, blue, alpha);
	return;
}


void LightObject::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = XMFLOAT4(red, green, blue, alpha);
	return;
}


void LightObject::SetDirection(float x, float y, float z)
{
	m_direction = XMFLOAT3(x, y, z);
	return;
}

void LightObject::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT3(x, y, z);
	return;
}


//Getters

XMFLOAT4 LightObject::GetAmbientColor()
{
	return m_ambientColor;
}


XMFLOAT4 LightObject::GetDiffuseColor()
{
	return m_diffuseColor;
}


XMFLOAT3 LightObject::GetDirection()
{
	
	return m_direction;


}


XMFLOAT3 LightObject::GetPosition()
{
	return m_position;
}



void LightObject::GenerateViewMatrix()
{
	//setting up a perspective-based view matrix
	XMFLOAT3 up;

	// sets up our up-vector
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	DirectX::XMVECTOR m_posVec = DirectX::XMLoadFloat3(&m_position);
	DirectX::XMVECTOR m_upVec = DirectX::XMLoadFloat3(&up);
	DirectX::XMVECTOR m_lookVec = DirectX::XMLoadFloat3(&m_lookAt);

	// Use the three loaded float3s as vectors for the view matrix
	m_viewMatrix = DirectX::XMMatrixLookAtLH(m_posVec, m_lookVec, m_upVec);

	return;
}

void LightObject::GenerateOrthoMatrix(float width, float screenDepth, float screenNear)
{
	//setting up a orthographic-based view matrix
	m_orthoMatrix = XMMatrixOrthographicLH(width, width, screenNear, screenDepth);

	return;
}

void LightObject::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}


void LightObject::GetOrthoMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_orthoMatrix;
	return;
}

