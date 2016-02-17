/*
 * ilp_solver.cpp
 *
 *  Created on: Jan 14, 2014
 *      Author: eia2
 */


#include "old_ilp_solver.h"
#define RC_EPS 1.0e-6


void OldILPSolver::PrintResultsIP()
{
	ofstream file("../data/x_ip.txt");
	cout << "The Ip solution is :" << ip_cplex.getObjValue() << endl;
	file << " the IP solution is " << endl;

	IloNumArray vals(env);
	for (int i = 0; i < R.Candidates(); i++)
	{
		ip_cplex.getValues(vals,x[i]);
		cout << "x[" << i <<"]=" << vals << endl;
		for(int j =0 ; j < R.Candidates(); j++)
			file << vals[j]  << " ";
		file << endl;
	}
	file.close();
}


void OldILPSolver::PrintXVars(IloCplex cplex, BoolVarMatrix x, const char* s)
{
	ofstream file(s);
	IloNumArray vals(env);
	for (int i = 0; i < R.Candidates(); i++)
	{
		cplex.getValues(vals,x[i]);
		for(int j =0 ; j < R.Candidates(); j++)
			file << vals[j]  << " ";
		file << endl;
	}
	file.close();
}

void OldILPSolver::PrintResultsLP()
{
	ofstream file("../data/x_lp.txt");
	//file << endl << "the LP solution is :" << endl;

	IloNumArray vals_lp(env);
	for (int i = 0; i < R.Candidates(); i++)
	{
		lp_cplex.getValues(vals_lp,x_lp[i]);
		cout << "x[" << i <<"]=" << vals_lp << endl;
		for(int j =0 ; j < R.Candidates(); j++)
			file << vals_lp[j]  << " ";
		file << endl;
	}
	file.close();
}



IloExpr OldILPSolver::CreateObjFunction()
	/**************************************************/
	/* This Function generates the objective function */
	/**************************************************/
{

	IloExpr e(env);
	for(int i = 0; i < R.Candidates(); i++)
	{
		for(int j = 0; j < R.Candidates(); j++)
		{
		//	if (i != j)
		//	{
				//cout << " R j, i is :"<<R.Get_m_Array()[j][i] <<" ";
				//cout << endl;
				e += (R.Get_m_Array()[i][j] * x[i][j]);

			//}
		}
	}
	return e;

}


void OldILPSolver::AddConstraints()
	/****************************************************/
	/* This function add constarints to the formulation */
	/****************************************************/

{
	for (int i = 0; i < R.Candidates(); i++)
		for (int j = 0; j < R.Candidates(); j++)
			if (i != j)
			{
				ip_model.add(x[i][j] + x[j][i] == 1);
				//			lp_model.add(x[i][j] + x[j][i] == 1);
			}
			else
			{
				ip_model.add(x[i][i] == 0);
				//			lp_model.add(x[i][i] == 0);
			}
}



void OldILPSolver::AddCycleConstraints()
{
	for (int i = 0; i < R.Candidates(); i++)
		for (int j = 0; j < R.Candidates(); j++)
			for (int k = 0; k < R.Candidates(); k++)
				if ((i != j) && (j != k) && ( k != i))
				{
					ip_model.add(x[i][j] + x[j][k] + x[k][i] <= 2);
					//			lp_model.add(x[i][j] + x[j][k] + x[k][i] <= 2);

				}
}	




