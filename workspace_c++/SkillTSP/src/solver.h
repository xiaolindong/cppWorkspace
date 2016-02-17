/*
 * old_ilp_solver.h
 *
 *  Created on: March 28, 2014
 *      Author: eia2
 */

#ifndef SOLVER_H_
#define SOLVER_H_


#include "network.h"
#include "../include/master_header.h"
typedef IloArray<IloBoolVarArray> BoolVarMatrix;
typedef IloArray < BoolVarMatrix > ThreeDBoolVarMatrix;

typedef IloArray<IloNumVarArray> NumVarMatrix;
typedef IloArray < NumVarMatrix > ThreeDNumVarMatrix;


class Solver{
    Network &net;
	IloEnv env;
	IloModel ip_model,lp_model;
	IloCplex ip_cplex,lp_cplex;
	ThreeDBoolVarMatrix x;
	ThreeDNumVarMatrix x_lp;
	double val_ip,val_lp;
	double rt_lp, rt_ip;
	int n_iterat_constraint,n_lp_constraint;
public :
	Solver(Network& n):
		net(n),ip_model(env),ip_cplex(ip_model),x(env,n.get_n_workers()), val_ip(-1),val_lp(-1),rt_lp(0),rt_ip(0),
		lp_model(env),lp_cplex(lp_model),x_lp(env,n.get_n_workers())
		{
            for (int i = 0; i < n.get_n_workers(); i++){
				x[i]= BoolVarMatrix(env, n.get_n_jobs());
                for (int j =0; j < n.get_n_jobs(); j++)
                    x[i][j] = IloBoolVarArray(env, n.get_n_jobs());
            }
            for (int i = 0; i < n.get_n_workers(); i++){
                x_lp[i]= NumVarMatrix(env, n.get_n_jobs());
                for (int j =0; j < n.get_n_jobs(); j++)
                    x_lp[i][j] = IloNumVarArray(env, n.get_n_jobs(), 0, 1, ILOFLOAT);
            }
		}
	Solver(Network& n, const char* s):
		net(n),x_lp(env,n.get_n_workers()),val_lp(-1),rt_lp(0),
		lp_model(env),lp_cplex(lp_model)
		{
            for (int i = 0; i < n.get_n_workers(); i++){
                x_lp[i]= NumVarMatrix(env, n.get_n_jobs());
                for (int j =0; j < n.get_n_jobs(); j++)
                    x_lp[i][j] = IloNumVarArray(env, n.get_n_jobs(), 0, 1, ILOFLOAT);
            }
		}
	
	~Solver(){ env.end();};
	void AddConstraints();
	void AddCycleConstraints();
	IloExpr CreateObjFunction();
	void Lp_AddConstraints();
	void Lp_AddCycleConstraints();
  	IloExpr Lp_CreateObjFunction();
	void SolveILP();
	void SolveLP();
	void PrintResultsIP();
	void PrintResultsLP();
	void PrintIteratLPResults();
	void PrintXVars(IloCplex cplex, ThreeDBoolVarMatrix x, const char* s);
	double GetValIP() const {return val_ip;}
	double GetValLP() const {return val_lp;}
	double GetRTLP() const {return rt_lp;}
	double GetRTIP() const {return rt_ip;}
	

};


#endif /* OLD_ ILP_SOLVER_H_ */
