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
		XMFLOAT3 p = ai->GetPosition();
		XMINT2 goal = GetRandomPosition(ai);
		ai->SetPath(aStar(tileGrid, 10, XMINT2((int)p.x, (int)p.z), goal));
	}
	else
	{
		//Do something else?
	}
}

XMINT2 AiModule::GetRandomPosition(Enemy* ai)
{
	XMINT2 goal(0, 0);
	bool found = false;

	while (!found)
	{
		for (int i = 0; i < 2; i++)
		{
			int rnd = rand() % (R_MAX - R_MIN) + R_MIN;

			//int max = ((rand() % R_MAX) + R_MIN);
			//int min = ((rand() % R_MIN) + R_MIN);
			//int rnd = rand() % (max - min) + min;

			if (i == 0)
			{
				if (rand() % 2 == 0)
				{
					goal.x = rnd;

					int boundsX = tileGrid->size();
					if (goal.x >= boundsX)
						goal.x = boundsX;
				}
				else
				{
					goal.x = -rnd;
					if (goal.x < 0)
						goal.x = 0;
				}
			}
			else if (i == 1)
			{
				if (rand() % 2 == 0)
				{
					goal.y = rnd;

					int boundsY = tileGrid->at(goal.x).size();
					if (goal.y >= boundsY)
						goal.y = boundsY;
				}
				else
				{
					goal.y = -rnd;

					if (goal.y < 0)
						goal.y = 0;
				}
			}
		}

		if (tileGrid->at(goal.x).at(goal.y).getTileIsWalkable())
			found = true;
	}

	return goal;
}

vector<Node*> AiModule::GetPath(vector< vector<Tile> >* tileGrid, XMINT2 startPosXZ, XMINT2 endPosXZ)
{
	return aStar(tileGrid, 10, startPosXZ, endPosXZ);
}

void AiModule::ChangeTileGrid(vector< vector<Tile> >* tileGrid)
{
	this->tileGrid = tileGrid;
	//GenerateStaticPF()
}

void AiModule::Update(vector<GameObject*> dynamicObjects)
{
	//Generate dynamic pf each frame, or as often as needed.

	GenerateTotalPF();
}

void AiModule::GenerateStaticPF(vector<GameObject*> staticObjects)
{
	//Sum the hard-coded potential fields of all static objects (walls, pillars, static lights etc). Or check position and generate a field there.
}
void AiModule::GenerateDynamicPF(vector<GameObject*> dynamicObjects)
{
	//Sum the hard-coded potential fields of all dynamic objects (player, boxes, dynamic lights etc). Or check position and generate a field there.
}
void AiModule::GenerateTotalPF()
{
	//Sum the static and dynamic fields to get the total.
	//Update the field for each tile?
}
vector< vector<int> > AiModule::GetTotalPF()
{
	return totalPF;
}