void OldILPSolver::SolveILP()
{
	/**********************************************************/
	/* This function compute the optimal solution using CPLEX */
	/* 		   Minimizing Proportional Representaion          */
	/**********************************************************/
	try
	{
		Solution sol(R.Candidates());
		SolveLP();
		// intial solution 
		IloNumVarArray startVar(env);
		IloNumArray startVal(env);
	//	startVal = CreateInitialSolution(sol);	

		//prepare the vars for the initial solution
		for (int u = 0; u < R.Candidates(); ++u){
			for (int v = 0; v < R.Candidates(); ++v) {
				startVar.add(x[u][v]);
			}
		}

		IloExpr obj_fun = CreateObjFunction(); //generate the obj_function
		ip_model.add(IloMaximize(env, obj_fun));
		//	lp_model.add(IloMinimize(env, obj_fun));

		AddConstraints();/*add new constraints here*/
		//adding cycle elimination and cardinality constraints
		AddCycleConstraints();

		//cplex parameter
		//ip_cplex.setOut(env.getNullStream());
		//cplex.setParam(IloCplex::RootAlg, IloCplex::Primal);
		//cplex.setParam(IloCplex::NodeAlg, IloCplex::Primal);

		//polish the MIP start
		/*
		   ip_cplex.setParam(IloCplex::PolishAfterTime,0);
		   ip_cplex.setParam(IloCplex::TiLim, 1000);
		   ip_cplex.setParam(IloCplex::AdvInd, 1);*/
		 //  ip_cplex.addMIPStart(startVar, startVal, IloCplex::MIPStartAuto,  "secondMIPStart");
		   
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
		rt_lp = diffclock(e,s);
		double time_differ = difftime(end, start);
		cout << "The time is :" << lp_cplex.getCplexTime() << endl;;
		cout << "the clock time is :" << rt_lp << endl;
		cout << "The difftime is :" << time_differ << endl;

		//print results
		//env.out() << "old formulation IP Solution status = " << ip_cplex.getStatus() << endl;
		//env.out() << "Old formulation IP Solution value = " << ip_cplex.getObjValue() << endl;
		val_ip = ip_cplex.getObjValue();	
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



IloExpr OldILPSolver::Lp_CreateObjFunction()
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
				//cout << "i is : " << " and j is : " << j  << endl;
				//	cout << endl<< endl;;
				//	cout << " R j, i is :"<<R.Get_m_Array()[j][i] <<" ";
				//cout << endl;
				e += (R.Get_m_Array()[i][j] * x_lp[i][j]);

			}
		}
	}
	return e;

}


void OldILPSolver::Lp_AddConstraints()
	/****************************************************/
	/* This function add constarints to the formulation */
	/****************************************************/

{
	for (int i = 0; i < R.Candidates(); i++)
		for (int j = 0; j < R.Candidates(); j++)
		{
			n_iterat_constraint++;
			if (i != j)
			{
				//	lp_model.add(x_lp[i][j] + x_lp[j][i] == 1);
				lp_model_iterat.add(x_lp[i][j] + x_lp[j][i] == 1);
			}
			else
			{
				//	lp_model.add(x_lp[i][i] == 0);
				lp_model_iterat.add(x_lp[i][i] == 0);
			}
		}
}



void OldILPSolver::Lp_AddCycleConstraints()
{
	for (int i = 0; i < R.Candidates(); i++)
		for (int j = 0; j < R.Candidates(); j++)
			for (int k = 0; k < R.Candidates(); k++)
				//		if ((i != j) && (j != k) && ( k != i))
			{
				lp_model.add(x_lp[i][j] + x_lp[j][k] + x_lp[k][i] <= 2);

			}
}	

void OldILPSolver::SolveLP(){
	try{
		IloExpr obj_fun = Lp_CreateObjFunction(); //generate the obj_function
		lp_model.add(IloMaximize(env, obj_fun));
		lp_cplex.setOut(env.getNullStream());
		//	lp_cplex.setParam(IloCplex::TiLim, 100);
		lp_cplex.setParam(IloCplex::CutsFactor, 1.0);
		for (int i = 0; i < R.Candidates(); i++)
			for (int j = 0; j < R.Candidates(); j++)
				if (i != j)
					lp_model.add(x_lp[i][j] + x_lp[j][i] == 1);
				else
					lp_model.add(x_lp[i][i] == 0);
		Lp_AddCycleConstraints();
		time_t start, end;
		time(&start);
		clock_t s,e;
		s = clock();
		if ( !lp_cplex.solve() ) {
			env.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}
		env.out() << "old formulation LP Solution status = " << lp_cplex.getStatus() << endl;
		env.out() << "Old formulation LP Solution value = " << lp_cplex.getObjValue() << endl;		
		cout << "The number of cuts are :" << lp_cplex.getNcuts(IloCplex::CutCover) << endl;
		e = clock();
		time(&end);
		double time_differ = difftime(end, start);
		rt_lp = diffclock(e,s);
		n_lp_constraint = lp_cplex.getNrows();
		cout << "The time is :" << lp_cplex.getCplexTime() << endl;;
		cout << "the clock time is :" << rt_lp << endl;
		cout << "The difftime is :" << time_differ << endl;
		//Adding target cuts
		cout << "calling target cuts :" << endl;
		GenerateTargetCuts();
		cout << "after cuts generation" << endl;
		if ( !lp_cplex.solve() ) {
		env.error() << "Failed to optimize LP." << endl;
		throw (-1);
		}
		cout<<"####################solved after adding target cuts "<<endl;
		//	env.out() << "old formulation LP Solution status = " << lp_cplex.getStatus() << endl;
		//	env.out() << "Old formulation LP Solution value = " << lp_cplex.getObjValue() << endl;
		val_lp = lp_cplex.getObjValue();
		rt_lp = diffclock(e,s);
		n_lp_constraint = lp_cplex.getNrows();
		//lp_cplex.clearModel();	
	}
	catch ( IloException & e){
		cerr << "Concert exception caught: " << e << endl;
	} catch (...){
		cerr << "Unknown exception caught" << endl;
	}
}

