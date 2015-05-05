#include "Enemy.h"

Enemy::Enemy(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, int enemyType)
	 : GameObject(position, rotation,  renderObject, coordX, coordY)
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
		if (HasValidPath())
		{
			//follow path here
		}
		else
		{
			//Get new path? Since this is handled each frame in AiModule, maybe HasValidPath will never be false?
		}
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