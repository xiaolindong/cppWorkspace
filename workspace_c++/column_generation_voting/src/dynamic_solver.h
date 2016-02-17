/*
 * dynamic_solver.h
 *
 *  Created on: Jan 14, 2014
 *      Author: eia2
 */

#ifndef DYNAMIC_SOLVER_H_
#define DYNAMIC_SOLVER_H_



#include "master_header.h"
#include "election_class.h"



class DynamicSolver
{
	election& E;
	int** dynamic_table;
	char** opt_path;
	int* voters;
	bool divide;
public :
	DynamicSolver(election& e): E(e), divide(E.Voters()%E.K())
	{
		dynamic_table  = new int* [E.Candidates()];
		opt_path = new char* [E.Candidates()];
		for(int i = 0; i < E.Candidates(); i++)
		{
			dynamic_table[i] = new int [E.K()];
			opt_path[i] = new char [E.K()];
		}
		voters = new int [E.Voters()];
	}
	~DynamicSolver();
	int V(int c, int beg_indx);
	void FillTable();
	void LexicoSortOnVoters();
	int OptimalDynamic() {return dynamic_table[E.Candidates()-1][E.K()-1]; }
	int** Table() const {return dynamic_table;}
	int Candid() const  {return E.Candidates();}
	int Voters() const  {return E.Voters();}
	int K()    const  {return E.K();}
	void PrintOptPath();
	void InitialOptPath();
	void InitialDynTable();
	void ExtractPrintDynamicSolution();
};
ostream& operator <<(ostream& out ,const DynamicSolver& d);


#endif /* DYNAMIC_SOLVER_H_ */