void OldILPSolver::Print_Constraints(IloCplex cplex, NumVarMatrix x)
{
	int counter =0;
	for(int i = 0; i < R.Candidates(); i++)
		for(int j =0 ; j < R.Candidates(); j++)
			for(int k =0; k < R.Candidates(); k++)
			{
				cout << "***************************************" << endl;
				cout << "constraint number " << counter++ << endl;
				cout << "i,j,k = "  << i << ", "<< j << " , "<< k << endl;
				cout << cplex.getValue(x[i][j]) << " + " << cplex.getValue(x[j][k]) << " + " << cplex.getValue(x[k][i]) << " <=  2" << endl;
				if (cplex.getValue(x[i][j])+ cplex.getValue(x[j][k]) + cplex.getValue(x[k][i]) > 2.01)
				{
					cerr << "find the violated constarint"<< endl;
					int a; 
					cin >> a; 
				}   
				cout <<"*******************************************" << endl;
			}
}
/*
   void OldILPSolver::Print_Constraints(IloCplex cplex, NumVarMatrix x, vector<pair<int,int> > fixings)
   {
   int counter =0;
   vector<pair<int, int> >::iterator it;
   for(it = fixings.begin(); it != fixings.end(); it++)
   {
   pair<int, int> p = *it;
   for(int i = 0; i < R.Candidates(); i++)
   if(i!=p.first && i!=p.second)
   {
   cout << "***************************************" << endl;
   cout << "constraint number " << counter++ << endl;
   cout << "i,j,k = "  << p.first << ", "<< p.second << " , "<< i << endl;
   cout << cplex.getValue(x[p.first][p.second]) << " + " << cplex.getValue(x[p.second][i]) << " + " << cplex.getValue(x[i][p.first]) << " <=  2" << endl;
   if ( cplex.getValue(x[p.second][i]) + cplex.getValue(x[i][p.first]) - 1 > RC_EPS)
   {
   cerr << "find the violated constarint"<< endl;
   int a; 
   cin >> a; 
   }   
   cout <<"*******************************************" << endl;
   }
   }
   }

*/



void OldILPSolver::AddOne_3_Dicycle_Constraint(int i, int j, int k)
{
	lp_model_iterat.add(x_lp[i][j] + x_lp[j][k] + x_lp[k][i] <= 2);
}



double OldILPSolver::Manual_Obj_Computation(IloCplex cplex)
{
	double obj = 0; 

	for (int i = 0 ; i < R.Candidates(); i++)
		for (int j = 0; j < R.Candidates(); j++)
			obj += cplex.getValue(x[i][j]*R.Get_m_Array()[j][i]);

	return obj;
}



