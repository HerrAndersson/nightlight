#include "Enemy.h"

Enemy::Enemy(XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 rotation)
	 : GameObject(worldMatrix, renderObject, position, rotation)
{
	bool followPlayer = false;
}

Enemy::~Enemy()
{
	for (auto p : path) delete p;
	path.clear();
}

void Enemy::Update()
{
	if (!followPlayer)
	{
		//follow path here
	}
	else
	{
		//Follow player here, using potential field
	}
}

bool Enemy::IsFollowingPlayer()
{
	return followPlayer;
}

bool Enemy::HasValidPath()
{
	//Check the path for validity
	return true;
}