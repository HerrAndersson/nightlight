#pragma once
#include "AiUtil.h"
#include "Enemy.h"
#include "Tile.h"
using namespace std;

class AiModule
{
private:

	const int R_MAX = 10;
	const int R_MIN = 4;

	vector<vector<Tile*>> tileGrid;

	vector< vector<int> > staticPF;
	vector< vector<int> > dynamicPF;
	vector< vector<int> > totalPF;

	void GenerateStaticPF(vector<GameObject*> staticObjects);
	void GenerateDynamicPF(vector<GameObject*> dynamicObjects);
	void GenerateTotalPF();

	XMINT2 GetRandomPosition(Enemy* ai);

public:

	AiModule(vector<vector<Tile*>> tileGrid);
	~AiModule();

	void HandleAI(Enemy* enemy);

	void ChangeTileGrid(vector<vector<Tile*>> tileGrid);
	void Update(vector<GameObject*> dynamicObjects);

	//For use from the outside?
	vector<Node*> GetPath(vector<vector<Tile*>> tileGrid, XMINT2 startPosXZ, XMINT2 endPosXZ);

	vector<vector<int>> GetTotalPF();
};

