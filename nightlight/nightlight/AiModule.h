#pragma once
#include "AiUtil.h"
#include "Enemy.h"
#include "Tile.h"
using namespace std;

class AiModule
{
private:

	vector<vector<Tile>> tileGrid;

public:

	AiModule();
	~AiModule();

	void HandleAI(Enemy* enemy);

	//For use from the outside?
	vector<Node*> GetPath(vector<vector<Tile>>* tileGrid, XMINT2 startPosXZ, XMINT2 endPosXZ);

};

