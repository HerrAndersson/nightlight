#include "AiModule.h"

AiModule::AiModule()
{
	//Receive all enemies here and save a pointer to them?
	//Receive the level grid here and save.
}

AiModule::~AiModule()
{

}

void AiModule::HandleAI(Enemy* ai)
{
	if (!ai->IsFollowingPlayer() && !ai->HasValidPath())
	{
		//Give the enemy a path to follow
	}
	else
	{
		//Do something else?
	}
}

vector<Node*> AiModule::GetPath(vector<vector<Tile>>* tileGrid, XMINT2 startPosXZ, XMINT2 endPosXZ)
{
	return aStar(tileGrid, 10, startPosXZ, endPosXZ);
}
