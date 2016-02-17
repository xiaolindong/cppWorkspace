/*
 * node.h
 *
 *  Created on: Mar 23, 2012
 *      Author: eia2
 */

#ifndef NODE_H_
#define NODE_H_
#include <vector>

using namespace std;

class Node
{
public:
	int id;
	bool flag;
	vector<Node>links;
	Node()
	{
		id = 0;
		flag = false;
	}
	Node(int i)
	{
		id = i;
	}
	void AddNeighbor (int v)
	{
		links.push_back (v);
	}
	void PrintNeighbor ()
	{
		vector<Node>::iterator p;
		cout << " id is :" << this->id << endl;
		for (p = links.begin(); p != links.end(); p++)
		{
			cout << p->id << " , ";
		}
		cout << endl;
	}
};




#endif /* NODE_H_ */
