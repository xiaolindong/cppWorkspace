/*
 * ilp_solver.cpp
 *
 *  Created on: Jan 14, 2014
 *      Author: eia2
 */

#include "ilp_solver.h"




void ILPSolver::PrintOptAssignment()
{

	//IP results
	cout << "The IP solution is :" << cplex.getObjValue() << endl;
	E.eFile() << "The IP solution is : " << cplex.getObjValue() << endl;
	IloNumArray vals(env),vals_y(env);
	cplex.getValues(vals_y,y);
	E.eFile() << "Selected candiates are : "; 	
	cout << "Slected Candidates are : ";
	for (int i = 0; i < E.Candidates(); i++)
		if (vals_y[i])
		{
			E.eFile() << i << " ";
			cout << i << " "; 
		}
	E.eFile() << endl;
	cout << endl;
	for (int i = 0; i < E.Candidates(); i++)
	{
		if (vals_y[i])
		{
			cplex.getValues(vals, x[i]);
			E.eFile() << "candidate " << i << " : " ;
			cout << "candidate " << i << " : "; 
			for (int j = 0; j < E.Voters(); j++)
				if (vals[j])	
				{
					E.eFile() << j << " ";
					cout << j << " ";
				}
		}
		E.eFile() << endl;
		cout  << endl;
	}
	vals.clear();
	vals_y.clear();
	
	//LP results
	cout << "The LP solution is :" << lp_cplex.getObjValue() << endl;
	E.eFile() << "The LP solution is : " << lp_cplex.getObjValue() << endl;
	lp_cplex.getValues(vals_y,y);
	E.eFile() << "Selected candiates are : "; 	
	cout << "Slected Candidates are : ";
	for (int i = 0; i < E.Candidates(); i++)
		if (vals_y[i])
		{
			E.eFile() << i << " ";
			cout << i << " "; 
		}
	E.eFile() << endl;
	cout << endl;
	for (int i = 0; i < E.Candidates(); i++)
	{
		if (vals_y[i])
		{
			lp_cplex.getValues(vals, x[i]);
			E.eFile() << "candidate " << i << " : " ;
			cout << "candidate " << i << " : "; 
			for (int j = 0; j < E.Voters(); j++)
				if (vals[j])	
				{
					E.eFile() << j << " ";
					cout << j << " ";
				}
		}
		E.eFile() << endl;
		cout  << endl;
	}
	vals.end();
	vals_y.end();



}





void ILPSolver::PrintResult()
{
	//IP results
	E.eFile() << "****** The IP results *****" << endl;
	cout << "The IP solution is :" << cplex.getObjValue() << endl;
	E.eFile() << "The IP solution is : " << cplex.getObjValue() << endl;
	IloNumArray vals(env),vals_y(env);
	cplex.getValues(vals_y,y);
	E.eFile() << "y = " << vals_y << endl;
	cout << "y = " << vals_y << endl;
	for (int i = 0; i < E.Candidates(); i++)
		{
			cplex.getValues(vals,x[i]);
			cout << "x[" << i <<"]=" << vals << endl;
			E.eFile() << "x[" << i << "]= " << vals << endl;
		}
	vals.clear();
	vals_y.clear();
	E.eFile() << endl << endl;

	//LP Results
	E.eFile() << "*********** The LP Results" << endl;
	cout << "The LP solution is :" << lp_cplex.getObjValue() << endl;
	E.eFile() << "The LP solution is : " << lp_cplex.getObjValue() << endl;

	lp_cplex.getValues(vals_y,y);
	E.eFile() << "y = " << vals_y << endl;
	cout << "y = " << vals_y << endl;
	for (int i = 0; i < E.Candidates(); i++)
		{
			lp_cplex.getValues(vals,x[i]);
			cout << "x[" << i <<"]=" << vals << endl;
			E.eFile() << "x[" << i << "]= " << vals << endl;
		}
	E.eFile() << endl << endl;
	vals.end();
	vals_y.end();

}