int OldILPSolver::Exist_Violated_3_Dicycle()
	/*
	 * This function check if there is any violated 3-Dicycle
	 * if there is any then it will add the corresponding cut
	 * to the lp_iterat_model and return true
	 * otherwise it will return false
	 */
{
	//cout << "begin" << endl;  
	//bool flag = false;
	int nCut=1000, counter = 0;
	cout << "counter at the begining: " << counter << endl;
	//copy the x_lp matrix
	double **x_copy= new double* [R.Candidates()];
	for(int i =0; i < R.Candidates(); i++)
		x_copy[i] = new double[R.Candidates()]; 

	for(int i = 0; i < R.Candidates(); i++)
		for(int j =0; j < R.Candidates(); j++)
			x_copy[i][j] = lp_cplex_iterat.getValue(x_lp[i][j]);
	//add the violated constraints
	for (int i = 0; i < R.Candidates(); i++)
		for (int j = 0; j < R.Candidates(); j++)
			for (int k = 0; k < R.Candidates(); k++)
				//if ((i != j) && (j != k) && ( k != i))
				if (x_copy[i][j]+ x_copy[j][k]+ x_copy[k][i] -2 > RC_EPS  )
				{
					double violated = x_copy[i][j]+ x_copy[j][k]+ x_copy[k][i];
					//cout << x_copy[i][j] << " + " << x_copy[j][k] << " + " << x_copy[k][i] <<" > 2" << endl;
					//cout << violated << " > 2 " << endl;
					// add the cuts to the model
					lp_model_iterat.add(x_lp[i][j] + x_lp[j][k] + x_lp[k][i] <= 2);
					n_iterat_constraint++;
					//  check to see if ncut upper bound is reached
					// cout << "heloo" << endl;
					if (++counter > nCut) 
					{
						//   cout << "nconstraint is " << n_iterat_constraint << endl;
						//	cout << "const number is " << n_iterat_constraint << endl;
						for(int i = 0; i < R.Candidates(); i++)
							delete x_copy[i];
						delete x_copy;
						cout <<"counter is :" << counter << endl;
						return counter;
					}
				}
	for(int i = 0; i < R.Candidates(); i++)
		delete x_copy[i];
	delete x_copy;
	cout <<"counter isssss :" << counter << endl;
	return counter;

}




void OldILPSolver::PrintIteratLPResults()
{

	cout << "The Iterated LP solution is :" << lp_cplex_iterat.getObjValue() << endl;
	IloNumArray vals_lp(env);
	for (int i = 0; i < R.Candidates(); i++)
	{
		lp_cplex_iterat.getValues(vals_lp,x_lp[i]);
		cout << "x[" << i <<"]=" << vals_lp << endl;
	}
}




void OldILPSolver::SolveLPIterat()
	/*
	 * This function solves the Integr program of Kemeny 
	 * The 3-dicylce constraints added iteratively
	 * with the help of separation algorithm
	 */
{
	try
	{
		// Generate the objective function
		IloExpr obj_fun = Lp_CreateObjFunction();
		Lp_AddConstraints();/*add new constraints here*/
		lp_model_iterat.add(IloMaximize(env, obj_fun));
		//lp_cplex_iterat.setOut(env.getNullStream());
		lp_cplex_iterat.setParam(IloCplex::RootAlg, IloCplex::Dual);
		//track the running time
		time_t start, end;
		time(&start);
		clock_t s,e;
		s = clock();
		// solve the model
		//cout<<"#######################start"<<endl;
		if ( !lp_cplex_iterat.solve() ) {
			env.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}
		//cout<<"####################solved"<<endl;
		// PrintIteratLPResults();
		lp_cplex_iterat.exportModel("itlp.lp");

		// run separation and add constraints iteratively
		while (Exist_Violated_3_Dicycle() != 0)
		{		
			if ( !lp_cplex_iterat.solve() ) 
			{
				env.error() << "Failed to optimize LP." << endl;
				throw (-1);
			}
		}
		if ( !lp_cplex_iterat.solve() ) 
		{
			env.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}
		//compute the running time
		e = clock();
		//cout << "s is " << s << " and e is : " << e << endl;
		time(&end);
		//rt_lp_iterat = difftime(end, start);
		rt_lp_iterat = diffclock(e,s);
		cout << "iterat LP obj is " << lp_cplex_iterat.getObjValue() << endl;
		val_lp_iterat = lp_cplex_iterat.getObjValue();
		lp_cplex_iterat.clearModel();	
	}
	catch ( IloException & e)
	{
		cerr << "Concert exception caught: " << e << endl;
	} catch (...)
	{
		cerr << "Unknown exception caught" << endl;
	}
}


