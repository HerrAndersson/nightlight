#pragma once
#include "AiUtil.h"
#include "Enemy.h"
#include "Tile.h"
using namespace std;

class AiModule
{
private:

	vector< vector<Tile> > tileGrid;

	vector< vector<int> > staticPF;
	vector< vector<int> > dynamicPF;
	vector< vector<int> > totalPF;

	void GenerateStaticPF(vector<GameObject*> staticObjects);
	void GenerateDynamicPF(vector<GameObject*> dynamicObjects);
	void GenerateTotalPF();

public:

	AiModule();
	~AiModule();

	void HandleAI(Enemy* enemy);

	void ChangeLevel(vector< vector<Tile> > tileGrid);
	void Update(vector<GameObject*> dynamicObjects);

	//For use from the outside?
	vector<Node*> GetPath(vector< vector<Tile> >* tileGrid, XMINT2 startPosXZ, XMINT2 endPosXZ);

};

