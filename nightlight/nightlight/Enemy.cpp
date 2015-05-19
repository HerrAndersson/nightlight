#include "Enemy.h"
#include "Collision.h"

Enemy::Enemy(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, std::string enemyType)
	 : GameObject(position, rotation,  renderObject, coordX, coordY)
{
	if (enemyType == "small")
		this->enemyType = EnemyType::SMALL;
	else if (enemyType == "average")
		this->enemyType = EnemyType::MEDIUM;
	else if (enemyType == "large")
		this->enemyType = EnemyType::LARGE;

	followingPlayer = true;

	weights[0] = 1;
	weights[1] = 0;
	weights[2] = 0;
	weights[3] = 0;
}

Enemy::~Enemy()
{
	path.clear();
}

void Enemy::Update(Level* level, LightObject* spotlight)
{
	if (path.size() > 0)
	{
		Coord aiCoord = GetTileCoord();
		Coord dir = aiCoord - next;

		XMFLOAT3 currentPos = GetPosition();
		XMFLOAT3 direction = XMFLOAT3((-currentPos.x - (next.x + (TILE_SIZE / 2))), 0.0f, -currentPos.z - (next.y + (TILE_SIZE / 2)));

		XMFLOAT3 nextPos = GetPosition();
		nextPos.x += dir.x / SPEED;
		nextPos.z += dir.y / SPEED;

		float radius = 0.5f;
		bool result = false;
		bool inLight = false;

		Tile* currentTile = level->getTile(aiCoord);
		if (currentTile != nullptr)
		{
			Coord nextTileCoord = Coord((int)(abs(nextPos.x)), (int)(abs(nextPos.z)));
			Tile* nextTile = level->getTile(nextTileCoord.x, nextTileCoord.y);
			if (nextTile != nullptr)
			{
				for (int x = nextTileCoord.x - 1; x <= nextTileCoord.x + 1 && !inLight; x++)
				{
					for (int y = nextTileCoord.y - 1; y <= nextTileCoord.y + 1 && !inLight; y++)
					{
						Tile* iteratorTile = level->getTile(x, y);
						if (iteratorTile && !iteratorTile->IsWalkableAI())
						{	
							inLight = InLight(level, this, spotlight);

							Coord iteratorTileCoord = Coord(x, y);
							nextPos = NextPositionFromCollision(result, nextPos, radius, iteratorTileCoord);
						}
					}
				}
			}
		}

		float xRem = -(nextPos.x - (int)nextPos.x);
		float zRem = -(nextPos.z - (int)nextPos.z);
		//cout << xRem << " " << zRem << endl;
		// && xRem >= 0.45f && zRem >= 0.45f

		if (aiCoord == next)
		{
			//cout << "next!" << endl;
			XMINT2 p = path.at(path.size() - 1);
			this->path.pop_back();

			next = Coord(p.x, p.y);
		}

		SetPosition(nextPos);
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
	return followingPlayer;
}

void Enemy::SetFollowingPlayer(bool val)
{
	followingPlayer = val;
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
