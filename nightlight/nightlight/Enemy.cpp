#include "Enemy.h"

Enemy::Enemy(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, std::string enemyType)
	 : GameObject(position, rotation,  renderObject, coordX, coordY)
{
	if (enemyType == "small")
		this->enemyType = EnemyType::SMALL;
	else if (enemyType == "average")
		this->enemyType = EnemyType::MEDIUM;
	else if (enemyType == "large")
		this->enemyType = EnemyType::LARGE;

	isFollowingPlayer = true;

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
	if (path.size() > 0)
	{
		Coord aiCoord = GetTileCoord();

		if (aiCoord == next)
		{
			XMINT2 p = path.at(path.size() - 1);
			this->path.pop_back();

			next = Coord(p.x, p.y);
		}


		Coord dir = aiCoord - next;

		XMFLOAT3 pos = GetPosition();
		pos.x += dir.x / SPEED;
		pos.z += dir.y / SPEED;
		SetPosition(pos);
	}
}

void Enemy::UpdateWeights(XMFLOAT4 &outputweights)
{
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
	return isFollowingPlayer;
}

bool Enemy::HasValidPath(Level* level)
{
	return CheckPathValidity(level);
}

bool Enemy::CheckPathValidity(Level* level)
{
	bool valid = true;

	//Traverse the path and check all tiles for validity
	for (int i = path.size() - 1; i > 0; i--)
	{
		XMINT2 p = path.at(i);

		Tile* tile = level->getTile(p.x, p.y);
		if (!(tile && tile->IsWalkableAI()))
		{
			valid = false;
			break;
		}
	}

	if (GetTileCoord() == end)
		valid = false;

	if (path.size() <= 0)
		valid = false;

	return valid;
}

void Enemy::SetPath(vector<XMINT2> path)
{
	this->path = path;

	if (path.size() > 0)
	{
		end = Coord(path.at(0).x, path.at(0).y);
		next = Coord(path.at(path.size() - 1).x, path.at(path.size() - 1).y);
	}
}

bool Enemy::InLight(LightObject* spotlight)
{
	XMFLOAT3 pos = GetPosition();
	XMFLOAT3 lightEnemyVec = XMFLOAT3((pos.x - spotlight->getPosition().x), (pos.y - spotlight->getPosition().y), (pos.z - spotlight->getPosition().z));
	float vecLenght = sqrt((lightEnemyVec.x * lightEnemyVec.x) + (lightEnemyVec.y * lightEnemyVec.y) + (lightEnemyVec.z * lightEnemyVec.z));

	if ((spotlight->getRange() / 2) > vecLenght)
	{
		XMFLOAT3 spotDirection = spotlight->getDirection();

		float dot = spotDirection.x*lightEnemyVec.x + spotDirection.y*lightEnemyVec.y + spotDirection.z*lightEnemyVec.z;
		float lenSq1 = spotDirection.x*spotDirection.x + spotDirection.y*spotDirection.y + spotDirection.z*spotDirection.z;
		float lenSq2 = lightEnemyVec.x*lightEnemyVec.x + lightEnemyVec.y*lightEnemyVec.y + lightEnemyVec.z*lightEnemyVec.z;
		float angle = acos(dot / sqrt(lenSq1 * lenSq2));

		float angleInRads = (180 / XM_PI) * angle;

		if (spotlight->getCone() < angleInRads)
			return false;

		return true;
	}
	return false;
}