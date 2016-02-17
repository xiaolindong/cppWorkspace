/*
 * ilp_solver.cpp
 *
 *  Created on: Jan 14, 2014
 *      Author: eia2
 */

#include "solver.h"
#define RC_EPS 1.0e-6

void Solver::PrintResultsIP(){}
//void Solver::PrintXVars(IloCplex cplex, BoolVarMatrix x, const char* s){}
void Solver::PrintResultsLP(){}

IloExpr Solver::CreateObjFunction()
{
    IloExpr e(env);
    for( int k = 0; k < net.get_n_workers(); k++){
        for(int i = 0; i < net.get_n_jobs(); i++){
            for(int j = 0; j < net.get_n_jobs(); j++){
                if(i != j)
                    e += net.Cost()[i][j] * x[k][i][j];
            }
        }
    }
	return e;
}

void Solver::AddConstraints()
{
    for(int i = 0; i < net.get_n_jobs(); i++)
    {
        IloExpr constraint(env);
        for(int k = 0; k < net.get_n_workers(); k++){
            for(int j = 0; j < net.get_n_jobs(); j++)
                if(i != j)
                    constraint += x[k][i][j];
        }
        ip_model.add(constraint == 1);
    }
    
    //constraint 3.4
    for(int k = 0; k < net.get_n_workers(); k++){
        IloExpr constraint(env);
        for(int j = 0; j < net.get_n_jobs(); j++){
            constraint += x[k][0][j];
        }
        ip_model.add(constraint == 1);
    }
    //constraint 3.5
    for(int k = 0; k < net.get_n_workers(); k++){
        for(int i = 1; i <  net.get_n_jobs(); i++){
            IloExpr constraint(env);
            for(int j = 0; j <= net.get_n_jobs(); j++){
                if(i != j)
                    constraint = constraint + x[k][i][j] - x[k][j][i];
            }
            lp_model.add(constraint == 0);
        }
    }

}
void Solver::SolveILP()
{
try
	{
		IloExpr obj_fun = CreateObjFunction(); //generate the obj_function
        ip_model.add(IloMinimize(env, obj_fun));

		AddConstraints();/*add new constraints here*/
		//cplex parameter
		//ip_cplex.setOut(env.getNullStream());

		// time setting
		time_t start, end;
		time(&start);
		clock_t s,e;
		s = clock();	
	
		// solve the model
		//cout<<"#######################start"<<endl;
		if ( !ip_cplex.solve() ) {
			env.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}
		ip_cplex.exportModel("ip.lp");
		//cout<<"####################solved"<<endl;
		
		//time setting
		e = clock();
		time(&end);
	//	rt_lp = diffclock(e,s);
		double time_differ = difftime(end, start);
		cout << "The time is :" << lp_cplex.getCplexTime() << endl;;
		cout << "the clock time is :" << rt_lp << endl;
        cout << "The difftime is :" << time_differ << endl;
		PrintResultsIP();
	}
	catch ( IloException & e)
	{
		cerr << "Concert exception caught: " << e << endl;
	} catch (...)
	{
		cerr << "Unknown exception caught" << endl;
	}
}

IloExpr Solver::Lp_CreateObjFunction()
{
    IloExpr e(env);
    for( int k = 0; k < net.get_n_workers(); k++){
        for(int i = 0; i < net.get_n_jobs(); i++){
            for(int j = 0; j < net.get_n_jobs(); j++){
                if(i != j)
                    e += net.Cost()[i][j] * x_lp[k][i][j];
            }
        }
    }
    return e;
}

void Solver::Lp_AddConstraints()
{
    for(int i = 0; i < net.get_n_jobs(); i++)
    {
        IloExpr constraint(env);
        for(int k = 0; k < net.get_n_workers(); k++){
            for(int j = 0; j < net.get_n_jobs(); j++)
                if(i != j)
                    constraint += x_lp[k][i][j];
        }
        lp_model.add(constraint == 1);
    }
    
    //constraint 3.4
    for(int k = 0; k < net.get_n_workers(); k++){
        IloExpr constraint(env);
        for(int j = 0; j < net.get_n_jobs(); j++){
            constraint += x_lp[k][0][j];
        }
        lp_model.add(constraint == 1);
    }
    //constraint 3.5
    for(int k = 0; k < net.get_n_workers(); k++){
        for(int i = 1; i < net.get_n_jobs(); i++){
            IloExpr constraint(env);
            for(int j = 0; j <= net.get_n_jobs(); j++){
                if(i != j)
                    constraint = constraint + x_lp[k][i][j] - x_lp[k][j][i];
            }
            lp_model.add(constraint == 0);
        }
    }

}
void Solver::SolveLP()
{
	try
	{
		IloExpr obj_fun = Lp_CreateObjFunction(); //generate the obj_function
		lp_model.add(IloMinimize(env, obj_fun));
		// cplex settings
		lp_cplex.setOut(env.getNullStream());
		// time setting
		time_t start, end;
		time(&start);
		clock_t s,e;
		s = clock();
		// solve thelp  model
		cout<<"#######################start"<<endl;
		if ( !lp_cplex.solve() ) {
			env.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}
		cout<<"####################solved"<<endl;
		e = clock();
		time(&end);
		double time_differ = difftime(end, start);
		//rt_lp = diffclock(e,s);
		n_lp_constraint = lp_cplex.getNrows();
		cout << "The time is :" << lp_cplex.getCplexTime() << endl;;
		cout << "the clock time is :" << rt_lp << endl;
		cout << "The difftime is :" << time_differ << endl;
		//	env.out() << "old formulation LP Solution status = " << lp_cplex.getStatus() << endl;
		//	env.out() << "Old formulation LP Solution value = " << lp_cplex.getObjValue() << endl;
		val_lp = lp_cplex.getObjValue();
		//rt_lp = diffclock(e,s);
	}
	catch ( IloException & e)
	{
		cerr << "Concert exception caught: " << e << endl;
	} catch (...)
	{
		cerr << "Unknown exception caught" << endl;
	}
}
