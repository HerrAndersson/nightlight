#include "Enemy.h"
#include "Collision.h"
#include <math.h>

Enemy::Enemy(int id, XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, std::string enemyType)
	: GameObject(id, position, rotation, renderObject, coordX, coordY)
{
	if (enemyType == "small")
		this->enemyType = EnemyType::SMALL;
	else if (enemyType == "average")
		this->enemyType = EnemyType::MEDIUM;
	else if (enemyType == "large")
		this->enemyType = EnemyType::LARGE;

	followingPlayer = false;

	SetUpAnimation(renderObject, 3);
	PlayAnimation(0);
}

Enemy::~Enemy()
{
	path.clear();
}

void Enemy::Update(Level* level, LightObject* spotlight)
{
	if (path.size() > 0)
	{
		XMFLOAT3 nextPos = position;
		nextPos.x += direction.x / SPEED;
		nextPos.z += direction.z / SPEED;

		float radius = 0.5f;
		bool result = false;
		bool inLight = false;

		Tile* currentTile = level->getTile(tileCoord);
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
		// 

		if (tileCoord == next && xRem >= 0.45f && zRem >= 0.45f)
		{
			level->getTile(next)->getFloorTile()->SetSelected(false);
			//cout << "next!" << endl;
			XMINT2 p = path.at(path.size() - 1);
			this->path.pop_back();

			next = Coord(p.x, p.y);
			direction = XMFLOAT3((-position.x - (next.x + (TILE_SIZE / 2))), 0.0f, -position.z - (next.y + (TILE_SIZE / 2)));

			rotation.y = -atan2(direction.z, direction.x) * 180 / XM_PI;
		}
		moved = 1;
		SetPosition(nextPos);
	}
	else
		moved = 0;
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
	//Traverse the path and check all tiles for validity
	for (int i = path.size() - 1; i >= 0; i--)
	{
		XMINT2 p = path.at(i);

		Tile* tile = level->getTile(p.x, p.y);
		if (!(tile && tile->IsWalkableAI()))
		{
			return false;
		}
	}

	if (GetTileCoord() == end)
		return false;

	if (path.size() <= 0)
		return false;

	return true;
}

void Enemy::SetPath(vector<XMINT2> path)
{
	this->path = path;

	if (path.size() > 0)
	{
		end = Coord(path.at(0).x, path.at(0).y);
		next = Coord(path.at(path.size() - 1).x, path.at(path.size() - 1).y);
		direction = XMFLOAT3((-position.x - (next.x + (TILE_SIZE / 2))), 0.0f, -position.z - (next.y + (TILE_SIZE / 2)));
		rotation.y = -atan2(direction.z, direction.x) * 180 / XM_PI;
	}
}

vector<XMINT2> Enemy::GetPath()
{
	return path;
}

void Enemy::UpdateAnimation()
{
	if (!currentAnim)
	{
		UpdatePrimaryAnimation();
	}
	else
	{
		UpdateSecondaryAnimation();
	}
}

void Enemy::UpdatePrimaryAnimation()
{
	if (moved)
	{
		frame += 0.1f;
		if (frame > (framelength * 2))
			frame = 0;

		if (frame < (framelength * 2))
		{
			Weights.x = 0;
			Weights.y = 0;
			Weights.z = (-frame + (framelength * 2)) / framelength;
			Weights.w = (frame - (framelength)) / framelength;
		}
		if (frame < (framelength))
		{
			Weights.x = 0;
			Weights.y = (-frame + (framelength)) / framelength;
			Weights.z = frame / framelength;
			Weights.w = 0;
		}
	}
	else{
		frame = 0;
		Weights = { 1, 0, 0, 0 };
	}
}

void Enemy::UpdateSecondaryAnimation()
{
	frame += 0.1f;
	if (frame > (framelength * 3))
		PlayAnimation(0);

	if (frame < (framelength * 3))
	{
		Weights.x = 0;
		Weights.y = 0;
		Weights.z = (-frame + (framelength * 3)) / framelength;
		Weights.w = (frame - (framelength * 2)) / framelength;
	}
	if (frame < (framelength * 2))
	{
		Weights.x = 0;
		Weights.y = (-frame + (framelength * 2)) / framelength;
		Weights.z = (frame - (framelength)) / framelength;
		Weights.w = 0;
	}
	if (frame < (framelength))
	{
		Weights.x = (-frame + (framelength)) / framelength;
		Weights.y = frame / framelength;
		Weights.z = 0;
		Weights.w = 0;
	}
}

void Enemy::PlayAnimation(int animID)
{
	frame = 0;
	currentAnim = animID;
	renderObject = animations[animID].animation;
	framelength = animations[animID].framelength;
}

void Enemy::SetUpAnimation(RenderObject* anim, float framelengthin)
{
	Animation temp;
	temp.animation = anim;
	temp.framelength = framelengthin;
	animations.push_back(temp);
}
