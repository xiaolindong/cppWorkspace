/*
 * master_header.h
 *
 *  Created on: Jan 14, 2014
 *      Author: eia2
 */

#ifndef MASTER_HEADER_H_
#define MASTER_HEADER_H_



#include <ilcplex/ilocplex.h>
#include <iostream>
#include <fstream>
#include <ostream>
#include <cmath>
#include <vector>
#include <set>
#include <assert.h>
#include <map>
#include <time.h>
#include <sstream>
#include <queue>
#include <algorithm>
#include <list>
#include "CycleManager.h"
#include "EdgeManager.h"
//#include <boost/bimap.hpp>
//#include <unordered_set>

enum solve_status {fully, restricted};
using namespace std;
ILOSTLBEGIN



struct dicycle{
	int i , j, k;
	dicycle(int s=0,int t=0, int w=0):i(s), j(t), k(w){}
};



struct kerlin_struct{
	pair<int, int> p;
	int gain;
	kerlin_struct(pair<int,int> a, int g):gain(g){
		if (a.first < a.second){
			p.first = a.first ;
			p.second = a.second;
		}
		else if (a.first > a.second)
			p.first = a.second;
			p.second = a.first;
	}
	bool operator==(kerlin_struct a) const{
		    if((p.first==a.p.first && p.second == a.p.second) ||
				(p.second == a.p.first && p.first == a.p.second)) return true;
			    else return false;
	}
	bool operator <(kerlin_struct a) const{
		if ((p.first < a.p.first) || 
				((p.first == a.p.first) &&(p.second < a.p.second)))
			return true;
			else return false;	
	}
};




struct TS{
	int a;
	vector<int> v;
	bool operator<(const TS& rhs) const{
		return a < rhs.a;
	}
	TS(int val, vector<int> vec):a(val), v(vec){};
};


struct  Solution
{
	double best_solution_value;
	double current_solution_value;
	vector<vector<int> > best_solution;
	vector<vector<int> > current_solution;
	Solution(int n){
		best_solution.resize(n);
		for(int i = 0; i < n; i++)
			best_solution[i].resize(n);
		current_solution.resize(n);
		for(int i = 0; i < n; i++)
			current_solution[i].resize(n);
	}
};


#endif /* MASTER_HEADER_H_ */
