#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

#include <vector>
#include <deque>
#include <list>

#include "Level.h"
#include "GameObject.h"
#include "Tile.h"

using namespace std;
using DirectX::XMFLOAT2;
using DirectX::XMINT2;

//struct Node
//{
//	int x, y;
//
//	//f = g + h, g = sum of all costs to get here, h = heuristic
//	int f, g;
//
//	Node* parent;
//
//	Node(int x, int y)
//	{
//		this->x = x;
//		this->y = y;
//		f, g = 0;
//		parent = nullptr;
//	}
//
//	Node()
//	{
//		x, y, f, g  = 0;
//		parent = nullptr;
//	}
//
//	bool operator==(const Node* n)
//	{
//		if (this->x == n->x && this->y == n->y)
//			return true;
//		else 
//			return false;
//	}
//};

//static int ManhattanDistance(Node* n1, Node* n2) //Cheap, less accurate
//{
//	return (abs(n1->x - n2->x) + abs(n1->y - n2->y));
//}

//static int EuclideanDistance(Node* n1, Node* n2) //Expensive, more accurate
//{
//	return (int)sqrt((pow((n1->x - n2->x), 2) + pow((n1->y - n2->y), 2)));
//}
//
//static int ChebyshevDistance(Node* n1, Node* n2)
//{
//	return max(abs(n1->x - n2->x), abs(n1->y - n2->y));
//}

//static Node GetPositionFromCoord(float x, float z, int tileWidth)
//{
//	return Node((int)floor(x / tileWidth), (int)floor(z / tileWidth));
//}

//static vector<Node*> aStar(Level* level, int tileSize, XMINT2 startPosXZ, XMINT2 endPosXZ) //Start and end are tile positions in the grid.
//{
//	vector<Node*> path;
//	Node* start = new Node(startPosXZ.x, startPosXZ.y);
//	Node* end = new Node(endPosXZ.x, endPosXZ.y);
//	Node* current = start;
//
//	vector<Node*> closedset;
//	list<Node*> openset;
//	openset.push_back(start);
//
//	vector<Node*> adjList;
//
//	start->f = start->g + ManhattanDistance(start, end);
//
//	while (!openset.empty())
//	{
//		cout << "openset size : " << openset.size() << endl;
//		cout << "closedset size : " << closedset.size() << endl;
//
//		for each (Node* n in openset)
//		{
//			if (n->f < current->f)
//				current = n;
//		}
//
//		cout << "Current: " << current->x << " " << current->y << endl;
//
//		if (current->x == end->x && current->y == end->y)
//			break;
//
//		closedset.push_back(current);
//		openset.remove(current);
//
//		adjList.clear();
//		adjList.push_back(new Node(current->x - 1, current->y - 1));
//		adjList.push_back(new Node(current->x - 1, current->y));
//		adjList.push_back(new Node(current->x - 1, current->y + 1));
//		adjList.push_back(new Node(current->x + 1, current->y - 1));
//		adjList.push_back(new Node(current->x + 1, current->y));
//		adjList.push_back(new Node(current->x + 1, current->y + 1));
//		adjList.push_back(new Node(current->x, current->y + 1));
//		adjList.push_back(new Node(current->x, current->y - 1));
//
//		for each (Node* n in adjList)
//		{
//			bool inOpen = false;
//			bool inClosed = false;
//
//			for each (Node* x in closedset)
//			{
//				if (n->x == x->x && n->y == x->y)
//				{
//					inClosed = true;
//					break;
//				}
//			}
//
//			for each (Node* x in openset)
//			{
//				if (n->x == x->x && n->y == x->y)
//				{
//					inOpen = true;
//					break;
//				}
//			}
//
//			cout << "Node: " << current->x << " " << current->y << " inOpen: " << boolalpha << inOpen << " inClosed: " << boolalpha << inClosed << endl;
//
//			//if (!(find(closedset.begin(), closedset.end(), &n) != closedset.end()))
//			if (!inClosed)
//			{
//				int tentativeG = current->g + 1;
//
//				if (!inOpen || tentativeG < n->g)
//				{
//					Tile* tile = nullptr;
//					
//					if (level->withinBounds(n->x, n->y))
//						tile = level->getTile(n->x, n->y);
//
//					if (tile != nullptr)
//					{
//						if (tile->IsWalkable())
//						{
//							n->parent = current;
//							n->g = tentativeG;
//							n->f = n->g + ManhattanDistance(n, end);
//
//							if (!inOpen)
//								openset.push_back(n);
//						}
//					}
//				}
//			}
//		}
//	}
//
//	while (current != start)
//	{
//		cout << current->x << " " << current->y << endl;
//		path.push_back(current);
//		current = current->parent;
//	}
//
//	return path;
//}






