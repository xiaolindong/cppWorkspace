/*
 * CycleManager.h
 *
 *  Created on: Mar 29, 2012
 *      Author: eia2
 */

#ifndef CYCLEMANAGER_H_
#define CYCLEMANAGER_H_
#include <map>
#include<assert.h>
using namespace std;

typedef pair<pair<int,int>, int> cycle;
class CycleManager{
private:
	map<cycle,int> CycleToLabel;
	map<int,cycle > LabelToCycle;

public:
	void Initial(int n);
    int GetLabel (int i, int j,int k);
    cycle GetCycleByLabel (int label);
    map<cycle, int>::iterator GetIterator ();
    map<cycle,int>::iterator EndIterator ();
    bool AddCycle (int i, int j, int k, int Label);
	int CycleCount();
	void Print();
};


void CycleManager::Initial(int n){
	int l = 0;
	for(int i=0; i < n ; i++)
		for(int j=0 ; j < n; j++)
			for(int k =0; k < n ; k++){
				if(i!=j && j!=k && k!=i)
					if (!(( j >i && j > k) || ( j < i && j < k ))){
						bool flag = AddCycle(i, j,k ,l);
						if (flag)
							l = l+1;
						flag = AddCycle(k,j,i,l);
						if (flag)
							l=l+1;
				}
			}
}

bool CycleManager::AddCycle (int i, int j, int k , int Label)
{
	if (CycleToLabel.find(make_pair(make_pair(i,j),k)) != CycleToLabel.end() )
		return false;
	CycleToLabel.insert (make_pair (
							       make_pair (
											 make_pair(i, j),
											 k),
			                       Label
			                       )
			             );
	LabelToCycle.insert (make_pair (
								Label,
								make_pair (
											make_pair(i, j),
										  k)
			                       )
			             );
	return true;

}

int CycleManager::GetLabel (int i, int j,int k)
{
	if (CycleToLabel.find (make_pair(make_pair (i, j),k)) != CycleToLabel.end ())
		return CycleToLabel.find (make_pair(make_pair (i, j),k))->second;
	return -1;

}

cycle CycleManager::GetCycleByLabel (int label)
{
	if (LabelToCycle.find (label)!= LabelToCycle.end ())
		return LabelToCycle.find (label)->second;
	assert (false);
}

int CycleManager::CycleCount ()
{
	return LabelToCycle.size ();

}

map<cycle,int>::iterator CycleManager::GetIterator ()
{
  return CycleToLabel.begin ();

}

map<cycle,int>::iterator CycleManager::EndIterator ()
{
  return CycleToLabel.end();

}
void CycleManager::Print(){
	for(map<cycle,int>::iterator it = GetIterator(); it !=EndIterator(); it ++)
		cout << "( " << it->first.first.first << " , " << it->first.first.second << ", " << it->first.second << " ) -> ( " << it->second << " )" <<  endl;	
	for(map<int, cycle>::iterator it = LabelToCycle.begin(); it !=LabelToCycle.end(); it ++)
		cout <<"( " <<it->first << " ) ->  ( " << it->second.first.first << " , " << it->second.first.second << " , " << it->second.second << " ) "<<  endl;	



}

#endif /* EDGEMANAGER_H_ */