void OldILPSolver::Print_Constraints()
{
	ofstream file("../data/constraint.txt");
	for (int i = 0; i < R.Candidates(); i++)
		for (int j = 0; j < R.Candidates(); j++)
			for (int k = 0; k < R.Candidates(); k++)
			{
				double violated = lp_cplex_iterat.getValue(x_lp[i][j]) + lp_cplex_iterat.getValue(x_lp[j][k])+ lp_cplex_iterat.getValue(x_lp[k][i]);

				cout << lp_cplex_iterat.getValue(x_lp[i][j]) << " + " << lp_cplex_iterat.getValue(x_lp[j][k]) << " + " << lp_cplex_iterat.getValue(x_lp[k][i]) <<" > 2" << endl;
				cout << "i,j,k = "<< i << ","<< j << "," << k << " and " << violated << " > 2 " << endl;
				file << lp_cplex_iterat.getValue(x_lp[i][j]) << " + " << lp_cplex_iterat.getValue(x_lp[j][k]) << " + " << lp_cplex_iterat.getValue(x_lp[k][i]) <<" > 2" << endl;
				file << "i,j,k = "<< i << ","<< j << "," << k << " and " << violated << " > 2 " << endl;
			}
	file.close();
}




void OldILPSolver::TestLPIterat(){
	try{
		// Generate the objective function
		IloExpr obj_fun = Lp_CreateObjFunction();
		Lp_AddConstraints();               //add new constraints here
		lp_model_iterat.add(IloMaximize(env, obj_fun));
		//lp_cplex_iterat.setOut(env.getNullStream());
		lp_cplex_iterat.setParam(IloCplex::RootAlg, IloCplex::Dual);
		//track the running time
		time_t start, end;
		time(&start);
		clock_t s,e;
		s = clock();
		if ( !lp_cplex_iterat.solve() ) {
			env.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}
		// PrintIteratLPResults();
		lp_cplex_iterat.exportModel("itlp.lp");
		// run separation and add constraints iteratively
		/*
		   while (Exist_Violated_3_Dicycle() != 0)
		   {		
		   if ( !lp_cplex_iterat.solve() ) 
		   {
		   env.error() << "Failed to optimize LP." << endl;
		   throw (-1);
		   }
		   }*/
		Print_Constraints();
		if ( !lp_cplex_iterat.solve() ) 
		{
			env.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}
		e = clock();
		time(&end);
		//rt_lp_iterat = difftime(end, start);
		rt_lp_iterat = diffclock(e,s);
		cout << "iterat LP obj is " << lp_cplex_iterat.getObjValue() << endl;
		val_lp_iterat = lp_cplex_iterat.getObjValue();
		lp_cplex_iterat.clearModel();	
	}
	catch ( IloException & e){
		cerr << "Concert exception caught: " << e << endl;
	} catch (...){
		cerr << "Unknown exception caught" << endl;
	}
}

/*
void OldILPSolver::SolveDualbyDRP(vector<pair<int,int> > J){
	IloEnv env;
	IloModel model(env);
	IloCplex cplex(model);
	IloNumVarArray y(env);
	IloNumVarArray z(env);
	for(int i = 0; i < 12; i++)
		y.add(IloFloatVar(env,-IloInfinity,1,"z_(123)"));

}


*/












vector<int> OldILPSolver::Mapping(int subspace_size)
{
	vector<int> p;
	p.resize(subspace_size);
	for (int i =0; i < subspace_size; i++)
		p[i] = i;
	/*	 
		 bool* flag = new bool[R.Candidates()];
		 for(int i = 0; i < R.Candidates();i++)
		 flag[i] = false;
		 int n=0;
		 while(n<subspace_size)
		 {
		 int temp = rand() % R.Candidates();
		 if (!flag[temp])
		 {
		 p.push_back(temp);
		 flag[temp] = true;
		 n++;
		 }
		 }
		 cout  << p << endl << endl;;
		 delete[] flag;
		 */
	return p;
}


