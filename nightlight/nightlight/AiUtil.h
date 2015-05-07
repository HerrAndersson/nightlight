#pragma once
#include "GameObject.h"
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include <deque>
#include "Tile.h"
#include <list>

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

static vector<Node*> aStar(vector<vector<Tile*>> tileGrid, int tileSize, XMINT2 startPosXZ, XMINT2 endPosXZ) //Start and end are tile positions in the grid.
{
	vector<Node*> path;	
	Node* start = new Node(startPosXZ.x, startPosXZ.y);
	Node* end = new Node(endPosXZ.x, endPosXZ.y);
	Node* current = nullptr;

	vector<Node*> closedset;
	list<Node*> openset;
	openset.push_back(start);

	vector<Node> adjList;

	start->f = start->g + ManhattanDistance(start, end);

	while (!openset.empty())
	{
		current = openset.front();
		for each (Node* n in openset)
		{
			if (n->f < current->f)
				current = n;
		}

		if (current == end)
			break;

		closedset.push_back(current);
		openset.remove(current);

		adjList.clear();
		adjList.push_back(Node(current->x - 1, current->y - 1));
		adjList.push_back(Node(current->x - 1, current->y));
		adjList.push_back(Node(current->x - 1, current->y + 1));
		adjList.push_back(Node(current->x + 1, current->y - 1));
		adjList.push_back(Node(current->x + 1, current->y));
		adjList.push_back(Node(current->x + 1, current->y + 1));
		adjList.push_back(Node(current->x, current->y + 1));
		adjList.push_back(Node(current->x, current->y - 1));

		for each (Node n in adjList)
		{
			if (!(find(closedset.begin(), closedset.end(), &n) != closedset.end()))
			{
				int tentativeG = current->g + 1;
				bool inOpen = find(openset.begin(), openset.end(), &n) != openset.end();

				if (!inOpen || tentativeG < n.g)
				{
					if (n.x >= 0 && n.x < (signed)tileGrid.size() && n.y >= 0 && n.y < (signed)tileGrid[n.x].size() && tileGrid.at(n.x).at(n.y)->getTileIsWalkable())
					{
						Node* x = new Node(n.x, n.y);
						x->parent = current;
						x->g = tentativeG;
						x->f = x->g + ManhattanDistance(x, end);

						if (!inOpen)
							openset.push_back(x);
						else
							delete x;
					}
				}
			}
		}
	}

	while (current != start)
	{
		//cout << current->x << " " << current->y << endl;
		path.push_back(current);
		current = current->parent;
	}

	return path;
}
