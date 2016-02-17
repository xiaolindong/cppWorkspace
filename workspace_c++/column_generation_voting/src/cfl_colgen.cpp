/*
 * cfl_colgen.cpp
 *
 *  Created on: May 29, 2014
 *      Author: eia2
 */
#include "cfl_colgen.h"




CFLColGenSolver::~CFLColGenSolver()
{
	//subsets.~vector();
	env.end();

}




void CFLColGenSolver::PrintResult()
{
	cout << "ufl_colgen LP optimal  value is :" << eSolver.getObjValue() << endl;
	E.eFile() <<  "ufl_colgen LP optimal  value is :" << eSolver.getObjValue() << endl;
	IloNumArray vals_lp(env);
	eSolver.getValues(Cut,vals_lp);
	cout << "x= " << vals_lp << endl;
	E.eFile() << " x = " << vals_lp << endl;
	vals_lp.clear();
	
	cout << "The final subsets are : " << endl;
	E.eFile() << "The final subsets are :" << endl;
	vector< vector<int> >::const_iterator it_i;
	for(it_i = subsets.begin(); it_i != subsets.end();it_i++)
	{
		vector <int>::const_iterator it_j;
		for (it_j = it_i->begin(); it_j != it_i->end(); it_j++)
		{
			cout << *it_j+1 << " "  ; //  *it_j+1 --> increase the candidates range by 1 for proper showing
			E.eFile() << *it_j+1 << " "  ; //  *it_j+1 --> increase the candidates range by 1 for proper showing

		}
		cout << endl;
		E.eFile() << endl;
	}



}



void CFLColGenSolver::AddCuts()
{
	IloExpr e(env);
	e += Cut[2];
	eOpt.add(e == 0);
}

void CFLColGenSolver::InitSubSets()
{
	double alt[E.Candidates()];
	for (int i = 0; i < E.Candidates(); i++ )
		alt[i] = i;
	
	for (int i = 0; i < size_subsets; i++)
	{
		for (int j =0; j < E.Candidates(); j++)
		{
			int r = static_cast<int>((E.Candidates()-j)*(rand()/(RAND_MAX + 1.0)) + j );
			int t= alt[j];
			alt[j] = alt[r];
			alt[r] = t;
		}
		int b = rand() % E.Candidates()+1;
		sort(alt,b);
		vector<int> temp;
		for (int j = 0; j < b; j++)
			temp.push_back(static_cast<int>(alt[j]));
		//Need to implement!!!  Test to see a redundant subset does not add 

		// find the best candidate for each voter
		


	 
		subsets.push_back(temp);
		temp.clear();
	}


}

void CFL_PrepareRHS(IloNumArray &amount, int nRHS, int k)
{

	amount[0]= -k;
	amount[1] = 1;
	for (int i = 2; i < nRHS; i++)
		amount[i] = 0;
}



ostream& operator <<(ostream& out,  const CFLColGenSolver& s)
{
	out << endl;
	vector< vector<int> >::const_iterator it_i;
	for(it_i = s.subsets.begin(); it_i != s.subsets.end();it_i++)
	{
		vector <int>::const_iterator it_j;
		for (it_j = it_i->begin(); it_j != it_i->end(); it_j++)
		{
			out << *it_j+1 << " "  ; //  *it_j+1 --> increase the candidates range by 1 for proper showing
		}
		out << endl;
	}
	return out;
}


void CFL_InitialToZero(IloNumArray colCoef, int size)
{
	for (int i = 0; i < size; i++)
		colCoef[i] = static_cast<IloNum>(0.0);
}




inline int CFL_ComputeBeginIndexInDualArray( int c, int n) { return n + n*c +1;}


inline int CFL_ComputeEndIndexInDualArray(int c, int n) { return n + (c+1)*n ;}





double CFL_CalcColumnCost(const election& E, vector<vector<int> > solution)
{
	double cost;
	return cost;
}


double CFL_CalcColumnCost(const election& E, vector<int> solution, vector <int>& assignment)
{
	//cout << "vector size is : " << solution.size() << endl << endl;
	double cost = 0;
	for (int j= 0; j < E.Voters(); j++)
	{
		vector<int>::iterator i;
		int min_cost = 100000000;
		int min_index = -1;
		//cout << "solution in calc is " << solution << endl; 
		for (i = solution.begin(); i != solution.end(); i++)
		{	
			//cout << "*i is :" << *i << " and j is : " << j << endl;
			if (min_cost > E.Ranking()[*i][j])
			{
				min_cost = E.Ranking()[*i][j];
				min_index = *i;
			}
		}
		assert (min_index != -1);
		//cout << "min index is : " << min_index << endl<< endl;
		cost += E.Ranking()[min_index][j]-1;
		assignment.at(j) = min_index;
		//cout << "min index is : " << min_index << endl;

	}
	//cout << endl << "The subset is :" << solution << " and the assignment is : " << assignment << endl;
	return cost;
}




