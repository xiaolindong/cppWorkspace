/*
 * colgen.cpp
 *
 *  Created on: Jan 14, 2014
 *      Author: eia2
 */
#include "colgen.h"

/*


ColGenSolver::~ColGenSolver()
{
	//subsets.~vector();
	env.end();

}




void ColGenSolver::PrintResult()
{
	cout << "colgen LP optimal  value is :" << eSolver.getObjValue() << endl;
	E.eFile() <<  "colgen LP optimal  value is :" << eSolver.getObjValue() << endl;
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



void ColGenSolver::AddCuts()
{
	IloExpr e(env);
	e += Cut[2];
	eOpt.add(e == 0);
}

void ColGenSolver::InitSubSets()
{
//	srand (time(0));
	int bound = nK_combination(E.Candidates(), E.K());
	int temp[E.K()];
	for(int i = 0; i < E.K(); i++)
		temp[i] = -1;
	int index = 0;
	while (index < size_subsets)
	{
		int r = rand() % bound;
		if (!BelongToArray(temp,index,r))
		{
			temp[index++] = r;
			//cout << "r is : " << r << endl;
			K_Subset_Lex_Unrank(subsets,r,E.Candidates(),E.K());
		}
	}

}

void PrepareRHS(IloNumArray &amount, int nRHS, int v, int k)
{

	for (int i = 0; i < nRHS; i++)
		amount[i] = 0;
	for(int i  = 0; i < v; i++)
		amount[i] = 1;
	amount[v] = -1;
}



ostream& operator <<(ostream& out,  const ColGenSolver& s)
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


ostream& operator <<(ostream& out,  const vector<int> s)
{
	vector<int>::const_iterator i;
	for(i = s.begin(); i != s.end(); i++)
		out << *i+1 << " ";  // increase by 1 in order to match the convention indexing scheme
	out << endl;
	return out;
}


inline int ComputeBeginIndexInDualArray( int c, int n) { return n + n*c +1;}


inline int ComputeEndIndexInDualArray(int c, int n) { return n + (c+1)*n ;}

double ColGenSolver::SolveLP()
{
	double solution; 
	try
	{
		int nRHS = E.Voters()*E.Candidates() + E.Voters() + 1;
		IloNumArray rhs(env,nRHS);
		PrepareRHS(rhs, nRHS, E.Voters(), E.K());
		IloRangeArray constraints = IloAdd (eOpt, IloRangeArray(env, rhs, IloInfinity));
		IloObjective   obj = IloAdd(eOpt, IloMinimize(env));
		//	IloNumVarArray Cut(env);

		int **ranking = E.Ranking();
		int nFixedCols = E.Voters() * E.Candidates();
		for (int i = 0; i < nFixedCols; i++)
		{
			int c = i / E.Voters(), v = i % E.Voters();
			Cut.add(IloNumVar(obj(ranking[c][v]-1) + constraints[E.Voters()+ (i+1)](-1)+ constraints[i % E.Voters()](1) ));
		}

		// add z's columns

		vector<vector<int> >::iterator it_i;
		for(it_i = subsets.begin(); it_i != subsets.end();it_i++)
		{
			vector <int>::iterator it_j;
			IloNumArray zColumns(env, nRHS);
			InitialToZero(zColumns, nRHS); // pick the
			zColumns[E.Voters()] = static_cast<IloNum>(-1.0);
			for (it_j = (*it_i).begin(); it_j != (*it_i).end(); it_j++)
			{
				int temp = *it_j;
				int beg_indx = ComputeBeginIndexInDualArray(temp,E.Voters());
				int end_indx = ComputeEndIndexInDualArray(temp, E.Voters());
				for (int i = beg_indx; i <= end_indx; i++)
					zColumns[i] = static_cast<IloNum>(1.0);
			}

			Cut.add(static_cast<IloNumVar>(obj(0) + constraints(zColumns)));

			zColumns.clear();
		}

		for(;;)
		{
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
			//eSolver.exportModel("lpx.lp");


			IloNumArray price(env, nRHS);
			for (int i = 0; i < nRHS; i++)
			{
				price[i] = eSolver.getDual(constraints[i]);
			}

		
//			   cout << "price is :" << endl;
//			   for(int i = 0; i < nRHS; i++)
//			   cout << price[i] << " ";
//			   cout<< endl;


			vector<int> newCol = SubProblemSolving(price);
			cout << "new entering column is :" << newCol << endl;
			vector<int> ::iterator i;
			IloNum sum = 0;
			double lambda = static_cast<double>(price[E.Voters()]);
			for (i = newCol.begin(); i != newCol.end(); i++)
			{
				int beg_indx = ComputeBeginIndexInDualArray(*i,E.Voters());
				int end_indx = ComputeEndIndexInDualArray(*i, E.Voters());
				for (int index = beg_indx; index <= end_indx; index++)
					sum += price[index];
			}
			cout << "sum is : " << sum << " and lambda is " << lambda << endl;
		//	if (sum - lambda <= 0) break;  // need more thinking , might cause problem
			if (lambda - sum >= -RC_EPS) break;

			//need to check if this subset is already in the vector


			subsets.push_back(newCol); 			//add new subset to set of columns
			size_subsets++;						// increase the size of subsets
			vector<int> ::iterator it;
			IloNumArray newSubset(env, nRHS);
			InitialToZero(newSubset, nRHS);
			newSubset[E.Voters()] = static_cast<IloNum>(-1.0);
			for (it = newCol.begin(); it != newCol.end(); it++)
			{
				int temp = *it;
				int beg_indx = ComputeBeginIndexInDualArray(temp,E.Voters());
				int end_indx = ComputeEndIndexInDualArray(temp, E.Voters());
				for (int i = beg_indx; i <= end_indx; i++)
					newSubset[i] =static_cast<IloNum>(1.0);
			}
			Cut.add(static_cast<IloNumVar>(obj(0)+ constraints(newSubset)));
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
	return solution;
	env.end();

}






vector<int> ColGenSolver::SubProblemSolving(IloNumArray price)
{
	double dualVarSumCandidArray[E.Candidates()];

	for(int i = 0; i < E.Candidates(); i++)
	{
		double sum  = 0;
		int beg_indx = ComputeBeginIndexInDualArray(i,E.Voters());
		int end_indx = ComputeEndIndexInDualArray(i, E.Voters());
		for (int index = beg_indx; index <= end_indx; index++ )
		{
			sum += price[index];
		}
		dualVarSumCandidArray[i] = sum;
	}

	vector<int> enteringSubset;

	int candid[E.Candidates()];
	for (int i = 0; i < E.Candidates(); i++)
		candid[i] = i;

	sort(dualVarSumCandidArray,candid,E.Candidates());

	for (int i = 0; i < E.K(); i++)
		enteringSubset.push_back(candid[i]);

	return enteringSubset;


}




   class election::ColGenSolver::subsetIterator
   {
   const ColGenSolver &solver;

   int index, size;

   public:

   subsetIterator(const ColGenSolver &s):
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

   };*/










