/*
 * lp_solver.cpp
 *
 *  Created on: April 5, 2014
 *      Author: eia2
 */


#include "old_lp_solver.h"


void OldLPSolver::PrintSolutionAssignment()
{

}



IloExpr OldLPSolver::CreateObjFunction()
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


void OldLPSolver::AddConstraints()
	/****************************************************/
	/* This function add constarints to the formulation */
	/****************************************************/

{
	for (int i = 0; i < R.Candidates(); i++)
		for (int j = 0; j < R.Candidates(); j++)
			if (i != j)
			{
				model.add(x[i][j] + x[j][i] == 1);
				lp_model.add(x[i][j] + x[j][i] == 1);
			}
			else
			{
				model.add(x[i][i] == 0);
				lp_model.add(x[i][i] == 0);
			}
}







void OldLPSolver::Seperation_3_cycles(vector<pair<pair<int, int>, int > >& cycles)
{
	/*
	   IloNumArray vals(env);
	   for (int i = 0; i < R.Candidates(); i++)
	   {
	   lp_cplex.getValues(vals,x[i]);
	   cout << "x[" << i <<"]=" << vals << endl;
	   }
	 */
	static int index= 0;
	for(int i = 0; i < R.Candidates(); i++)
		for(int j = 0; j < R.Candidates(); j++)
			if (i != j)
				if (lp_cplex.getValue(x[i][j]) >= 2./3)
				{
					for (int k = 0 ; k < R.Candidates(); k++)
						if (k != i && k != j)
						{	
							//cout << "hihiiiiiiiiiiiiiiiiiiiiiii" << endl;
							double sum = lp_cplex.getValue(x[i][j]) + lp_cplex.getValue(x[j][k]) + lp_cplex.getValue(x[k][i]); 
							//cout << "hi22222222222222222222222222"<< endl;
							if (sum > 2)
							{
								cycles.push_back(make_pair(make_pair(i,j) , k));
								index++;
							//	cout << "empty is not correct" << endl;
							//	cout << "index is : " << index++ << endl;
							}
						}		
				}
	cout << "index is: " << index << endl;

}


void OldLPSolver::AddViolated_3_Cycles(vector<pair<pair<int,int>,int> >& cycles)
{
	vector<pair<pair<int, int>,int > >::iterator it;
	for (it = cycles.begin(); it != cycles.end(); it++ )
	{
		int i = it->first.first;
		int j = it->first.second;
		int k = it-> second;
		//cout << "added a constraint222222222222222222222222222222" << endl;	
		lp_model.add(x[i][j] + x[j][k] + x[k][i] <= 2); 
	}
}

void OldLPSolver::IterativelyAddCycleConstraints_demo()
{
	vector<pair<pair<int, int> , int> > cycles ;
	for(;;)
	{
		Seperation_3_cycles(cycles);
		if (cycles.empty())
			break;
		AddViolated_3_Cycles(cycles);
		if ( !lp_cplex.solve() ) 
		{
			env.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}	
		cycles.clear();

	}
	env.out() << "old formulation LP ieteratively Solution status = " << lp_cplex.getStatus() << endl;
	env.out() << "Old formulation LP iteratively Solution value = " << lp_cplex.getObjValue() << endl;

}



void OldLPSolver::IterativelyAddCycleConstraints()
{

	bool flag = false;
	int nAdded_constraint = 0;
	while(!flag)
	{
		flag = true;
		for(int i = 0; i < R.Candidates(); i++)
			for(int j = 0; j < R.Candidates(); j++)
				if (i != j)
					if (lp_cplex.getValue(x[i][j]) >= 2./3)
					{
						for (int k = 0 ; k < R.Candidates(); k++)
							if (k != i && k != j)
							{	
								cout << "hihiiiiiiiiiiiiiiiiiiiiiii" << endl;
								double sum = lp_cplex.getValue(x[i][j]) + lp_cplex.getValue(x[j][k]) + lp_cplex.getValue(x[k][i]); 
								cout << "hi22222222222222222222222222"<< endl;
								if (sum > 2)
								{
									cout << "One const is added " << endl;
									lp_model.add(x[i][j]+x[j][k]+x[k][i] <= 2);
									nAdded_constraint++;
									flag = false;
								}
							}		
					}
		if ( !lp_cplex.solve() ) 
		{
			env.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}			
	}
	cout << "The number of added constraints are : " << nAdded_constraint << endl;
	env.out() << "old formulation LP ieteratively Solution status = " << lp_cplex.getStatus() << endl;
	env.out() << "Old formulation LP iteratively Solution value = " << lp_cplex.getObjValue() << endl;

	/*

	   -extract the fractional x_ab's and put them in graph data structure
	   -run the separation algotithm to find the violated ineqaulities if there is any
	   - introduce those inequalities to the model
	   -solve the model 
	   - repeat the above steps until there is no violated inequality

	// a potential edge is ddetected now look for 3-cycles
	//check if any of the endpoints has a in+out arc > 4./3
	/double sum_in_out_i = 0;
	//for (int k = 0; k < R.Candidates(); k++)
	//	sum_int_out_i += (x[i][k] + x[k][i]);
	//double sum_in_out_j = 0;
	//for (int k =0; k < R.Candidates(); k++)
	//	sum_in_out_j += (x[j][k] + x[k][j]);	


	 */



}	