vector<vector<vector<int> > > OldILPSolver::GenerateExtremPointPCV(vector<int>& vars,int subspace_size, int nFeasPoints){
	vector<vector<vector<int> > > points;
	points.resize(nFeasPoints);
	for(int  i =0; i < nFeasPoints; i++)
		points[i].resize(subspace_size);
	for(int i =0; i < nFeasPoints; i++)
		for(int j =0; j < subspace_size; j++)
			points[i][j].resize(subspace_size);
	int* ordering= new int[subspace_size];
	for(int i = 0; i < subspace_size; i++)
		ordering[i] = vars[i];
	//create the point based on the above ordering
	for(int k = 0; k < nFeasPoints; k++)
	{
		//print ordering
		//cout << endl<< "/****** ordering *****/" << endl;
		//for(int i =0; i<subspace_size; i++)
		//	cout << ordering[i] << " ";
		//cout << endl << "/*********************** /"<< endl;

		//  cout << "k is " << k << endl;
		for(int i =0; i < subspace_size; i++)
			for(int j =0; j < subspace_size; j++){
				int rank_i = FindPosition(ordering, ordering[i], subspace_size);
				int rank_j = FindPosition(ordering, ordering[j], subspace_size);
				if(rank_i < rank_j){
					// cout << "first pos is :::" << FindPositionVector(vars,ordering[i],subspace_size) << endl;
					// cout << "the second pos is :::" << FindPositionVector(vars,ordering[j],subspace_size) << endl;
					points[k][FindPositionVector(vars,ordering[i],subspace_size)][FindPositionVector(vars,ordering[j],subspace_size)]= 1;
					points[k][FindPositionVector(vars,ordering[j],subspace_size)][FindPositionVector(vars,ordering[i],subspace_size)]= 0;
				}
				else{
					//cout << "first pos is :" << FindPositionVector(vars,ordering[i],subspace_size) << endl;
					//cout << "the second pos is :" << FindPositionVector(vars,ordering[j],subspace_size) << endl;
					points[k][FindPositionVector(vars,ordering[j],subspace_size)][FindPositionVector(vars,ordering[i],subspace_size)]= 1;
					points[k][FindPositionVector(vars,ordering[i],subspace_size)][FindPositionVector(vars,ordering[j],subspace_size)]= 0;
				}
			}
		//update the ordering
		for(int i = 0; i < subspace_size; i++){
			int temp = ordering[i];
			int index = i+ (int)rand()%((subspace_size-i));
			ordering[i] = ordering[index];
			ordering[index] = temp;
		} 
	}
	delete[] ordering;
	//cout << points;
	return points;
}

vector<vector<double> > OldILPSolver::GenerateQ(vector<vector<vector<int> > >& points,int subspace_size, int nFeasPoints){
	vector<vector<double> > q;
	q.resize(subspace_size);
	for(int i =0; i< subspace_size; i++)
		q[i].resize(subspace_size);
	for(int i =0; i < subspace_size; i++)
		for(int j =0; j < subspace_size; j++){
			int sum = 0;
			for(int k = 0; k < nFeasPoints; k++)
				sum+= points[k][i][j];
			q[i][j] = (float)sum / float(nFeasPoints);
		}
	cout << endl << "q is : "<< endl <<  q << endl;
	return q;
}

void print_vector(vector<vector<int> >s, int size){
	cout << "the vecotr i print function is : " << endl;
	for(int i = 0; i < size; i++){
		for(int j =0; j < size; j++)
			cout << s[i][j]<< " " ;
		cout << endl;
	}
}

