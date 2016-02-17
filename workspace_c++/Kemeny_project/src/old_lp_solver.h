/*
 * old_lp_solver.h
 *
 *  Created on: April 5, 2014
 *      Author: eia2
 */

#ifndef OLD_LP_SOLVER_H_
#define OLD_LP_SOLVER_H_


#include "ranking.h"
typedef IloArray<IloNumVarArray> NumVarMatrix;


class OldLPSolver
{
	Ranking  &R;
	IloEnv env;
	IloModel model,lp_model;
	IloCplex cplex,lp_cplex;
	NumVarMatrix x;
public :
	OldLPSolver(Ranking& r):
		R(r),model(env),cplex(model),x(env,r.Candidates()),
		lp_model(env),lp_cplex(lp_model)
		{

			for (int i = 0; i < R.Candidates(); i++)
				x[i]= IloNumVarArray(env, r.Candidates(),0.0, 1.0);
		}

	~OldLPSolver(){ env.end();};
	void AddConstraints();
	void AddCycleConstraints();
	void IterativelyAddCycleConstraints();
	IloExpr CreateObjFunction();
	void SolveILP();
	void SolveLP();
	void Seperation_3_cycles(vector<pair<pair<int, int>,int > >& cycles);
	void AddViolated_3_Cycles(vector<pair<pair<int, int>,int > >& cycles);
	void IterativelyAddCycleConstraints_demo();
	void SolveLPIteratively();
	void PrintSolutionAssignment();
	double Manual_Obj_Computation(IloCplex cplex);
};




#endif /* OLD_ LP_SOLVER_H_ */
