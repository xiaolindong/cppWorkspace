/*
 * new_ilp_solver.h
 *
 *  Cretated on : Mar 25, 2014
 * 	Author : Ehsan
 */
#ifndef NEW_ILP_SOLVER_H_
#define NEW_ILP_SOLVER_H_

#include "ranking.h"

typedef IloArray<IloBoolVarArray> BoolVarMatrix;


class NewILPSolver
{

	Ranking& R;
	IloEnv env;
	IloModel ip_model, lp_model;
	IloCplex ip_cplex, lp_cplex;
	BoolVarMatrix y;
	IloBoolVarArray d;
public: 
	NewILPSolver (Ranking& r) : 
		R(r), ip_model(env), ip_cplex(ip_model),
		lp_model(env),lp_cplex(lp_model),
		d(env,r.GetRowSizeDiffMatrix()),y(env,r.Candidates())
	{
		for (int i = 0; i < r.Candidates(); i++)
			y[i] = IloBoolVarArray(env,r.Candidates());
	}

	~NewILPSolver(){env.end();};
	void AddConstraint();
	IloExpr CreatObjFunction();
	void SolveILP();
	void SolveLP();
	void PrintSolutionRanking();
	void TestFeasible();
	void BirkhoffNeumannDecomposition();	
};





#endif /* NEW_ILP_SOLVER_H_ */
