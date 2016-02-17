/*
 * Created on : Mar 25, 2014
 *	Author : Ehsan
 *
 */

#include"map_data_structure.h"


void MapDataStruct::AddTriple(int i, int j, int k, int label)
{
	if (TripleToLabel.find( make_pair(i, make_pair(j, k))) != TripleToLabel.end())
		return;

	if (LabelToTriple.find (label) != LabelToTriple.end())
		return;

	LabelToTriple.insert(make_pair(label , make_pair(i, make_pair(j, k))));
	TripleToLabel.insert(make_pair(make_pair(i, make_pair(j, k) )  , label ));

}



int MapDataStruct::GetLabel(int i, int j, int k)
{

	if (TripleToLabel.find(make_pair(i, make_pair(j,k))) != TripleToLabel.end())
		return TripleToLabel.find(make_pair(i, make_pair(j,k)))->second;  
	return -1;
}




pair<int , pair<int, int> > MapDataStruct:: GetTriple(int i)
{
	if (LabelToTriple.find(i) != LabelToTriple.end())
		return LabelToTriple.find(i)->second;
	assert(false);
}

int MapDataStruct::TripleCount()
{

	return TripleToLabel.size();
}


void MapDataStruct::PrintMapData()
{
	cout <<"The Triple to label map structure is :" <<endl;
	map<pair<int, pair<int, int> >, int>::iterator it;
	for (it = TripleToLabel.begin(); it != TripleToLabel.end(); it++)
	{
		cout << it->first.first << " " ;
		cout << it->first.second.first << " " ;
		cout << it->first.second.second << " " ;
		cout << " --> ";
		cout << it->second << " ";
		cout << endl;
	}
/*	
	cout << "The Label to Triple map structure is :" << endl;
	map<int, pair <int, pair<int, int> > >::iterator itr;
	for (itr = LabelToTriple.begin(); itr != LabelToTriple.end(); itr++)
	{
		cout << itr->first << " ";
		cout << " --> ";
		cout << itr->second.first << " ";
		cout << itr->second.second.first << " ";
		cout << itr->second.second.second << " ";
		cout << endl;

	}
*/


}
