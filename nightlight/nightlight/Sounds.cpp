#include "Sounds.h"


Sounds::Sounds()
{
	YSE::System().init();

	menuMusic.create("Assets/Sounds/menuMusic.ogg");
	menuMusic.setLooping(true);

	gameMusic.create("Assets/Sounds/gameMusic.ogg");
	//gameMusic.setLooping(true);

	walk.create("Assets/Sounds/walk.ogg");
	hit.create("Assets/Sounds/hit.ogg");
	doorOpen.create("Assets/Sounds/doorOpen.ogg");
	leverFailed.create("Assets/Sounds/leverFailed.ogg");
	dies.create("Assets/Sounds/dies.ogg");
	endDoorOpen.create("Assets/Sounds/endDoorOpen.ogg");
	moveBox.create("Assets/Sounds/moveBox.ogg");
	grabReleaseBox.create("Assets/Sounds/grabReleaseBox.ogg");
	shadowContainerActivated.create("Assets/Sounds/shadowContainerActivated.ogg");
}


Sounds::~Sounds()
{
	YSE::System().close();
}

void Sounds::Update()
{


	YSE::System().update();
}