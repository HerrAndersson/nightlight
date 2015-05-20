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

	bool CheckPathValidity(Level* level);

	float frame;
	int moved;

	std::vector<Animation> animations;
	float framelength;
	int currentAnim;

	void UpdatePrimaryAnimation();
	void UpdateSecondaryAnimation();

public:

	enum EnemyType { SMALL, MEDIUM, LARGE };
	Enemy(XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, std::string enemyType);

	virtual ~Enemy();

	void Update(Level* level, LightObject* spotlight);

	void SetPath(vector<XMINT2> path);

	void SetFollowingPlayer(bool val);
	bool IsFollowingPlayer();
	bool HasValidPath(Level* level);

	void PlayAnimation(int animID);
	void UpdateAnimation();
	void SetUpAnimation(RenderObject* anim, float framelengthin);
};

