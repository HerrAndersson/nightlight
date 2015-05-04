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
	vector<Node> GetPath(XMFLOAT2 startPosXZ, XMFLOAT2 endPosXZ);


};

