/*
 * CycleManager.h
 *
 *  Created on: Mar 29, 2012
 *      Author: eia2
 */

#ifndef CYCLEMANAGER_H_
#define CYCLEMANAGER_H_
#include "master_header.h"
using namespace std;
typedef pair<pair<int,int>, int> cycle;

class CycleManager{
private:
	map<cycle,int> CycleToLabel;
	map<int,cycle > LabelToCycle;

public:
	//~CycleManager();
	void Initial(int n);
    int GetLabel (int i, int j,int k);
    cycle GetCycleByLabel (int label);
    map<cycle, int>::iterator GetIterator ();
    map<cycle,int>::iterator EndIterator ();
    bool AddCycle (int i, int j, int k, int Label);
	int CycleCount();
	void Print();
};

#endif /* EDGEMANAGER_H_ */
