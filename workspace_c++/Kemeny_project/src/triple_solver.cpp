/*
 * triple_solver.cpp
 *
 *  Created on: April 14, 2014
 *      Author: eia2
 */


#include "triple_solver.h"


void TripleFormulKemeny::PrintResults()
{
	cout << "The x values in IP are : " << endl;
	IloNumArray val_x_ip(env);
	for (int i = 0; i < R.Candidates(); i++)
	{
		ip_cplex.getValues(val_x_ip,x[i]);
		cout << "x[" << i <<"]=" << val_x_ip << endl;
	}
	cout << "The y values in IP are :" << endl;
	IloNumArray val_y_ip(env);
	for (int i = 0; i < R.Candidates();i++)
		for (int j = 0; j < R.Candidates(); j++)
		{	
			cout << "y["<< i <<"]["<< j << "]= " ;

			for (int k = 0; k < R.Candidates();k++)
			{
				//ip_cplex.getValues(val_y_ip,y[i][j]);
				cout <<ip_cplex.getValue( y[i][j][k]) << " " ;						
			}
			cout << endl;
		}
}



IloExpr TripleFormulKemeny::CreateObjFunction()
	/**************************************************/
	/* This Function generates the objective function */
	/**************************************************/
{

	IloExpr e(env);
	for(int i = 0; i < R.Candidates(); i++)
	{
		for(int j = 0; j < R.Candidates(); j++)
		{
			if (i != j)
			{
				//cout << " R j, i is :"<<R.Get_m_Array()[j][i] <<" ";
				//cout << endl;
				e += (R.Get_m_Array()[j][i] * x[i][j]);

			}
		}
	}
	return e;

}


void TripleFormulKemeny::AddConstraints()
	/****************************************************/
	/* This function add constarints to the formulation */
	/****************************************************/

{
	for (int i = 0; i < R.Candidates(); i++)
		for (int j = 0; j < R.Candidates(); j++)
			if (i != j)
			{
				ip_model.add(x[i][j] + x[j][i] == 1);
				lp_model.add(x[i][j] + x[j][i] == 1);
			}
			else
			{
				ip_model.add(x[i][i] == 0);
				lp_model.add(x[i][i] == 0);
			}
}



void TripleFormulKemeny::AddTripleConstraints()
{
	for (int i = 0; i < R.Candidates(); i++)
		for (int j = 0; j < R.Candidates(); j++)
			for (int k = 0; k < R.Candidates(); k++)
				if ( i!=j && i!=k && j!=k)
				{
					ip_model.add(y[i][j][k] - x[i][j] <= 0);
					ip_model.add(y[i][j][k] - x[j][k] <= 0);
					ip_model.add(y[i][j][k] - x[i][k] <= 0);

					lp_model.add(y[i][j][k] - x[i][j] <= 0);
					lp_model.add(y[i][j][k] - x[j][k] <= 0);
					lp_model.add(y[i][j][k] - x[i][k] <= 0);
				}
	for (int i = 0; i < R.Candidates(); i++)
		for (int j = 0; j < R.Candidates(); j++)
			for (int k = 0; k < R.Candidates(); k++)
				if ( i!=j && i!=k && j!=k)
				{
					ip_model.add(y[i][j][k] + y[i][k][j] + y[j][i][k] + y[j][k][i] + y[k][i][j] + y[k][j][i] == 1);
					lp_model.add(y[i][j][k] + y[i][k][j] + y[j][i][k] + y[j][k][i] + y[k][i][j] + y[k][j][i] == 1);

				}
	/*
	   for (int i = 0; i < R.Candidates(); i++)
	   for (int j = 0; j < R.Candidates(); j++)
	   if (i !=j)
	   for (int k = 0; k < R.Candidates(); k++)	
	   if (k!=j)
	   {
	   ip_model.add(y[k][i][j] + y[i][k][j] + y[i][j][k] - x[i][j] == 0);
	   lp_model.add(y[k][i][j] + y[i][k][j] + y[i][j][k] - x[i][j] == 0);

	   }*/
	/*	
		for(int i = 0; i < R.Candidates(); i++)
		for(int j =0; j < R.Candidates(); j++)
		for (int k = 0 ; k < R.Candidates(); k++)
		if (i == j || i == k || j == k)
		{
		ip_model.add(y[i][j][k] == 0);
		lp_model.add(y[i][j][k] == 0);
		cout << endl << i <<","<< j << "," << k << endl; 
		}*/
}




