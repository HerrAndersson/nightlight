#include "Level.h"


Level::Level ( ) {
}


Level::~Level ( ) {
}

void Level::PushGameObjectToGrid ( int coordX, int coordY, GameObject& content ) {
	if ( coordX > (int)(tileGrid.size ( ) - 1 )) {
		tileGrid.resize( coordX + 1 );
	}
	if ( coordY > (int)(tileGrid.at ( coordX ).size ( ) - 1) ) {
		tileGrid.at ( coordX ).resize ( coordY + 1 );
	}

	tileGrid[ coordX ][ coordY ].pushGameObjectToTile ( content );
}

void Level::SetGameObjects(){
	for (int x = 0; x < tileGrid.size(); x++)
		for (int y = 0; y < tileGrid[x].size(); y++)
			for (int z = 0; z < tileGrid[x][y].GetNumberOfGameObjects(); z++)
				gameObjects.push_back(tileGrid[x][y].GetGameObject(z));
}