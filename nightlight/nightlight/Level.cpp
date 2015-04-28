#include "Level.h"


Level::Level ( ) {
}


Level::~Level ( ) {
}

void Level::PushGameObjectToGrid ( int coordX, int coordY, GameObject &content ) {
	if ( coordX > tileGrid.size ( ) - 1 ) {
		tileGrid.resize( coordX + 1 );
	}
	if ( coordY > tileGrid.at ( coordX ).size ( ) - 1 ) {
		tileGrid.at ( coordX ).resize ( coordY + 1 );
	}

	tileGrid[ coordX ][ coordY ].pushGameObjectToTile ( content );
}