static int ManhattanDistance(Tile* n1, Tile* n2) //Cheap, less accurate
{
	XMINT2 n1c = n1->GetTileCoord();
	XMINT2 n2c = n2->GetTileCoord();

	return (abs(n1c.x - n2c.x) + abs(n1c.y - n2c.y));
}

static int EuclideanDistance(Tile* n1, Tile* n2) //Expensive, more accurate
{
	XMINT2 n1c = n1->GetTileCoord();
	XMINT2 n2c = n2->GetTileCoord();

	return (int)sqrt((pow((n1c.x - n2c.x), 2) + pow((n1c.y - n2c.y), 2)));
}

static int ChebyshevDistance(Tile* n1, Tile* n2)
{
	XMINT2 n1c = n1->GetTileCoord();
	XMINT2 n2c = n2->GetTileCoord();

	return max(abs(n1c.x - n2c.x), abs(n1c.y - n2c.y));
}

static bool Equals(XMINT2 f1, XMINT2 f2)
{
	if (f1.x == f2.x && f1.y == f2.y)
		return true;
	else
		return false;
}

static vector<XMFLOAT3> aStar(Level* level, int tileSize, XMINT2 startPosXZ, XMINT2 endPosXZ)
{
	vector<XMFLOAT3> path;

	Tile* start = level->getTile(startPosXZ.x, startPosXZ.y);
	Tile* end = level->getTile(endPosXZ.x, endPosXZ.y);
	Tile* current = nullptr;
	Tile* child = nullptr;

	list<Tile*> openList;
	list<Tile*> closedList;

	list<Tile*>::iterator i;

	//Counter to limit path length
	int n = 0;
	int limit = 100;

	openList.push_back(start);
	start->SetInOpen(true);
	start->SetParent(nullptr);

	while (n == 0 || (current != end && n < limit))
	{
		//Find the smallest F value in openList and make it the current tile
		for (i = openList.begin(); i != openList.end(); ++i)
			if (i == openList.begin() || (*i)->GetF() <= current->GetF())
				current = (*i);

		////Stop if it reached the end
		//if (current == end)
		//	break;

		//if (Equals(current->GetTileCoord(), endPosXZ))
		//	break;

		openList.remove(current);
		current->SetInOpen(false);

		closedList.push_back(current);
		current->SetInClosed(true);

		//Get all adjacent tiles
		for (int x = -1; x <= 1; x++)
		{
			for (int y = -1; y <= 1; y++)
			{
				XMINT2 currentTileCoord = current->GetTileCoord();
				if (!(x == 0 && y == 0) && level->withinBounds(currentTileCoord.x + x, currentTileCoord.y + y))
				{
					int xc = currentTileCoord.x + x;
					int yc = currentTileCoord.y + y;
					child = level->getTile(xc, yc);

					if (child != nullptr && child != current)
					{
						XMINT2 childTileCoord = child->GetTileCoord();

						bool inClosed = current->InClosed();
						bool inOpen = current->InOpen();

						cout << "Node: " << childTileCoord.x << " " << childTileCoord.y << " inOpen: " << boolalpha << inOpen << " inClosed: " << boolalpha << inClosed << endl;

						if (!inClosed && child->IsWalkable())
						{
							//Check for corners
							//Next vertical not walkable, skip
							//Next horizontal not walkable, skip


							int tentativeG = current->GetG() + 1;

							////Already in open but a better solution found, update it
							//if (inOpen)
							//{
							//	if (child->GetG() > tentativeG)
							//	{
							//		child->SetParent(current);
							//		child->SetG(tentativeG);
							//		child->SetF(child->GetG() + ManhattanDistance(child, end));
							//	}
							//}

							if (!inOpen && current->GetParent() != child)
							{
								openList.push_back(child);
								child->SetInOpen(true);

								child->SetParent(current);
								child->SetG(tentativeG);
								child->SetF(child->GetG() + ManhattanDistance(child, end));
								//child->SetF(child->GetG() + EuclideanDistance(child, end));
								//child->SetF(child->GetG() + ChebyshevDistance(child, end));
							}
						}
					}
				}
				else
				{
					//Utanför eller current själv?
					break;
				}
			}
		}

		//Add to the counter
		n++;
	}

	//Reset open/closed in tiles
	for (i = openList.begin(); i != openList.end(); ++i)
	{
		(*i)->SetInOpen(false);
	}
	for (i = closedList.begin(); i != closedList.end(); ++i)
	{
		(*i)->SetInClosed(false);
	}

	//Retrace the path from the end to start
	while (current->GetParent() != NULL && current != start)
	{
		XMINT2 currentCoord = current->GetTileCoord();
		path.push_back(XMFLOAT3((float)currentCoord.x, 0.0f, (float)currentCoord.y));
		current = current->GetParent();
		n++;
	}

	return path;
}