void CFL_VectorInitial(vector <int> a)
{
	vector<int>::iterator it;
	for (it = a.begin(); it != a.end(); it++)
		(*it) = 0;

}

int CFL_nAssignedVoter(int c, vector<int> assignment)
{
	int n = 0;
	vector<int>::iterator it;
	for (it= assignment.begin(); it != assignment.end(); it++)
		if (*it == c)
			n++;
	return n;
}




double CFLColGenSolver::SolveLP()
{

	double solution; 
	try
	{
		int nRHS = 2*E.Candidates()+2;
		IloNumArray rhs(env,nRHS);
		CFL_PrepareRHS(rhs,nRHS, E.K());
		IloRangeArray constraints = IloAdd (eOpt, IloRangeArray(env, rhs,IloInfinity));
		IloObjective   obj = IloAdd(eOpt, IloMinimize(env));

		// create the initial columns
		// add the initial columns to master problem
		// solve the subproblem

		// add initial columns
		vector<int> assignment(E.Voters());   // keep the assignment of voters to candidates
		vector<vector<int> >::iterator it_i;
		for(it_i = subsets.begin(); it_i != subsets.end();it_i++)
		{
			cout << " column is : " << *it_i << endl << endl;
			CFL_VectorInitial(assignment);
			//cout << "assign before is L: " << assignment << endl;
			double col_cost = CFL_CalcColumnCost(E,*it_i,assignment);
			//cout << "assign after is :" << assignment << endl;
			IloNumArray Columns(env, nRHS);
			CFL_InitialToZero(Columns, nRHS); // pick the
			Columns[0] = static_cast<IloNum>(-1.0*(*it_i).size());
			Columns[1] = static_cast<IloNum>(1.0);
			vector<int>::iterator it_j;
			for (it_j = (*it_i).begin(); it_j != (*it_i).end(); it_j++)
			{
				//cout << "c is : " << *it_j <<" and assignment is : " << assignment << endl;  
				int a_j = CFL_nAssignedVoter((*it_j),assignment);
				//cout << "a_" << *it_j << " is " << a_j << endl;
				Columns[*it_j + 2] = static_cast<IloNum>(a_j - floor(E.Voters()/E.K()));
				Columns[*it_j + E.Candidates() + 2]= static_cast<IloNum>(ceil(E.Voters()/E.K()) - a_j);
			}
			Cut.add((obj(col_cost) + constraints(Columns)));
			Columns.clear();
		}

		
		static int t = 0;
		// column generation process
		for(;;)
		{
			cout << "******** iteration " << t++ << " ********" << endl;
			eSolver.exportModel("lpx.lp");	
			// solve master problem
			if ( !eSolver.solve() ) {
				env.error() << "Failed to optimize LP." << endl;
				throw(-1);
			}
			cout << "objective function value is :" << eSolver.getObjValue() << endl;
			solution = eSolver.getObjValue();
			IloNumArray vals_lp(env);
			env.out() << "Column Gen Solution status = " << eSolver.getStatus() << endl;
			env.out() << "Column Gen value = " << eSolver.getObjValue() << endl;
			eSolver.getValues(Cut,vals_lp);
			cout << "x= " << vals_lp << endl;
		//	eSolver.exportModel("lpx.lp");

			//extract dual values
			IloNumArray price(env, nRHS);
			for (int i = 0; i < nRHS; i++)
			{
				price[i] = eSolver.getDual(constraints[i]);

			}

			
			 //cout << "price is :" << endl;
			 //for(int i = 0; i < nRHS; i++)
			 //cout << price[i] << " ";
			 //cout<< endl;

			//solve subproblem and get a new column
			double reduced_cost = 0;
			CFL_VectorInitial(assignment);	
			vector<int> newSubset = SubProblemSolving(price,assignment,reduced_cost);
			double col_cost = CFL_CalcColumnCost(E,newSubset,assignment);

		
			//stoping criteria
			if (reduced_cost >= 0) break;

			//need to check if this subset is already in the vector

			//add the new column to the model
			cout << "The new subset-solution is : " << newSubset << endl;
			subsets.push_back(newSubset); 		
			size_subsets++;						// increase the size of subsets
			IloNumArray newCol(env, nRHS);
			CFL_InitialToZero(newCol, nRHS);
			newCol[0] = static_cast<IloNum>(-1.0*newSubset.size());
			newCol[1] = static_cast<IloNum>(1.0);
			vector<int>::iterator it;
			for (it = newSubset.begin(); it != newSubset.end(); it++)
			{
				int a_j = CFL_nAssignedVoter(*it,assignment);
				newCol[*it + 2] = static_cast<IloNum>(a_j - floor(E.Voters()/E.K()));
				newCol[*it + E.Candidates() + 2] = static_cast<IloNum>(ceil(E.Voters()/E.K()) - a_j);
			}	
			Cut.add((obj(col_cost)+ constraints(newCol)));
			newCol.clear();
			
		}
	}
	catch (IloException& ex)
	{
		cerr << "Error: " << ex << endl;
	}
	catch (...)
	{
		cerr << "Error" << endl;
	}
	E.SetValColgen(static_cast<double>(solution));
	val_lp = solution;
	PrintResult();
	env.end();
	return solution;
}





