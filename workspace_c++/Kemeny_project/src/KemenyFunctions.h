/*
 * KemenyFunctions.h
 *
 *  Created on: Oct 7, 2013
 *      Author: eia2
 */

#ifndef KEMENYFUNCTIONS_H_
#define KEMENYFUNCTIONS_H_



#include <iostream>
#include "variables.h"
#include <ilcplex/ilocplex.h>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <map>
#include <time.h>


using namespace std;
ILOSTLBEGIN




/*******************************************************************************/






IloExpr generate_obj_fun(BoolVarMatrix x)
/**************************************************/
/* This Function generates the objective function */
/**************************************************/
{

	IloExpr e(env);
	for(int i = 0; i < nCandidates; i++)
	{
		for(int j = 0; j < nCandidates; j++)
		{
			if (i != j)
				e += m[j][i] * x[i][j] ;
		}
	}
	return e;

}




void addConstraint(IloModel &model, BoolVarMatrix x)
/****************************************************/
/* This function add constarints to the formulation */
/****************************************************/

{
	for (int i = 0; i < nCandidates; i++)
		for (int j = 0; j < nCandidates; j++)
			if (i != j)
				model.add(x[i][j] + x[j][i] == 1);
			else
				model.add(x[i][i] == 0);
}



void AddCycleConstraints( IloModel &model, BoolVarMatrix x)
{
	for (int i = 0; i < nCandidates; i++)
		for (int j = 0; j < nCandidates; j++)
			for (int k = 0; k < nCandidates; k++)
				if ((i != j) && (j != k) && ( k != i))
					model.add(x[i][j] + x[j][k] + x[k][1] <= 2);
}






float Kemeny_ILP(ofstream& result)
{
/**********************************************************/
/* This function compute the optimal solution using CPLEX */
/* 		   Minimizing Proportional Representaion          */
/**********************************************************/
	time_t start,end;
	double optSolution = 0 ;
	cout<<"##################construction"<<endl;
	time(&start);
	try
	{

		// Model and variables defining
		IloModel model(env);
		BoolVarMatrix x(env,nCandidates);
		for (int i = 0; i < nCandidates; i++)
				x[i]= IloBoolVarArray(env, nCandidates);


		IloExpr obj_fun = generate_obj_fun(x); //generate the obj_function
		model.add(IloMinimize(env, obj_fun));

		addConstraint(model,x);/*add new constraints here*/
		//adding cycle elimination and cardinality constraints
		 AddCycleConstraints(model, x);


		// solve the model
		cout<<"#######################start"<<endl;
		IloCplex cplex(model);
		if ( !cplex.solve() ) {
			env.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}
		cout<<"####################solved"<<endl;

		env.out() << "Solution status = " << cplex.getStatus() << endl;
		env.out() << "Solution value = " << cplex.getObjValue() << endl;
		IloNumArray vals(env);
		for (int i = 0; i < nCandidates; i++)
		{
			cplex.getValues(vals,x[i]);
			cout << "x[" << i <<"]=" << vals << endl;
		}
		//adding cycle elimination and cardinality constraints
		 //AddCycleConstraints(model, x);

		//cplex.setParam(IloCplex::RootAlg, IloCplex::Primal);
		//cplex.setParam(IloCplex::NodeAlg, IloCplex::Primal);
		/*if ( !cplex.solve() ) {
					env.error() << "Failed to optimize LP." << endl;
					throw (-1);
				}*/

		time(&end);
		cout << "The runnung time in seconds is : " << difftime(end,start) << " seconds" << endl;
		env.out() << "Solution value = " << cplex. getObjValue () << endl;

	} catch ( IloException & e)
	{
		cerr << "Concert exception caught: " << e << endl;
	} catch (...)
	{
		cerr << "Unknown exception caught" << endl;
	}

	env.end();
return optSolution;
}




#endif /* KEMENYFUNCTIONS_H_ */
