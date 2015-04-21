#ifndef _LIGHTCLASS_H_
#define _LIGHTCLASS_H_




#include <DirectXMath.h>
using namespace DirectX;


class LightClass
{
public:
	LightClass();
	~LightClass();

	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float);
	void SetPosition(float, float, float);

	XMFLOAT4 GetAmbientColor();
	XMFLOAT4 GetDiffuseColor();
	XMFLOAT3 GetDirection();
	XMFLOAT3 GetPosition();

	//Different types of view matrices; a regular perspective and an orthographic one.
	//These could in a later case be applied to create shadow maps.
	void GenerateViewMatrix();
	void GetViewMatrix(XMMATRIX&);

	void GenerateOrthoMatrix(float, float, float);
	void GetOrthoMatrix(XMMATRIX&);



private:
	XMFLOAT4 m_ambientColor;
	XMFLOAT4 m_diffuseColor;
	XMFLOAT3 m_position;
	XMFLOAT3 m_direction;
	XMFLOAT3 m_lookAt;
	XMMATRIX m_viewMatrix;
	XMMATRIX m_orthoMatrix;


};

#endif