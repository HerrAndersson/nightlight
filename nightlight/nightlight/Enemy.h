#pragma once
#include "GameObject.h"
#include "AiUtil.h"
#include "lightObject.h"

class Enemy : public GameObject
{
private:
	
	const float SPEED = 50.0F;

	int enemyType;

	bool isFollowingPlayer;
	vector<XMINT2> path;
	Coord next, end;

	float weights[4];
	float weightchange[4];

	bool CheckPathValidity(Level* level);

public:

	enum EnemyType { SMALL, MEDIUM, LARGE };
	Enemy(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, std::string enemyType);

	virtual ~Enemy();

	void Update(LightObject* spotlight);
	void UpdateWeights(XMFLOAT4 &outputweights);

	void SetPath(vector<XMINT2> path);

	bool IsFollowingPlayer();
	bool HasValidPath(Level* level);

	bool InLight(LightObject* spotlight);

};

