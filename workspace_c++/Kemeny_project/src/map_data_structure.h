/*
 * Created on : Mar 25, 2014
 *	Author : Ehsan
 *
 */

#ifndef MAP_DATA_STRUCTURE_H_
#define MAP_DATA_STRUCTURE_H_


#include <map>
#include <assert.h>
#include "master_header.h"

using namespace std;

class MapDataStruct
{

private:

	map< pair<int, pair<int, int> >, int> TripleToLabel;
	map<int, pair<int, pair<int, int> > > LabelToTriple;

public:
	void AddTriple(int i, int j, int k, int label);
	int GetLabel(int i, int j, int k);	
	pair<int, pair<int, int> > GetTriple (int i);
	int TripleCount();
	void PrintMapData();	
};


#endif /* MAP_DATA_STRUCTURE_H_ */