void TripleFormulKemeny::SolveILP()
{
	/**********************************************************/
	/* This function compute the optimal solution using CPLEX */
	/* 		   Minimizing Proportional Representaion          */
	/**********************************************************/
	try
	{

		IloExpr obj_fun = CreateObjFunction(); //generate the obj_function
		ip_model.add(IloMinimize(env, obj_fun));
		lp_model.add(IloMinimize(env, obj_fun));

		AddConstraints();/*add new constraints here*/
		//adding cycle elimination and cardinality constraints
		AddTripleConstraints();


		// solve the model
		cout<<"#######################start"<<endl;
		if ( !ip_cplex.solve() ) {
			env.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}
		cout<<"####################solved"<<endl;

		env.out() << "Triple formulation IP Solution status = " << ip_cplex.getStatus() << endl;
		env.out() << "Triple formulation IP Solution value = " << ip_cplex.getObjValue() << endl;
		val_ip = ip_cplex.getObjValue();
		cout << "The x values in IP are : " << endl;
		IloNumArray vals(env);
		for (int i = 0; i < R.Candidates(); i++)
		{
			ip_cplex.getValues(vals,x[i]);
			cout << "x[" << i <<"]=" << vals << endl;
		}


		for (int i = 0; i < R.Candidates(); i++)
		{
			lp_model.add(IloConversion(env, x[i], ILOFLOAT));
		}
		for (int i = 0; i < R.Candidates();i++)
		{
			for (int j = 0; j < R.Candidates(); j++)
				lp_model.add(IloConversion(env,y[i][j], ILOFLOAT));
		}

		time_t start, end;
		time(&start);
		// solve the lp  model
		cout<<"#######################start"<<endl;
		if ( !lp_cplex.solve() ) {
			env.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}
		cout<<"####################solved"<<endl;

		env.out() << "Triple formulation LP Solution status = " << lp_cplex.getStatus() << endl;
		env.out() << "Triple formulation LP Solution value = " << lp_cplex.getObjValue() << endl;

/*
		//add cuts
		for (int i = 0; i < R.Candidates(); i++)
			for (int j = 0; j < R.Candidates(); j++)	
				if (i != j)
				{
					for (int p = 0; p < R.Candidates(); p++)
						for (int q = 0; q < R.Candidates(); q++)
							if ( i!=p && i!=q && j!=p && j!=q)	

								lp_model.add(y[p][i][j] + y[i][p][j] + y[i][j][p] + y[q][j][i] + y[j][q][i] + y[j][i][q] == 1);

				}
		

		// again solving the LP model after adding the cuts
		if ( !lp_cplex.solve() ) {
			env.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}
		//
*/		
		val_lp = lp_cplex.getObjValue();
		time(&end);
		cout << "The Triple Formulation LP time (relax in ILP) in second is : " << difftime(end, start) << endl;
		rt_lp = difftime(end, start);
		//	cout << "The manual obj is : " << Manual_Obj_Computation(lp_cplex) << endl ;
		cout << "The x values in LP are : " << endl;
		IloNumArray vals_lp(env);
		for (int i = 0; i < R.Candidates(); i++)
		{
			lp_cplex.getValues(vals_lp,x[i]);
			cout << "x[" << i <<"]=" << vals_lp << endl;
		}
		//	PrintResults();
		//cplex.setParam(IloCplex::RootAlg, IloCplex::Primal);
		//cplex.setParam(IloCplex::NodeAlg, IloCplex::Primal);


	}
	catch ( IloException & e)
	{
		cerr << "Concert exception caught: " << e << endl;
	} catch (...)
	{
		cerr << "Unknown exception caught" << endl;
	}



}


void TripleFormulKemeny::SolveLP()
{

	//IloExpr obj_fun = CreateObjFunction();
	//lp_model.add(IloMinimize(env, obj_fun));

}


void TripleFormulKemeny::Test_Triple_Violation(ofstream& file)
{
	file << "This is to  find the violated triple constraint "<< endl;
	for (int i = 0; i < R.Candidates(); i++)
		for (int j = 0; j < R.Candidates(); j++)	
			if (i != j)
			{
				for (int p = 0; p < R.Candidates(); p++)
					for (int q = 0; q < R.Candidates(); q++)
						if ( i!=p && i!=q && j!=p && j!=q)
							if (lp_cplex.getValue(y[p][i][j]) + lp_cplex.getValue(y[i][p][j]) + lp_cplex.getValue(y[i][j][p]) + lp_cplex.getValue(y[q][j][i]) + lp_cplex.getValue(y[j][q][i]) + lp_cplex.getValue(y[j][i][q]) != 1)
								file << "Find violated constraint DDDDD" << endl;
			}
}

double TripleFormulKemeny::Manual_Obj_Computation(IloCplex cplex)
{
	double obj = 0; 

	for (int i = 0 ; i < R.Candidates(); i++)
		for (int j = 0; j < R.Candidates(); j++)
			obj += cplex.getValue(x[i][j]*R.Get_m_Array()[j][i]);

	return obj;
}



