#include "GameLogic.h"

GameLogic::GameLogic(HWND hwnd, int screenWidth, int screenHeight)
{
	Input = new InputManager(hwnd, screenWidth, screenHeight);
}

GameLogic::~GameLogic()
{
	delete Input;
}

bool GameLogic::Update(Level* currentLevel, Character* character, CameraObject* camera, LightObject* spotLight)
{
	return UpdatePlayer(currentLevel, character, camera, spotLight);
}

bool GameLogic::UpdatePlayer(Level* currentLevel, Character* character, CameraObject* camera, LightObject* spotlight)
{
	XMFLOAT2 oldP = Input->GetMousePos();
	Input->HandleMouse();
	XMFLOAT2 newP = Input->GetMousePos();

	XMFLOAT3 pos = character->GetPosition();
	XMFLOAT3 rot = character->GetRotation();

	bool playerMoved = false;

	if ( Input->KeyDown ('w') ) 
	{
		pos = XMFLOAT3 (pos.x, pos.y, pos.z + 0.1f);
		playerMoved = true;
	}
	if ( Input->KeyDown ('s') )
	{
		pos = XMFLOAT3 (pos.x, pos.y, pos.z - 0.1f);
		playerMoved = true;
	}
	if ( Input->KeyDown ('a') )
	{
		pos = XMFLOAT3 (pos.x + 0.1f, pos.y, pos.z);
		playerMoved = true;
	}
	if ( Input->KeyDown ('d') )
	{
		pos = XMFLOAT3 (pos.x - 0.1f, pos.y, pos.z);
		playerMoved = true;
	}

	if (playerMoved) 
	{
		pos = ManageStaticPlayerCollisions(currentLevel, character, pos);
	}

	//Only update if mouse or player moved
	if (oldP.x != newP.x || oldP.y != newP.y || playerMoved)
	{
		XMMATRIX v, p, vp;
		camera->GetViewMatrix(v);
		camera->GetProjectionMatrix(p);
		vp = v * p;

		XMVECTOR localSpace = XMVector3Transform ( XMLoadFloat3 ( &pos ), vp );
		float screenX = XMVectorGetX ( localSpace ) / XMVectorGetZ ( localSpace );
		float screenY = XMVectorGetY ( localSpace ) / XMVectorGetZ ( localSpace );

		XMFLOAT2 msp = Input->GetMousePosScreenSpace();
		double dx = ( msp.x - screenX );
		double dy = ( msp.y - screenY ) / camera->GetAspectRatio();
		double angle = atan2(dy, dx) * (180 / XM_PI);

		rot = XMFLOAT3(0.0f, (float)angle, 0.0f);
		character->SetRotation(rot);
	}

	camera->SetPosition(character->GetPosition().x, -15, character->GetPosition().z );
	camera->SetLookAt(character->GetPosition().x * 0.9f, 0.0f, character->GetPosition().z * 0.9f);
	character->SetPosition(pos);

	UpdateSpotLight ( character, camera, spotlight );

	return !Input->Esc();
}

bool GameLogic::UpdateSpotLight (Character* player, CameraObject* camera, LightObject* spotlight)
{
	XMFLOAT3 pForward;
	XMStoreFloat3 ( &pForward, player->GetForwardVector ( ) );
	spotlight->setDirection ( pForward.x, pForward.y, pForward.z );

	
	/*XMFLOAT3 lightDirFinal;
	XMStoreFloat3(&lightDirFinal, player->GetForwardVector());*/
	
	XMFLOAT3 pPos = player->GetPosition ( );
	//offset light
	pPos.x += pForward.x/100;
	pPos.z += pForward.z/100;
	pPos.y -= 0.7f;
	spotlight->setPosition ( pPos.x, pPos.y, pPos.z );
	spotlight->generateViewMatrix();
	return true;

}

XMFLOAT3 GameLogic::ManageStaticPlayerCollisions(Level* currentLevel, Character* character, XMFLOAT3 nextPos) {
	float tileOffset = TILE_SIZE / 2;

	XMFLOAT3 currentPos = character->GetPosition();

	Tile* currentTile = currentLevel->getTile((int)(abs(currentPos.x)), (int)(abs(currentPos.z)));
	if (currentTile != nullptr) {

		int nextTileCoordX = (int)(abs(nextPos.x));
		int nextTileCoordY = (int)(abs(nextPos.z));
		Tile* nextTile = currentLevel->getTile(nextTileCoordX, nextTileCoordY);
		if (nextTile != nullptr) {
			for (int x = nextTileCoordX - 1; x <= nextTileCoordX + 1; x++) {
				for (int y = nextTileCoordY - 1; y <= nextTileCoordY + 1; y++) {
					Tile* tile = currentLevel->getTile(x, y);
					if (!IsTileWalkable(tile)) {
						nextPos = NextPositionFromCollision(nextPos, character->getCharacterRadius(), Coord(x, y));
					}
					else {
						Door* door = tile->getDoor();
						if (door != nullptr && !door->getIsOpen()) {
							XMFLOAT3 doorRot = door->GetRotation();
							//TODO: some voodo to enable door collisions
						}
					}
				}
			}
		}
	}
	return nextPos;
}

bool GameLogic::IsTileWalkable(Tile* tile) {
	if (tile == nullptr ||
		tile->getMovableObject() != nullptr ||
		tile->getShadowContainer() != nullptr ||
		tile->getStaticObject() != nullptr ||
		tile->getFloorTile() == nullptr) {
		return false;
	}
	return true;
}

XMFLOAT3 GameLogic::NextPositionFromCollision(XMFLOAT3 nextPos, float radius, Coord tileCoord) {
	//Calculate the tiles edge coordinates.
	float xMin = -(tileCoord.x + TILE_SIZE);
	float xMax = -tileCoord.x;
	float yMin = -(tileCoord.y + TILE_SIZE);
	float yMax = -tileCoord.y;

	//Find the closest point to the character on the perimiter of the tile.
	float closestX = Clamp(nextPos.x, xMin, xMax);
	float closestY = Clamp(nextPos.z, yMin, yMax);

	float distanceX = nextPos.x - closestX;
	float distanceY = nextPos.z - closestY;
	float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);

	if (distanceSquared < (radius * radius)) {
		float length = sqrt(distanceSquared);

		nextPos.x = (distanceX / length) * radius + closestX;
		nextPos.z = (distanceY / length) * radius + closestY;
	}
	return nextPos;
}