IloExpr CFL_Subproblem_Generate_Obj_Fun(const election& E, IloEnv env,BoolVarMatrix x,IloBoolVarArray y, IloNumArray price)
/**************************************************/
/* This Function generates the objective function */
/**************************************************/
{

	IloExpr e(env);
	for(int i = 0; i < E.Candidates() ; i++)
		for(int j = 0; j < E.Voters() ; j++)
			e += ((E.Ranking()[i][j]-1)-price[i+2]-price[i+E.Candidates()+2]) * x[i][j] ;
	for (int i = 0; i < E.Candidates(); i++)
		e += ( price[0] + (price[i+2]*floor(E.Voters()/E.K())) - (price[i+E.Candidates()+2] * ceil(E.Voters()/E.K())))*y[i];
	e -= price[1];
	return e;

}



void CFL_Subproblem_AddConstraint(const election& E,IloEnv env, IloModel &model, IloBoolVarArray y, BoolVarMatrix x)
/****************************************************/
/* This function add constarints to the formulation */
/****************************************************/

{
	IloExpr constraint(env);
	for (int j = 0; j < E.Voters(); j++){
		IloExpr constraint(env);
		for(int i = 0; i < E.Candidates(); i++){
			constraint+=x[i][j];
		}
		model.add(constraint == 1);
	}
	for(int i = 0; i < E.Candidates(); i++){
		for (int j = 0; j < E.Voters(); j++){
			model.add(x[i][j]-y[i] <= 0);
		}
	}
}



vector<int> CFLColGenSolver::SubProblemSolving(IloNumArray price, vector<int>& assignment, double& reduced_cost)
{

	IloEnv env;
	vector<int> col;
	try {
		IloModel model(env);
	
		// IP variable definition
		IloBoolVarArray y(env,E.Candidates());
		BoolVarMatrix x(env,E.Candidates());
		for (int i = 0; i < E.Candidates(); i++)
				x[i]= IloBoolVarArray(env, E.Voters());

		//add constraints and objective function
		CFL_Subproblem_AddConstraint(E,env,model,  y, x);
		IloExpr obj_fun = CFL_Subproblem_Generate_Obj_Fun(E,env,x,y,price);
		model.add(IloMinimize(env, obj_fun));
				
		//solve  the model
		IloCplex cplex(model);
		if ( !cplex.solve() ) {
			env.error() << "Failed to optimize LP." << endl;
			throw(-1);
		}

		// display the results
		IloNumArray vals(env);
		env.out() << "Solution status = " << cplex.getStatus() << endl;
		env.out() << "Solution value = " << cplex.getObjValue() << endl;
		reduced_cost = cplex.getObjValue();
	 	
		// fill the assignment array
		for (int i = 0; i < E.Candidates(); i++)
		{
			cplex.getValues(vals,x[i]);
			for (int j = 0; j < E.Voters(); j++)
				if (vals[j])
					assignment[j] = i;
		}
		vals.clear();
		
		//extract the entering column "col"
		cplex.getValues(vals,y);	
		for (int i = 0 ; i < E.Candidates(); i++)
		{
				int s;
				if (vals[i])
					col.push_back(i);
		}
	
	}
	catch (IloException& e) {
		cerr << "Concert exception caught: " << e << endl;
	}
	catch (...) {
		cerr << "Unknown exception caught" << endl;
	}
	env.end();
	return col;

}

/*


   class election::CFLColGenSolver::subsetIterator
   {
   const CFLColGenSolver &solver;

   int index, size;

   public:

   subsetIterator(const CFLColGenSolver &s):
   solver(s), size(s.nSubsets()), index(-1){}

   int begin()
   {
   index = -1;
   return nxt();
   }
   int nxt()
   {
   if (end() == false)
   return ++index;
   return -1;
   }

   bool end(){return index >= size;}

   };




*/





