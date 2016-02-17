
#include "EdgeManager.h"
void EdgeManager::Initial(int n){
	int l = 0;
	for(int i=0; i < n ; i++)
		for(int j=i+1 ; j < n; j++){
			bool flag = AddEdge(i, j, l);
			if(flag)
				++l;
			flag = AddEdge(j,i,l);
			if(flag)
				++l;
		}
}

bool EdgeManager::AddEdge (int u, int v, int Label)
{
	if (EdgeToLabel.find (make_pair (u, v))!= EdgeToLabel.end ())
	{
		/*if (EdgeToLabel.find (make_pair (u, v))!= EdgeToLabel.end ())
		 *			assert (EdgeToLabel.find (make_pair (u, v))->second== Label);
		 *
		 *					if (EdgeToLabel.find (make_pair (v, u))!= EdgeToLabel.end ())
		 *								assert (EdgeToLabel.find (make_pair (v, u))->second== Label);
		 *										*/
		return false;

	}

	EdgeToLabel.insert (make_pair (
				make_pair (u, v),
				Label
				)
			);
	LabelToEdge.insert (make_pair (
				Label,
				make_pair (u, v)
				)
			);
	return true;

}

int EdgeManager::GetLabel (int u, int v)
{
	if (EdgeToLabel.find (make_pair (u, v))!= EdgeToLabel.end ())
		return EdgeToLabel.find (make_pair (u, v))->second;

	if (EdgeToLabel.find (make_pair (v, u))!= EdgeToLabel.end ())
		return EdgeToLabel.find (make_pair (v, u))->second;

	return -1;

}

pair<int, int> EdgeManager::GetEdgeByLabel (int label)
{
	if (LabelToEdge.find (label)!= LabelToEdge.end ())
		return LabelToEdge.find (label)->second;
	assert (false);
}

int EdgeManager::EdgeCount ()
{
	return LabelToEdge.size ();

}

map<pair<int, int>, int>::iterator EdgeManager::GetIterator ()
{
	return EdgeToLabel.begin ();

}

map<pair<int, int>, int>::iterator EdgeManager::EndIterator ()
{
	return EdgeToLabel.end();

}
void EdgeManager::Print(){
	for(map<pair<int,int>,int>::iterator it = GetIterator(); it !=EndIterator(); it ++)
		cout << "( " << it->first.first << " , " << it->first.second << " ) -> ( " << it->second << " )" <<  endl;	
	for(map<int, pair<int,int> >::iterator it = LabelToEdge.begin(); it !=LabelToEdge.end(); it ++)
		cout <<"( " <<it->first << " ) ->  ( " << it->second.first << " , " << it->second.second << " ) "<<  endl;	



}

