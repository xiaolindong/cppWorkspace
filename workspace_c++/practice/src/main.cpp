#include<iostream>
#include "EdgeManager.h"
#include "CycleManager.h"
#include<vector>
#include "master_header.h"
using namespace std;

struct dicycle{
	int i , j, k;
	dicycle(int s=0,int t=0, int w=0):i(s), j(t), k(w){}
};

void InitialConstraintManager(vector<vector<dicycle> >& ConstManager, EdgeManager& Edge, CycleManager& Cycle,int problem_size, int size_edge){
	for(int i=0; i < size_edge; i++ ){
		pair<int,int> e = Edge.GetEdgeByLabel(i);
		//cout << endl << "e is = (" << e.first << " , " << e.second << " ) " << endl; 
		int temp[3];
		temp[0]= e.first;
		temp[1] = e.second;
		for (int k = 0; k < problem_size; k++)
			if(k != temp[0] && k!=temp[1]){
				temp[2]	= k;
				int j =0;
				while (j < 3){
					if((temp[j%3] < temp[(j+1)%3] && temp[(j+1)%3] < temp[(j+2)%3]) || 
							(temp[j%3] > temp[(j+1)%3] && temp[(j+1)%3] > temp[(j+2)%3])){
						dicycle d(temp[j%3],temp[(j+1)%3],temp[(j+2)%3]);
						//cout << "i is : " << i << endl;
						ConstManager[i].push_back(d); 
						//cout << "size of const manager is sis" << ConstManager.size() << endl;
						break;
					}
					else 
						j++;
				}
			}

	}
	//cout <<"constananna size is  " << ConstManager.size() << endl;

}
void Print(vector<vector<dicycle> >& ConstManager,EdgeManager& Edge){
	vector<vector< dicycle> >::const_iterator it = ConstManager.begin();
	//cout << "size constmanager " << ConstManager.size() << endl;
	int i =0;
	for( it = ConstManager.begin(); it!=ConstManager.end(); it++){
		pair<int,int> e = Edge.GetEdgeByLabel(i++);
		cout << "y_( " << e.first << " , " << e.second << " )  " ;
		for(vector<dicycle>::const_iterator it_j = it->begin(); it_j != it->end(); it_j++ ){
			cout << "+ z_( " << it_j->i << " , " << it_j->j << " , " << it_j->k << " )  " ;  
		}
		cout << " <= 0 " << endl;

	}

}

int main(){
	EdgeManager Edge;
	CycleManager Cycle;
	int problem_size = 4;
	Edge.Initial(problem_size);
	Edge.Print();
	Cycle.Initial(problem_size);
	//Cycle.Print();
	vector<vector<dicycle> > ConstManager;
	int size_edge = problem_size*(problem_size-1);
	ConstManager.resize(size_edge);
	//for(int i =0; i<size_edge;i++)
	//	ConstManager[i].resize(problem_size-2);
	InitialConstraintManager(ConstManager, Edge, Cycle,problem_size,size_edge);
	Print(ConstManager, Edge);
	cout << setw(10) << "pair" 
		 << setw(10)  << "loos" << endl;
	return 0;
}

