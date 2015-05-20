#pragma once
#include "GameObject.h"
#include "AiUtil.h"
#include "lightObject.h"

class Enemy : public GameObject
{
private:
	
	const float SPEED = 25.0F;

	int enemyType;

	bool followingPlayer;
	vector<XMINT2> path;
	Coord next, end;
	XMFLOAT3 direction;
	float tileEps = 0.05f;

	float weights[4];
	float weightchange[4];

	bool CheckPathValidity(Level* level);

public:

	enum EnemyType { SMALL, MEDIUM, LARGE };
	Enemy(int id, XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, std::string enemyType);

	virtual ~Enemy();

	void Update(Level* level, LightObject* spotlight);
	void UpdateWeights(XMFLOAT4 &outputweights);

	void SetPath(vector<XMINT2> path);
	vector<XMINT2> GetPath();

	void SetFollowingPlayer(bool val);
	bool IsFollowingPlayer();
	bool HasValidPath(Level* level);

};

