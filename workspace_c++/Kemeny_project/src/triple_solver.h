/*
 * triple_solver.h
 *
 *  Created on: April 14, 2014
 *      Author: eia2
 */

#ifndef TRIPLE_SOLVER_H_
#define TRIPLE_SOLVER_H_


#include "ranking.h"
typedef IloArray<IloBoolVarArray> BoolVarMatrix;
typedef IloArray<BoolVarMatrix> BoolVarCube;

class TripleFormulKemeny
{
	Ranking  &R;
	IloEnv env;
	IloModel ip_model,lp_model;
	IloCplex ip_cplex,lp_cplex;
	BoolVarMatrix x;
	BoolVarCube y;
	double val_lp, val_ip, rt_lp, rt_ip;
public :
	TripleFormulKemeny(Ranking& r):
		R(r),ip_model(env),ip_cplex(ip_model),x(env,r.Candidates()),val_lp(-1), val_ip(-1), rt_lp(0), rt_ip(0),
		lp_model(env),lp_cplex(lp_model), y(env,r.Candidates())
		{

			for (int i = 0; i < r.Candidates(); i++)
				x[i]= IloBoolVarArray(env, r.Candidates());
			for (int i = 0; i < r.Candidates(); i++)
			{
				y[i] = BoolVarMatrix(env,r.Candidates());
				for (int j = 0; j < r.Candidates(); j++)
					y[i][j] = IloBoolVarArray(env, r.Candidates());
			}
		}

	~TripleFormulKemeny(){ env.end();};
	void AddConstraints();
	void AddTripleConstraints();
	IloExpr CreateObjFunction();
	void SolveILP();
	void SolveLP();
	void PrintResults();
	double Manual_Obj_Computation(IloCplex cplex);
	double GetValIP() const {return val_ip;}
	double GetValLP() const {return val_lp;}
	double GetRTLP() const {return rt_lp;}
	double GetRTIP() const {return rt_ip;}
	void Test_Triple_Violation(ofstream& file);
};




#endif /* TRIPLE_SOLVER_H_ */
