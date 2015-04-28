#pragma once
#include "AiUtil.h"
#include "Enemy.h"
class AiModule
{
private:

	//Level* level

public:

	AiModule();
	~AiModule();

	void HandleAI(Enemy* ai);

	//For use from  the outside?
	Position* GetPath(XMFLOAT2 startPosXZ, XMFLOAT2 endPosXZ)

};

