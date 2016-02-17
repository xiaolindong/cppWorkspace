/*
 * ufl_colgen.cpp
 *
 *  Created on: May 29, 2014
 *      Author: eia2
 */
#include "ufl_colgen.h"




UFLColGenSolver::~UFLColGenSolver()
{
	//subsets.~vector();
	env.end();

}




void UFLColGenSolver::PrintResult()
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



void UFLColGenSolver::AddCuts()
{
	IloExpr e(env);
	e += Cut[2];
	eOpt.add(e == 0);
}

void UFLColGenSolver::InitSubSets()
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
 
		subsets.push_back(temp);
		temp.clear();
	}


}

void PrepareRHS(IloNumArray &amount, int k)
{

	amount[0]= -k;
	amount[1] = 1;
}



ostream& operator <<(ostream& out,  const UFLColGenSolver& s)
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


void InitialToZero(IloNumArray colCoef, int size)
{
	for (int i = 0; i < size; i++)
		colCoef[i] = static_cast<IloNum>(0.0);
}


/*

ostream& operator <<(ostream& out,  const vector<int> s)
{
	vector<int>::const_iterator i;
	for(i = s.begin(); i != s.end(); i++)
		out << *i+1 << " ";  // increase by 1 in order to match the convention indexing scheme
	out << endl;
	return out;
}
*/

inline int ComputeBeginIndexInDualArray( int c, int n) { return n + n*c +1;}


inline int ComputeEndIndexInDualArray(int c, int n) { return n + (c+1)*n ;}




double CalcColumnCost(const election& E, vector<int> solution)
{
	//cout << "vector size is : " << solution.size() << endl << endl;
	double cost = 0;
	for (int j= 0; j < E.Voters(); j++)
	{
		vector<int>::iterator i;
		int min_cost = 100000000;
		int min_index = -1;
		for (i = solution.begin(); i != solution.end(); i++)
			if (min_cost > E.Ranking()[*i][j])
			{
				min_cost = E.Ranking()[*i][j];
				min_index = *i;
			}
		assert (min_index != -1);
		//cout << "min index is : " << min_index << endl<< endl;
		cost += E.Ranking()[min_index][j]-1;
	}
	cout << endl << "The subset is :" << solution << " and the cost is : " << cost << endl;
	return cost;
}





double UFLColGenSolver::SolveLP()

{

	double solution; 
	try
	{
		int nRHS = 2;
		IloNumArray rhs(env,2);
		PrepareRHS(rhs, E.K());
		IloRangeArray constraints = IloAdd (eOpt, IloRangeArray(env, rhs,IloInfinity));
		IloObjective   obj = IloAdd(eOpt, IloMinimize(env));

		int **ranking = E.Ranking();
		// create the initial columns
		// add the initial columns to master problem
		// solve the subproblem

		// add initial columns
		vector<vector<int> >::iterator it_i;
		for(it_i = subsets.begin(); it_i != subsets.end();it_i++)
		{
		//	cout << *it_i << endl;
			double col_cost = CalcColumnCost(E,*it_i);
			IloNumArray Columns(env, nRHS);
			InitialToZero(Columns, nRHS); // pick the
			Columns[0] = static_cast<IloNum>(-1.0*(*it_i).size());
			Columns[1] = static_cast<IloNum>(1.0);
			Cut.add((obj(col_cost) + constraints(Columns)));
			Columns.clear();
		}

		int t =0;
		// column generation process
		for(;;)
		{	
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
			eSolver.exportModel("lpx.lp");

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
			vector<int> newCol = SubProblemSolving(price);
			cout << "new entering column is :" << newCol << endl;
		
			//stoping criteria
			if (CalcColumnCost(E,newCol) +  price[0]*newCol.size() - price[1] >= 0) break;

			//need to check if this subset is already in the vector

			//add the new column to the model
			subsets.push_back(newCol); 		
			size_subsets++;						// increase the size of subsets
			vector<int> ::iterator it;
			IloNumArray newSubset(env, nRHS);
			InitialToZero(newSubset, nRHS);
			newSubset[0] = static_cast<IloNum>(-1.0*newCol.size());
			newSubset[1] = static_cast<IloNum>(1.0);	
			Cut.add((obj(CalcColumnCost(E,newCol))+ constraints(newSubset)));
			newSubset.clear();
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





IloExpr Subproblem_Generate_Obj_Fun(const election& E, IloEnv env,BoolVarMatrix x,IloBoolVarArray y, IloNumArray price)
/**************************************************/
/* This Function generates the objective function */
/**************************************************/
{

	IloExpr e(env);
	for(int i = 0; i < E.Candidates() ; i++)
		for(int j = 0; j < E.Voters() ; j++)
			e += (E.Ranking()[i][j]-1) * x[i][j] ;
	for (int i = 0; i < E.Candidates(); i++)
		e += price[0]*y[i];
	e += -price[1];
	return e;

}



void Subproblem_AddConstarint(const election& E,IloEnv env, IloModel &model, IloBoolVarArray y, BoolVarMatrix x)
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



vector<int> UFLColGenSolver::SubProblemSolving(IloNumArray price)
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
		Subproblem_AddConstarint(E,env,model,  y, x);
		IloExpr obj_fun = Subproblem_Generate_Obj_Fun(E,env,x,y,price);
		model.add(IloMinimize(env, obj_fun));
				
		//solve  the model
		IloCplex cplex(model);
		if ( !cplex.solve() ) {
			env.error() << "Failed to optimize LP." << endl;
			throw(-1);
		}

		// extract the new column
		IloNumArray vals(env);
		env.out() << "Solution status = " << cplex.getStatus() << endl;
		env.out() << "Solution value = " << cplex.getObjValue() << endl;
		
		//print the solution
		/*
		for (int i = 0; i < nCandidates; i++){
			cplex.getValues(vals,x[i]);
			cout << "x[" << i <<"]=" << vals << endl;
		}
		*/
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


   class election::UFLColGenSolver::subsetIterator
   {
   const UFLColGenSolver &solver;

   int index, size;

   public:

   subsetIterator(const UFLColGenSolver &s):
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





