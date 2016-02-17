/*
 * old_ilp_solver.h
 *
 *  Created on: March 28, 2014
 *      Author: eia2
 */

#ifndef OLD_ILP_SOLVER_H_
#define OLD_ILP_SOLVER_H_


#include "ranking.h"
#include "Functions.h"
typedef IloArray<IloBoolVarArray> BoolVarMatrix;
typedef IloArray<IloNumVarArray> NumVarMatrix;
class OldILPSolver
{    
   Ranking  &R;
	IloEnv env;
	IloModel ip_model,lp_model,lp_model_iterat;
	IloCplex ip_cplex,lp_cplex,lp_cplex_iterat;
	BoolVarMatrix x;
	NumVarMatrix x_lp;
	double val_ip,val_lp,val_lp_iterat;
	double rt_lp, rt_ip,rt_lp_iterat;
	int n_iterat_constraint,n_lp_constraint;
public :
	OldILPSolver(Ranking& r):
		R(r),ip_model(env),ip_cplex(ip_model),x(env,r.Candidates()), val_ip(-1),val_lp(-1),rt_lp(0),rt_ip(0),
		val_lp_iterat(0),lp_model(env),lp_cplex(lp_model),n_iterat_constraint(0),n_lp_constraint(0),
		x_lp(env,r.Candidates())
		{

			for (int i = 0; i < R.Candidates(); i++)
				x[i]= IloBoolVarArray(env, r.Candidates());
			for (int i = 0; i < R.Candidates(); i++)
				x_lp[i]= IloNumVarArray(env, r.Candidates(),0,1,ILOFLOAT);
		}
	OldILPSolver(Ranking& r, const char* s):
		R(r),x_lp(env,r.Candidates()),val_lp(-1),rt_lp(0),
		val_lp_iterat(0),rt_lp_iterat(0),lp_model(env),lp_cplex(lp_model),lp_model_iterat(env)
		, lp_cplex_iterat(lp_model_iterat),n_iterat_constraint(0),n_lp_constraint(0)
		{
			for (int i = 0; i < R.Candidates(); i++)
				x_lp[i]= IloNumVarArray(env, r.Candidates(),0,1,ILOFLOAT);
		}
	
	~OldILPSolver(){ env.end();};
	void AddConstraints();
	void AddCycleConstraints();
	IloExpr CreateObjFunction();
	void Lp_AddConstraints();
	void Lp_AddCycleConstraints();
  	IloExpr Lp_CreateObjFunction();
	void SolveILP();
	void SolveLPIterat();
	void SolveLP();
	void SolveDualbyDRP(vector<pair<int,int> > J );
	int Exist_Violated_3_Dicycle();

	void PrintResultsIP();
	void PrintResultsLP();
	void PrintIteratLPResults();
	void PrintXVars(IloCplex cplex, BoolVarMatrix x, const char* s);
	double Manual_Obj_Computation(IloCplex cplex);
	double GetValLPIterat() const {return val_lp_iterat;}
	double GetValIP() const {return val_ip;}
	double GetValLP() const {return val_lp;}
	double GetRTLP() const {return rt_lp;}
	double GetRTIP() const {return rt_ip;}
	double GetRTLPIterat() const {return rt_lp_iterat;}
	int GetnConstraintLPIterat() const {return n_iterat_constraint;}
	int GetnConstraintLP() const {return n_lp_constraint;}
    double SolveLP_Ref_Call(IloModel model,IloCplex heur_lp_cplex, NumVarMatrix x , int i, int j, int val);
    double SolveLP(IloModel model,IloCplex lp_cplex, NumVarMatrix x , int i, int j, int val);
    double SolveLP(IloModel model, NumVarMatrix x , vector<pair<int, int> > fixings);
	void DualRestrictedPrimal();

	void print();
    void Print_Constraints(IloCplex cplex, NumVarMatrix x);

	// target and local cut functions
	void GenerateTargetCuts();
	vector<int> Mapping(int subspace_size);
	vector<int> Mapping();
    vector<vector<vector<int> > > GenerateExtremPointPCV(vector<int>& vars, int subspace_size, int nFeasPoints);
    vector<vector<double> > GenerateQ(vector<vector<vector<int> > >& points, int subspace_size, int nFeasPoints);
	bool Separation( vector<vector<vector<int> > >& points,vector<vector<double> >& q, vector<vector<int> >& x,
									vector<vector<double> >& hyper_plane_norm, int subspace_size, int nFeasPoints);
//	void Heuristics();
	bool Exact(vector<vector<double> >& hyper_plane_norm, vector<vector<vector<int> > >& points, int subspace);
	
	//test functions
	void TestLPIterat();
	void Print_Constraints();
	void AddOne_3_Dicycle_Constraint(int i, int j, int k);
};
#endif /* OLD_ ILP_SOLVER_H_ */
