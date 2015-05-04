#include "Enemy.h"

Enemy::Enemy(XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 rotation, int enemyType)
	 : GameObject(worldMatrix, renderObject, position, rotation)
{
	this->enemyType = enemyType;
	bool followPlayer = false;
}

Enemy::~Enemy()
{
	for (auto p : path) delete p;
	path.clear();
}

void Enemy::Update()
{
	if (!followingPlayer)
	{
		if (!HasValidPath())
		{
			
		}
		//follow path here
	}
	else
	{
		//Follow player here, using potential field
	}
}

bool Enemy::IsFollowingPlayer()
{
	return followingPlayer;
}

bool Enemy::HasValidPath()
{
	//Check the path for validity
	return true;
}