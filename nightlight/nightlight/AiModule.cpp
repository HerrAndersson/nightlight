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

vector<Node*> AiModule::GetPath(vector< vector<Tile> >* tileGrid, XMINT2 startPosXZ, XMINT2 endPosXZ)
{
	return aStar(tileGrid, 10, startPosXZ, endPosXZ);
}

void AiModule::ChangeLevel(vector< vector<Tile> > tileGrid)
{
	this->tileGrid = tileGrid;
	//GenerateStaticPF();
}

void AiModule::Update(vector<GameObject*> dynamicObjects)
{
	//Generate dynamic pf each frame, or as often as needed.

	GenerateTotalPF();
}

void AiModule::GenerateStaticPF(vector<GameObject*> staticObjects)
{
	//Sum the potential fields of all static objects (walls, pillars, static lights etc). Or check position and generate a field there.
}
void AiModule::GenerateDynamicPF(vector<GameObject*> dynamicObjects)
{
	//Sum the potential fields of all dynamic objects (player, boxes, dynamic lights etc). Or check position and generate a field there.
}
void AiModule::GenerateTotalPF()
{
	//Sum the static and dynamic fields to get the total.
	//Update the field for each tile?
}