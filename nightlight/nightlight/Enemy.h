#pragma once
#include "GameObject.h"
#include "AiUtil.h"
class Enemy : public GameObject
{
private:

	bool followingPlayer;
	vector<Position*> path;

public:

	Enemy(XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 rotation);
	virtual ~Enemy();

	void Update();

	bool IsFollowingPlayer();
	bool HasValidPath();
};