bool OldILPSolver::Separation(vector<vector<vector<int> > >& points, 
		vector<vector<double> >& q, vector<vector<int> >& x,vector<vector<double> >& hyper_plane_norm, int subspace_size, 
		int nFeasPoints){
	// print_vector(x, subspace_size);
	//  for(int i =0; i < subspace_size; i++)
	//      for(int j =0; j < subspace_size; j++)
	//          x[i][j] = 1;
	// cout << "x* is " << endl << x << endl << endl;
	// cout << "q is " << endl << q << endl << endl;
	//create obj function
	NumVarMatrix a(env,subspace_size);	
	for (int i = 0; i < subspace_size; i++)
		a[i]= IloNumVarArray(env, subspace_size,0,1,ILOFLOAT); // the bound need to be examined to be true
	IloModel tcut_model(env);
	IloCplex tcut_cplex(tcut_model);
	tcut_cplex.setOut(env.getNullStream());

	try{
		IloExpr obj(env);
		for (int i = 0; i < subspace_size; i++)
			for(int j = 0; j < subspace_size; j++){
				obj += a[i][j]*(x[i][j] - q[i][j]);
				//cout << "obj -> i, j = " << i << ","<< j <<" and =  " << x[i][j]-q[i][j]  << endl;
			}
		tcut_model.add(IloMaximize(env,obj));
		//add contraints
		for (int i =0; i < subspace_size; i++)
			tcut_model.add(a[i][i] == 0);
		for(int k =0; k < nFeasPoints; k++){
			IloExpr lhs(env);
			for(int i =0; i < subspace_size; i++)
				for(int j =0; j < subspace_size; j++)
					//	if ((i != j) && (j != k) && ( k != i))
					lhs += a[i][j]*(points[k][i][j]-q[i][j]);
			tcut_model.add(lhs -1 <=0 );
		}
		tcut_cplex.exportModel("tcut.lp");
		//solve the model
		if ( !tcut_cplex.solve() ) {
			env.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}
		//cout << "before this is :" << endl;
		//cout   << tcut_cplex.getValue(a[0][0]) << endl;

		cout << endl << "The objective function in separation function is =  " << tcut_cplex.getObjValue() << endl;
		env.out() << "the separation solution status is  = " << tcut_cplex.getStatus() << endl;
		for(int i = 0; i < subspace_size; i++)
			for(int j =0; j < subspace_size; j++ )
				if (i != j){
					//cout << "i, j = " << i << ","<< j   << endl;
					hyper_plane_norm[i][j] = tcut_cplex.getValue(a[i][j]);

				}
				else 
					hyper_plane_norm[i][j] = 0;
		/*
		   for(int i = 0; i < subspace_size; i++){
		   for(int j =0; j < subspace_size; j++ ){
		   cout << endl << "i,j : "  << i << "," << j << " = ";
		   cout  <<  tcut_cplex.getValue(a[i][j]) << " ";
		   }
		   cout << endl ;
		   }*/

	}
	catch ( IloException & e){
		cerr << "Concert exception caught: " << e << endl;
	} catch (...){
		cerr << "Unknown exception caught" << endl;
	}
	//print the results
	if (tcut_cplex.getObjValue() <= 1){
		cout  << "This point is inside the polytop" <<  endl << endl;
		return true;
	}
	else{
		cout  << "the pooint is not inside the polytope until now, lets hope!!" << endl << endl;
		return false;
	}
}

bool OldILPSolver::Exact(vector<vector<double> >& hyper_plane_norm, vector<vector<vector<int> > >& points, int subspace_size){
	BoolVarMatrix x(env, subspace_size);
	for(int  i =0; i < subspace_size; i++)
		x[i] = IloBoolVarArray(env, subspace_size);
	IloModel model_lop(env);	
	IloExpr obj(env);
	for(int i = 0; i < subspace_size;i++)
		for(int j =0; j < subspace_size; j++)
			obj+= hyper_plane_norm[i][j]*x[i][j];
	model_lop.add(IloMaximize(env,obj));
	for (int i = 0; i < subspace_size; i++)
		for (int j = 0; j < subspace_size; j++){
			if (i != j){
				//	lp_model.add(x_lp[i][j] + x_lp[j][i] == 1);
				model_lop.add(x[i][j] + x[j][i] == 1);
			}
			else{
				//	lp_model.add(x_lp[i][i] == 0);
				model_lop.add(x[i][i] == 0);
			}
		}

	for (int i = 0; i < subspace_size; i++)
		for (int j = 0; j < subspace_size; j++)
			for (int k = 0; k < subspace_size; k++){
				model_lop.add(x[i][j] + x[j][k] + x[k][i] <= 2);
			}
	IloCplex cplex_lop(model_lop);
	cplex_lop.setOut(env.getNullStream());
	if ( !cplex_lop.solve() ) {
		env.error() << "Failed to optimize LP." << endl;
		throw (-1);
	}
	cplex_lop.exportModel("subproblem.lp");
	cout << endl << "The objective in Exact  model is : " << cplex_lop.getObjValue() << endl;
	cout << "The soution in exact method status is = " << cplex_lop.getStatus() << endl;
	//print the results
	if (cplex_lop.getObjValue() <= 1){     // max ax then cplex.getObjValue() < b
		cout << "no point of lop convex hull  found that violate the hyper_plane norm" << endl << endl ;
		return false;
	}
	else{
		cout << "There is a point from lop convex hull that violated the hyper_plane norm " << endl << endl;
		//add the point
		vector<vector<int> > p;
		p.resize(subspace_size);
		for(int i=0; i < subspace_size; i++)
			p[i].resize(subspace_size);
		for(int i = 0; i < subspace_size; i++)
			for(int j =0; j < subspace_size; j++)
				p[i][j] = cplex_lop.getValue(x[i][j]);
		points.push_back(p);
		return true;
	}
}

