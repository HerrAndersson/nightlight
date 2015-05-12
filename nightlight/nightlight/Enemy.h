#pragma once
#include "GameObject.h"
#include "AiUtil.h"
class Enemy : public GameObject
{
private:

	int enemyType;
	bool followingPlayer;
	vector<XMFLOAT3> path;

public:

	Enemy(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, int enemyType);
	virtual ~Enemy();

	enum EnemyType { SMALL, MEDIUM, LARGE };

	void Update();

	void SetPath(vector<XMFLOAT3> path);

	bool IsFollowingPlayer();
	bool HasValidPath();
};

