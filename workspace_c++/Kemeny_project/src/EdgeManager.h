/*
 * EdgeManager.h
 *
 *  Created on: Mar 29, 2012
 *      Author: eia2
 */

#ifndef EDGEMANAGER_H_
#define EDGEMANAGER_H_
#include "master_header.h"

using namespace std;
class EdgeManager{
private:
	map<pair<int, int>, int> EdgeToLabel;
	map<int, pair<int, int> > LabelToEdge;

public:
	//~EdgeManager();
	void Initial(int n);
    int GetLabel (int u, int v);
    pair<int, int> GetEdgeByLabel (int label);
    map<pair<int, int>, int>::iterator GetIterator ();
    map<pair<int, int>, int>::iterator EndIterator ();
    bool AddEdge (int u, int v, int Label);
	int EdgeCount();
	void Print();
};

#endif /* EDGEMANAGER_H_ */
