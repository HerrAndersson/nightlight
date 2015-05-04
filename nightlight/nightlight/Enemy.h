#pragma once
#include "GameObject.h"
#include "AiUtil.h"
class Enemy : public GameObject
{
private:
	int enemyType;
	bool followingPlayer;
	vector<Position*> path;

public:

	Enemy(XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 rotation, int enemyType);
	virtual ~Enemy();

	void Update();

	bool IsFollowingPlayer();
	bool HasValidPath();
};