void OldLPSolver::SolveLPIteratively()
{
	/*
	 * This function compute the optimal lp solution using CPLEX 
	 * 		          
	 */
	time_t start, end;
	try
	{
		IloExpr obj_fun = CreateObjFunction(); //generate the obj_function
		//ip_model.add(IloMinimize(env, obj_fun));
		lp_model.add(IloMinimize(env, obj_fun));

		AddConstraints();/*add new constraints here*/
		// solve the model

		cout<<"#######################start"<<endl;
		time(&start);
		if ( !lp_cplex.solve() ) 
		{
			env.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}	
		//	env.out() << "old formulation iterative LP Solution status = " << lp_cplex.getStatus() << endl;
		//	env.out() << "Old formulation iterative LP Solution value = " << lp_cplex.getObjValue() << endl;

		//adding cycle elimination and cardinality constraints

		IterativelyAddCycleConstraints_demo();
		time(&end);
		cout << "The running time of Iterated LP in seconds is : " <<  difftime(end, start) << endl;
		cout<<"####################solved iteratively "<<endl;
		cout << "The manual obj is : " << Manual_Obj_Computation(lp_cplex) << endl ;
		IloNumArray vals(env);
		for (int i = 0; i < R.Candidates(); i++)
		{
			lp_cplex.getValues(vals,x[i]);
			cout << "x[" << i <<"]=" << vals << endl;
		}

	}
	catch ( IloException & e)
	{
		cerr << "Concert exception caught: " << e << endl;
	} catch (...)
	{
		cerr << "Unknown exception caught" << endl;
	}



}


void OldLPSolver::AddCycleConstraints()
{
	for (int i = 0; i < R.Candidates(); i++)
		for (int j = 0; j < R.Candidates(); j++)
			for (int k = 0; k < R.Candidates(); k++)
				if ((i != j) && (j != k) && ( k != i))
				{
					model.add(x[i][j] + x[j][k] + x[k][i] <= 2);
					//	model.add(x[i][j] + x[j][k] + x[k][i] <= 2);

				}
}

void OldLPSolver::SolveLP()
{
	time_t start, end;
	try
	{
		time(&start);
		IloExpr obj_fun = CreateObjFunction(); //generate the obj_function
		model.add(IloMinimize(env, obj_fun));

		// AddConstraints();/*add new constraints here*/
		//adding cycle elimination and cardinality constraints
		AddCycleConstraints();


		// solve the model
		cout<<"#######################start"<<endl;
		if ( cplex.solve() ) {
			env.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}
		cout<<"####################solved"<<endl;

		env.out() << "old formulation non-Itarated LP Solution status = " << cplex.getStatus() << endl;
		env.out() << "Old formulation non-Iterated LP Solution value = " << cplex.getObjValue() << endl;
		time(&end);
		cout << "The non-iterated LP time in second is : " << difftime(end, start) << endl;

		IloNumArray vals(env);
		for (int i = 0; i < R.Candidates(); i++)
		{
			cplex.getValues(vals,x[i]);
			cout << "x[" << i <<"]=" << vals << endl;
		}


	}
	catch ( IloException & e)
	{
		cerr << "Concert exception caught: " << e << endl;
	} catch (...)
	{
		cerr << "Unknown exception caught" << endl;
	}



}




double OldLPSolver::Manual_Obj_Computation(IloCplex cplex)
{
	double obj = 0; 

	for (int i = 0 ; i < R.Candidates(); i++)
		for (int j = 0; j < R.Candidates(); j++)
			obj += cplex.getValue(x[i][j]*R.Get_m_Array()[j][i]);

	return obj;
}




