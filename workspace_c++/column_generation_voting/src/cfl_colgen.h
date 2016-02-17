/*
 * cfl_colgen.h
 *
 *  Created on: May 29, 2014
 *      Author: eia2
 */

#ifndef CFL_COLGEN_H_
#define CFL_COLGEN_H_

#include "master_header.h"
#include "election_class.h"




class CFLColGenSolver
{
	election& E;
	IloEnv env;
	IloModel eOpt;
	IloCplex eSolver;
	IloNumVarArray Cut;
	vector<vector<int> > subsets;
	int size_subsets;
	double val_lp,rt_lp;
	/*
	 * for (int i = 0; i < n; i++)
				subsets[i].assign(E.K(),0);
			size_subsets = n;*/

public:

	CFLColGenSolver(election &E2, int n):
		E(E2),eOpt(env),eSolver(eOpt), Cut(env), size_subsets(n),val_lp(0),rt_lp(0){}

	~CFLColGenSolver();

	double SolveLP();
	vector<int> SubProblemSolving(IloNumArray price, vector<int>& assignment, double& reduced_cost);
	
	void InitSubSets();
	int nSubsets() const {return size_subsets;}
	vector<vector<int> > SubSets() const {return subsets;}
	friend ostream& operator <<(ostream& out,  const CFLColGenSolver& s);
	int K()const{ return E.K();}
	void AddCuts();
	double GetValLP() const {return val_lp;}
	double GetRTLP() const {return rt_lp;}
	void PrintResult();	

	class subsetIterator;
	friend class subsetIterator;



};



#endif /* CFL_COLGEN_H_ */