IloExpr ILPSolver::CreateObjFunction()
/**************************************************/
/* This Function generates the objective function */
/**************************************************/
{
	IloExpr e(env);
	for(int i = 0; i < E.Candidates(); i++)
		for(int j = 0; j < E.Voters(); j++)
			e += (E.Ranking()[i][j]-1) * x[i][j] ;
	/*for (int i = 0; i < E.Candidates();i++)
		e+= 3*y[i];*/
	return e;
}



void ILPSolver::AddConstraints()
/****************************************************/
/* This function add constarints to the formulation */
/****************************************************/

{
	IloExpr constraint(env);
	for (int i = 0; i < E.Candidates(); i++)
	{
		constraint += y[i];
	}
	model.add(constraint == E.K());
	lp_model.add(constraint == E.K());
	for (int j = 0; j < E.Voters(); j++)
	{
		IloExpr constraint(env);
		for(int i = 0; i < E.Candidates(); i++)
		{
			constraint+=x[i][j];
		}
		model.add(constraint == 1);
		lp_model.add(constraint == 1);
	}
	for(int i = 0; i < E.Candidates(); i++)
	{
		for (int j = 0; j < E.Voters(); j++)
		{
			model.add(x[i][j]-y[i] <= 0);
			lp_model.add(x[i][j]-y[i] <= 0);
		}
	}
	if (E.IsCapacitated())
		for (int i = 0; i < E.Candidates(); i++)
		{
			IloExpr e(env);
			for (int j = 0; j < E.Voters(); j++)
			{
				e += x[i][j];
			}	
			model.add(-ceil(E.Voters()/E.K())*y[i] + e <= 0);
			model.add(-floor(E.Voters()/E.K())*y[i] + e >= 0);
			lp_model.add(-ceil(E.Voters()/E.K())*y[i] + e <= 0);
			lp_model.add(-floor(E.Voters()/E.K())*y[i] + e >= 0);
		}
}



void ILPSolver::SolveILP()
{
	try {

		AddConstraints();
		IloExpr obj_fun = CreateObjFunction();
		model.add(IloMinimize(env, obj_fun));

		if ( !cplex.solve() ) {
			env.error() << "Failed to optimize LP." << endl;
			throw(-1);
		}
		//IloNumArray vals(env);
		env.out() << "Solution status = " << cplex.getStatus() << endl;
		env.out() << " IP Solution value = " << cplex.getObjValue() << endl;
		E.SetValIP(static_cast<double>(cplex.getObjValue()));


		//lp
		lp_model.add(IloMinimize(env, obj_fun));
		for (int i = 0; i < E.Candidates(); i++)
		{
			lp_model.add(IloConversion(env, x[i], ILOFLOAT));
			lp_model.add(IloConversion(env,y[i], ILOFLOAT));
		}
		if ( !lp_cplex.solve() )
		{
			env.error() << "Failed to optimize LP." << endl;
			throw(-1);
		}
		E.SetValLP(static_cast<double>(lp_cplex.getObjValue()));

		/*
		IloNumArray vals_lp(env);
		env.out() << "LP Solution status = " << lp_cplex.getStatus() << endl;
		env.out() << "LP Solution value = " << lp_cplex.getObjValue() << endl;
			
		for (int i = 0; i < E.Candidates(); i++)
		{
			lp_cplex.getValues(vals_lp,x[i]);
			cout << "x[" << i <<"]=" << vals_lp << endl;
		}
		lp_cplex.getValues(vals_lp,y);
		cout << "y = " << vals_lp << endl;
		vals_lp.end();*/
		PrintResult();
	}
	catch (IloException& e) {
		cerr << "Concert exception caught: " << e << endl;
	}
	catch (...) {
		cerr << "Unknown exception caught" << endl;
	}
	//env.end();
}

void ILPSolver::SolveLP()
{

	//IloExpr obj_fun = CreateObjFunction();
	//lp_model.add(IloMinimize(env, obj_fun));

}




