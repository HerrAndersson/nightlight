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

struct Node
{
	int x, y;

	//f = g + h, g = sum of all costs to get here, h = heuristic
	int f, g;

	Node* parent;

	Node(int x, int y)
	{
		this->x = x;
		this->y = y;
		f, g = 0;
		parent = nullptr;
	}

	Node()
	{
		x, y, f, g  = 0;
		parent = nullptr;
	}

	bool operator==(const Node* n)
	{
		if (this->x == n->x && this->y == n->y)
			return true;
		else 
			return false;
	}
};

static int ManhattanDistance(Node* n1, Node* n2) //Cheap, less accurate
{
	return (abs(n1->x - n2->x) + abs(n1->y - n2->y));
}

static int EuclideanDistance(Node* n1, Node* n2) //Expensive, more accurate
{
	return (int)sqrt((pow((n1->x - n2->x), 2) + pow((n1->y - n2->y), 2)));
}

static int ChebyshevDistance(Node* n1, Node* n2)
{
	return max(abs(n1->x - n2->x), abs(n1->y - n2->y));
}

static Node GetPositionFromCoord(float x, float z, int tileWidth)
{
	return Node((int)floor(x / tileWidth), (int)floor(z / tileWidth));
}

static vector<Node*> aStar(Level* level, int tileSize, XMINT2 startPosXZ, XMINT2 endPosXZ) //Start and end are tile positions in the grid.
{
	vector<Node*> path;
	Node* start = new Node(startPosXZ.x, startPosXZ.y);
	Node* end = new Node(endPosXZ.x, endPosXZ.y);
	Node* current = start;

	vector<Node*> closedset;
	list<Node*> openset;
	openset.push_back(start);

	vector<Node*> adjList;

	start->f = start->g + ManhattanDistance(start, end);

	while (!openset.empty())
	{
		cout << "openset size : " << openset.size() << endl;
		cout << "closedset size : " << closedset.size() << endl;

		for each (Node* n in openset)
		{
			if (n->f < current->f)
				current = n;
		}

		cout << "Current: " << current->x << " " << current->y << endl;

		if (current->x == end->x && current->y == end->y)
			break;

		closedset.push_back(current);
		openset.remove(current);

		adjList.clear();
		adjList.push_back(new Node(current->x - 1, current->y - 1));
		adjList.push_back(new Node(current->x - 1, current->y));
		adjList.push_back(new Node(current->x - 1, current->y + 1));
		adjList.push_back(new Node(current->x + 1, current->y - 1));
		adjList.push_back(new Node(current->x + 1, current->y));
		adjList.push_back(new Node(current->x + 1, current->y + 1));
		adjList.push_back(new Node(current->x, current->y + 1));
		adjList.push_back(new Node(current->x, current->y - 1));

		for each (Node* n in adjList)
		{
			bool inOpen = false;
			bool inClosed = false;

			for each (Node* x in closedset)
			{
				if (n->x == x->x && n->y == x->y)
				{
					inClosed = true;
					break;
				}
			}

			for each (Node* x in openset)
			{
				if (n->x == x->x && n->y == x->y)
				{
					inOpen = true;
					break;
				}
			}

			cout << "Node: " << current->x << " " << current->y << " inOpen: " << boolalpha << inOpen << " inClosed: " << boolalpha << inClosed << endl;

			//if (!(find(closedset.begin(), closedset.end(), &n) != closedset.end()))
			if (!inClosed)
			{
				int tentativeG = current->g + 1;

				if (!inOpen || tentativeG < n->g)
				{
					Tile* tile = nullptr;
					
					if (level->withinBounds(n->x, n->y))
						tile = level->getTile(n->x, n->y);

					if (tile != nullptr)
					{
						if (tile->getTileIsWalkable())
						{
							n->parent = current;
							n->g = tentativeG;
							n->f = n->g + ManhattanDistance(n, end);

							if (!inOpen)
								openset.push_back(n);
						}
					}
				}
			}
		}
	}

	while (current != start)
	{
		cout << current->x << " " << current->y << endl;
		path.push_back(current);
		current = current->parent;
	}

	return path;
}
