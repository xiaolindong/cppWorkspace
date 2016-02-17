/*
 * all_triple_formul_solver.h
 *
 *  Created on: April 14, 2014
 *      Author: eia2
 */

#ifndef ALL_TRIPLE_FORMUL_SOLVER_H_
#define ALL_TRIPLE_FORMUL_SOLVER_H_


#include "ranking.h"
typedef IloArray<IloBoolVarArray> BoolVarMatrix;
typedef IloArray<BoolVarMatrix> BoolVarCube;
typedef IloArray<IloNumVarArray> NumVarMatrix;
typedef IloArray<NumVarMatrix> NumVarCube;



class AllTripleFormulKemeny
{
	Ranking  &R;
	IloEnv env;
	IloModel ip_model,lp_model,lp_model_iterat;
	IloCplex ip_cplex,lp_cplex,lp_cplex_iterat;
	int *** triple_cost_cube;
	BoolVarCube y;
	NumVarCube y_lp;
	double val_lp, val_ip, rt_lp, rt_ip,val_lp_iterat, rt_lp_iterat;
	public :
	AllTripleFormulKemeny(Ranking& r):
		R(r),ip_model(env),ip_cplex(ip_model),val_lp(-1), val_ip(-1), rt_lp(0), rt_ip(0),
		lp_model(env),lp_cplex(lp_model),y(env,r.Candidates())
	{
		for (int i = 0; i < r.Candidates(); i++)
		{
			y[i] = BoolVarMatrix(env,r.Candidates());
			for (int j = 0; j < r.Candidates(); j++)
				y[i][j] = IloBoolVarArray(env, r.Candidates());

			triple_cost_cube= new int **[r.Candidates()];
			for(int i = 0; i < r.Candidates(); i++)	
			{
				triple_cost_cube[i] = new int *[r.Candidates()];
				for (int j = 0; j < r.Candidates(); j++)
					triple_cost_cube[i][j] = new int [r.Candidates()];

			} 

		}
	}

	AllTripleFormulKemeny(Ranking& r,const char* s):
		R(r),val_lp(-1), val_lp_iterat(-1), rt_lp(0), rt_lp_iterat(0),
		lp_model(env),lp_cplex(lp_model), y_lp(env,r.Candidates()),lp_model_iterat(env), 
        lp_cplex_iterat(lp_model_iterat)
	{
		for (int i = 0; i < r.Candidates(); i++)
		{
			y_lp[i] = NumVarMatrix(env,r.Candidates());
			for (int j = 0; j < r.Candidates(); j++)
				y_lp[i][j] = IloNumVarArray(env, r.Candidates(),0,1,ILOFLOAT);

			triple_cost_cube= new int **[r.Candidates()];
			for(int i = 0; i < r.Candidates(); i++)	
			{
				triple_cost_cube[i] = new int *[r.Candidates()];
				for (int j = 0; j < r.Candidates(); j++)
					triple_cost_cube[i][j] = new int [r.Candidates()];

			} 

		}
	}


	~AllTripleFormulKemeny(){ env.end();};
	void AddConstraints();
	void AddTripleConstraints();
	IloExpr CreateObjFunction();
	void Lp_AddConstraints();
    void Lp_Iterat_AddConstraints();
	void Lp_AddTripleConstraints();
	IloExpr Lp_CreateObjFunction();
	void SolveILP();
	void SolveLP();
	void SolveLPIterat();
    int Exist_Cycle();
	void PrintResults();
	void ComputeTripleCost();
	void InitialTripleCostCube();
	void PrintTripleCostCube();
	double Manual_Obj_Computation(IloCplex cplex);
	double GetValIP() const {return val_ip;}
	double GetValLP() const {return val_lp;}
	double GetRTLP() const {return rt_lp;}
	double GetRTIP() const {return rt_ip;}
	double GetValLPIterat() const {return val_lp_iterat;}
	double GetRTLPItera() const {return rt_lp_iterat;}
};




#endif /*  ALL_TRIPLE_FORMUL_SOLVER_H_*/
