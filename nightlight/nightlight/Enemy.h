#pragma once
#include "GameObject.h"
#include "AiUtil.h"
class Enemy : public GameObject
{
private:

	int enemyType;
	bool followingPlayer;
	vector<Node*> path;

public:

	Enemy(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, int enemyType);
	virtual ~Enemy();

	void Update();

	void SetPath(vector<Node*> path);

	bool IsFollowingPlayer();
	bool HasValidPath();
};

