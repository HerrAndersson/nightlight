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

//vector<Position> AiModule::GetPath(XMFLOAT2 startPosXZ, XMFLOAT2 endPosXZ)
//{
//	//return aStar(level, level->GetTileDimension(), startPosXZ, endPosXZ);
//}
