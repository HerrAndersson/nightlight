#pragma once
#include "GameObject.h"
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include <deque>
#include "Tile.h"

using namespace std;
using DirectX::XMFLOAT2;
using DirectX::XMINT2;

struct Node
{
	int x, y;
	int dist;

	Node(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	bool operator==(const Node n)
	{
		if (this->x == n.x && this->y == n.y)
			return true;
		else 
			return false;
	}
};

static int ManhattanDistance(Node n1, Node n2)
{
	return (abs(n1.x - n2.x) + abs(n1.y - n2.y));
}

static int EuclideanDistance(Node n1, Node n2)
{
	return sqrt((pow((n1.x - n2.x), 2) + pow((n1.y - n2.y), 2)));
}

static int ChebyshevDistance(Node n1, Node n2)
{
	return max(abs(n1.x - n2.x), abs(n1.y - n2.y));
}

static Node GetPositionFromCoord(float x, float z, int tileWidth)
{
	return Node((int)floor(x / tileWidth), (int)floor(z / tileWidth));
}

static vector<Node*> aStar(vector<vector<Tile>>* tileGrid, int tileWidth, XMFLOAT2 startPosXZ, XMFLOAT2 endPosXZ)
{
	vector<Node*> path;	
	Node start = GetPositionFromCoord(startPosXZ.x, startPosXZ.y, tileWidth);
	Node end = GetPositionFromCoord(endPosXZ.x, endPosXZ.y, tileWidth);

	vector<Node> closedset;
	deque<Node> openset;
	openset.push_back(start);

	vector<Node> adjList;

	int totalCost = INFINITE;
	int estimatedCost = totalCost + ManhattanDistance(start, end);

	while (!openset.empty())
	{
		Node current = openset.front();
		if (current == end)
			return path;
		//return reconstruct_path(came_from, goal)

		closedset.push_back(current);
		openset.pop_front();

		//lägg till i adj list. Specialfall för borders.
		tileGrid[current.x - 1][current.y - 1];
		tileGrid[current.x - 1][current.y];
		tileGrid[current.x - 1][current.y + 1];
		tileGrid[current.x + 1][current.y - 1];
		tileGrid[current.x + 1][current.y];
		tileGrid[current.x + 1][current.y + 1];
		tileGrid[current.x][current.y - 1];
		tileGrid[current.x][current.y + 1];

		for each (Node x in adjList)
		{
			if (!(find(closedset.begin(), closedset.end(), x) != closedset.end()))
			{
				//int cost = g_score[current] + cost from neighbor;


				//if (!(find(openset.begin(), openset.end(), x) != openset.end()) || cost < g_score[neighbor])

				//	came_from[neighbor] : = current
				//	g_score[neighbor] : = tentative_g_score
				//	f_score[neighbor] : = g_score[neighbor] + heuristic_cost_estimate(neighbor, goal)
				//	if neighbor not in openset
				//		add neighbor to openset
			}

		}
	}

//
//function reconstruct_path(came_from, current)
//
//total_path : = [current]
//
//	while current in came_from :
//		current : = came_from[current]
//		total_path.append(current)

//return total_path






	return path;
}