/*
 * new_ilp_solver.cpp
 *
 *  Created on : Mar 25, 2014
 * 	Author : Ehsan
 */

#include "new_ilp_solver.h"
 #define RC_EPS 1.0e-40




void NewILPSolver::BirkhoffNeumannDecomposition()
{

	//copy y into proper array
//	int dsMatrix[R.Candidates()][R.Candidates()];
	
	//
//	int row[R.Candidate()];
//	int column[R.Candidates()];
	


}


void NewILPSolver::AddConstraint()
{
	for (int i = 0; i < R.GetRowSizeDiffMatrix(); i++)
	{
		IloExpr e(env);
		e += d[i];
		for (int j = 0; j < R.Candidates(); j++)
			for (int k = 0; k < R.Candidates(); k++)
				e += (R.GetDiffMatrix()[i][k]*y[k][j])/pow(2.0,(double)j);
		ip_model.add(e >= 0);
		lp_model.add(e >= 0);
		e.end();			
	}
	for (int j = 0; j < R.Candidates(); j++)
	{
		IloExpr e(env);
		for (int k = 0; k < R.Candidates(); k++)
			e += y[j][k];
		ip_model.add(e == 1);
		lp_model.add(e == 1);
		e.end();
	}

	for (int k = 0; k < R.Candidates(); k++)
	{
		IloExpr e(env);
		for (int j = 0; j < R.Candidates(); j++)
			e += y[j][k];
		ip_model.add(e == 1);
		lp_model.add(e == 1);
		e.end();
	}

}



IloExpr NewILPSolver::CreatObjFunction()
{
	IloExpr e(env);
	for (int i = 0; i < R.GetRowSizeDiffMatrix(); i++)
		e += d[i];
	return e;
}

void NewILPSolver::SolveILP()
{

	try 
	{
		AddConstraint();
		IloExpr obj_func = CreatObjFunction();
		ip_model.add(IloMinimize(env,obj_func));
		lp_model.add(IloMinimize(env,obj_func));


		if (!ip_cplex.solve())
		{	
			env.error() << "Failed to optimize LP. " << endl;	
		}
		env.out() << "IP Solution status = " << ip_cplex.getStatus() << endl;
		env.out() << "IP Solution value = " << ip_cplex.getObjValue() << endl;


		IloNumArray vals_ip(env);
		for (int i = 0; i < R.Candidates(); i++)
		{
			ip_cplex.getValues(vals_ip, y[i]);
			cout << "y[" << i << "] = "  << vals_ip << endl;  
		}
		vals_ip.end();	

		//lp
		for (int i = 0; i < R.Candidates(); i++)
			lp_model.add(IloConversion(env, y[i], ILOFLOAT));

		for (int i = 0; i < R.GetRowSizeDiffMatrix(); i++)
			lp_model.add(IloConversion(env, d[i], ILOFLOAT));

		if (!lp_cplex.solve())
		{	
			env.error() << "Failed to optimize LP. " << endl;	
		}
		env.out() << "LP Solution status = " << lp_cplex.getStatus() << endl;
		env.out() << "LP Solution value = " << lp_cplex.getObjValue() << endl;

		IloNumArray vals_lp(env);
		for (int i = 0; i < R.Candidates(); i++)
		{
			lp_cplex.getValues(vals_lp, y[i]);
			cout << "y[" << i << "]  = " << vals_lp << endl;  
		}
		vals_lp.end();

		IloNumArray vals_d(env);
		lp_cplex.getValues(vals_d, d);
		cout << " d  = " << vals_d << endl;  
		for (int i = 0; i < R.GetRowSizeDiffMatrix(); i++)
			if (vals_d[i] == 0.0000000000)
				cout <<"d are true 0" << endl;

		vals_d.end();
		cout << "************************ Test Feasibility ******************************" << endl;
//		TestFeasible();


	}
	catch(IloException& e)
	{
		cerr << "Concert exception caught" << e << endl;
	}
	catch(...)
	{
		cerr << "Unknown exception caught" << endl;
	}

}

/*

   void TestFeasible(IloBoolVarMatrix y)
   {


   try 
   {
   IloEnv env_t;
   IloModel m(env_t);
   env_t.add(y);


   }
   catch (IloException& e)
   {
   cerr << "Concert exception caught" << endl;
   }
   catch(...){
   cerr << "Unknown exception caught " << endl;
   }

   } */


void NewILPSolver::TestFeasible()
{
	for (int i = 0; i < R.GetRowSizeDiffMatrix(); i++)
	{
		double e = 0;
		for (int j = 0; j < R.Candidates(); j++)
			for (int k = 0; k < R.Candidates(); k++)
			{	
				//cout << "Difference Matrix is : " << R.GetDiffMatrix()[i][k] << endl;
				//cout  << "y value " << lp_cplex.getValue(y[k][j]) << endl;
				e += (R.GetDiffMatrix()[i][k]*lp_cplex.getValue(y[k][j]))/pow(2.0,(double)j);
				cout << "e is : " << e << endl;
			}
		cout << "******* The constraint " << i << " value is : " << e << endl;

	}

}

void NewILPSolver::PrintSolutionRanking()
{

}

