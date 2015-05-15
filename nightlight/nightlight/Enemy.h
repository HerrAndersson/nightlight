#pragma once
#include "GameObject.h"
#include "AiUtil.h"
class Enemy : public GameObject
{
private:

	int enemyType;
	bool followingPlayer;
	vector<XMINT2> path;

	bool hasValidPath;

	float weights[4];
	float weightchange[4];

public:

	Enemy(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, int enemyType);
	virtual ~Enemy();

	enum EnemyType { SMALL, MEDIUM, LARGE };

	void Update();

	void SetPath(vector<XMINT2> path);

	bool IsFollowingPlayer();
	bool HasValidPath();
	void UpdateWeights(XMFLOAT4 &outputweights);
};