void UpdateQ(vector<vector<vector<int> > >& points, vector<vector<double> >& q, int subspace_size, int nFeasPoints){
	for(int i =0; i < subspace_size; i++)
		for(int j =0; j < subspace_size; j++){
			int sum = 0;
			for(int k = 0; k < nFeasPoints; k++)
				sum+= points[k][i][j];
			q[i][j] = (double)sum / double(nFeasPoints);
		}
	cout << "q is : "<< endl <<  q << endl;
}

vector<int> OldILPSolver::Mapping(){
	vector<int> v;
	v.resize(6);
	v[0] = 48;
	v[1] = 61;
	v[2] = 9;
	v[3] = 53;
	v[4] = 65;
	v[5] = 39;
	/*int size;
	  cout << "please enter the size of the projection subspace" << endl;
	  cin >> size;
	  int i =0;
	  while(i++ < size)
	  {
	  int temp;
	  cout << endl << "enter the variable number" << endl;
	  cin >> temp;
	  v.push_back(temp);	
	  }*/
	return v;
}

void OldILPSolver::GenerateTargetCuts(){
	srand(time(NULL));
	int subspace_size =20;
	int nFeasPoints= 7;
	static int nTargetCuts=0;
	int counter = 0;
	// create x* from x_lp
	vector<vector<int> > x_lp_copy;
	x_lp_copy.resize(R.Candidates());
	for(int i =0; i < R.Candidates(); i++)
		x_lp_copy[i].resize(R.Candidates());
	for(int i = 0; i < R.Candidates(); i++)
		for(int j =0; j < R.Candidates(); j++ )
			x_lp_copy[i][j] = lp_cplex.getValue(x_lp[i][j]);
	//while (true)
	while (counter++ < 20){
		cout << endl <<  "*************************************************" << endl;
		cout << "The  " << counter << "'s projection is " << endl;
		vector<int> proj_subspace = Mapping(subspace_size);
		//vector<int> proj_subspace = Mapping();
		vector<vector<vector<int> > > points = GenerateExtremPointPCV(proj_subspace, subspace_size, nFeasPoints);
		vector<vector<double> > q = GenerateQ(points, subspace_size, nFeasPoints);
		// select the proper variables from vector x (x_lp)
		vector<vector<int> > x;
		x.resize(subspace_size);
		for(int i =0; i < subspace_size; i++)
			x[i].resize(subspace_size);		
		for(int i = 0; i < subspace_size; i++)
			for(int j =0; j < subspace_size; j++ )
				x[i][j] = x_lp_copy[proj_subspace[i]][proj_subspace[j]];
		//copy the norm to a vector
		vector<vector<double> > hyper_plane_norm;
		hyper_plane_norm.resize(subspace_size);
		for(int i =0; i< subspace_size; i++)
			hyper_plane_norm[i].resize(subspace_size);
		//cout << "check here " << endl;
		//target cut generation process
		int map_counter=0;
		while(true){
			bool is_inside_polytope = Separation(points,q, x, hyper_plane_norm, subspace_size, nFeasPoints);
			if (is_inside_polytope){
				//cout << "x* is inside the projected polytope, change the projection" << endl;
				break;
			}
			else{
				//cout << "need more test to be sure if the norm is a separation" << endl;
				bool find_violated_point = Exact(hyper_plane_norm, points, subspace_size);
				if(!find_violated_point){
					cout << "a cut is found" << endl;
					nTargetCuts++;
					IloExpr expr(env);
					for(int i = 0; i < subspace_size; i++)
						for(int j =0; j < subspace_size; j++)
							expr+= hyper_plane_norm[i][j]*x_lp[proj_subspace[i]][proj_subspace[j]];
					lp_model.add(expr <= 1);
					break;
				}
			}
			//update q
			nFeasPoints++;
			UpdateQ(points, q, subspace_size, nFeasPoints);
		}
		cout << endl <<  "****************** End of " << counter << "'s projection **************" << endl;
	}
	cout << "the number of added target cuts is :" << nTargetCuts << endl; 
}


