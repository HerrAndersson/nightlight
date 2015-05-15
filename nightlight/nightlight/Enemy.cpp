#include "Enemy.h"

Enemy::Enemy(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, std::string enemyType)
	 : GameObject(position, rotation,  renderObject, coordX, coordY)
{
	if (enemyType == "small")
	{
		this->enemyType = EnemyType::SMALL;
	}
	else if (enemyType == "average")
	{
		this->enemyType = EnemyType::MEDIUM;
	}
	else if (enemyType == "large")
	{
		this->enemyType = EnemyType::LARGE;
	}
	

	followingPlayer = true;
	hasValidPath = false;

	weights[0] = 1;
	weights[1] = 0;
	weights[2] = 0;
	weights[3] = 0;
}

Enemy::~Enemy()
{
	path.clear();
}

void Enemy::Update()
{
	if (!followingPlayer)
	{
		if (HasValidPath())
		{
			Coord aiCoord = GetTileCoord();
			XMINT2 p = path.at(path.size() - 1);
			path.pop_back();

			Coord pathCoord = Coord(p.x, p.y);

			SetTilePosition(pathCoord);

			if (aiCoord == pathCoord)
			{
				hasValidPath = false;
			}
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

void Enemy::UpdateWeights(XMFLOAT4 &outputweights)
{
	//system("CLS");
	float weightchangechange[4] = { (float)(rand() % 100), (float)(rand() % 100), (float)(rand() % 100), (float)(rand() % 100) };
	float totalweightchangechange = weightchangechange[0] + weightchangechange[1] + weightchangechange[2] + weightchangechange[3];

	for (int i = 0; i < 4; i++)
	{
		weightchangechange[i] = weightchangechange[i] / totalweightchangechange * 4.0f - 1.0f;
		weightchange[i] += weightchangechange[i];
		weightchange[i] *= 0.9f;
	}

	float totalweight = 0;
	for (int i = 0; i < 4; i++)
	{
		weights[i] += (weightchange[i] / 60);
		if (weights[i] < 0)
			weights[i] = 0;
		if (weights[i] > 1)
			weights[i] = 1;
		totalweight += weights[i];
	}

	outputweights.x = weights[0] / totalweight;
	outputweights.y = weights[1] / totalweight;
	outputweights.z = weights[2] / totalweight;
	outputweights.w = weights[3] / totalweight;
}

bool Enemy::IsFollowingPlayer()
{
	return followingPlayer;
}

bool Enemy::HasValidPath()
{
	//Check the path for validity
	return hasValidPath;
}

void Enemy::SetPath(vector<XMINT2> path)
{
	this->path = path;
}