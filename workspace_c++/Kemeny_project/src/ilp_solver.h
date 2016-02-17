/*
 * ilp_solver.h
 *
 *  Created on: Jan 14, 2014
 *      Author: eia2
 */

#ifndef ILP_SOLVER_H_
#define ILP_SOLVER_H_



#include "master_header.h"
#include "election_class.h"



class ILPSolver
{
	election &E;
	IloEnv env;
	IloModel model,lp_model;
	IloCplex cplex,lp_cplex;
	IloBoolVarArray y;
	BoolVarMatrix x;
public :
	ILPSolver(election& e):
		E(e),model(env),cplex(model),y(env,E.Candidates()),x(env,E.Candidates()),
		lp_model(env),lp_cplex(lp_model)
		{

			for (int i = 0; i < E.Candidates(); i++)
				x[i]= IloBoolVarArray(env, E.Voters());
		}

	~ILPSolver(){ env.end();};
	void AddConstraints();
	IloExpr CreateObjFunction();
	void SolveILP();
	void SolveLP();
	void PrintSolutionAssignment();
};




#endif /* ILP_SOLVER_H_ */
