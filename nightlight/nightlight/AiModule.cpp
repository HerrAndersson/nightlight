#include "AiModule.h"

AiModule::AiModule()
{
	//Recieve all enemies here and save a pointer to them?
	//Recieve the level grid here and save.
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

//Position* AiModule::GetPath(XMFLOAT2 startPosXZ, XMFLOAT2 endPosXZ)
//{
//	//return aStar(level, level->GetTileDimension(), startPosXZ, endPosXZ);
//}