#include <ilcplex/ilocplex.h>
#include <string>
#include <sstream>
#include <list>

#include "variables.h"
#include "reader.h"

ILOSTLBEGIN

bool AddNewSubsetExhaustive(IloNumArray duals);
bool AddNewSubsetGreedy(IloNumArray duals);
double MST(vector<int> subset);
static void printSubsets();

int main()
{
   readfile();

   service_subsets.resize(num_worker + num_job);
   subsets_matrix.resize(num_worker + num_job);

//----------- Find initial Subsets ----------------------//
   IloEnv   env;
   IloModel model(env);

   IloNumVarArray var(env);
   IloRangeArray con(env);

   IloObjective obj = IloMinimize(env);

   for (int i=0; i<num_worker; i++){					// for each driver
	  con.add(IloRange(env, -1.0, IloInfinity, "d")); //con.add(IloRange(env, -IloInfinity, 1.0, "d"));  // sum of xiT <= 1
   }

   for (int j=1; j<=num_job; j++){						// for each job
	   con.add(IloRange(env, 1.0, IloInfinity, "j"));	// sum of xiT >=1
   }

   for (int i=0; i<num_worker; i++){					// creating initial subsets
	   subsets_matrix[i].push_back(1);
	   for (int j=1; j<=num_job; j++){					// puts all jobs that driver i can take ...
		   if (serviceable[i][j]){						// if i can service j
			   service_subsets[i].push_back(j);     	// add it to the service_subset[i]
			   subsets_matrix[i].push_back(1);
		   }
		   else
			   subsets_matrix[i].push_back(0);
	   }
  		cost_of_subset.push_back(MST(service_subsets[i]));
	}
   for (int j=1; j<=num_job; j++){
	   subsets_matrix[num_worker+j-1].push_back(1);
	   for (int i=1;i<=num_job; i++){
		   if (i==j){
			   service_subsets[num_worker+j-1].push_back(i);     	// add it to the service_subset[i]
			   subsets_matrix[num_worker+j-1].push_back(1);
		   }
		   else
			   subsets_matrix[num_worker+j-1].push_back(0);
	   }
		cost_of_subset.push_back(MST(service_subsets[num_worker+j-1]));
  }

   // -------- find all subsets -------------//
   // ---- do not use .. for testing only ---//
   /*find_all_subsets();
   for (unsigned int i=0; i<all_subsets.size(); i++){
	   double d = costOfTree(all_subsets[i]);
	   all_subsets_cost[i] = d;

//	   for (unsigned int j=0; j<all_subsets[i].size(); j++)
//		   cout << all_subsets[i][j]<<".";
//	   cout <<all_subsets_cost[i]<<endl;
   }*/

   // ---------------------------------------//

   printSubsets();

// ---------- Make formulation ---------------------- //

   IloNumArray newPatt(env);     // create new column constraint

   cout << "Generating Columns: "<<endl;
   for (unsigned int s=0; s<service_subsets.size(); s++){      		// for each subset S in the service_subsets
	   for (int i=0; i<num_worker; i++){			  				// for each worker i
		   bool b = true;
		   for(unsigned int j = 0; j < service_subsets[s].size(); j++){
			   if(!serviceable[i][service_subsets[s][j]])
			   {
				   b = false;
				   break;
			   }
		   }
		   if(b){			// check if i can service subset S		   if(serviceable[i][service_subsets[s][0]]){			// check if i can service subset S
			   for (int w=0; w<num_worker; w++){

				   if (i==w){
					   cout << 1;
					   newPatt.add((IloNum)-1.0);
				   }
				   else{
					   cout << 0;
					   newPatt.add((IloNum)0.0);
				   }
			   }
			   cout << " ";
			   for (int j=1; j<=num_job; j++){
				   cout << subsets_matrix[s][j];
				   newPatt.add((IloNum)subsets_matrix[s][j]);
			   }
			   double subset_cost = cost_of_subset[s];
			   var.add(IloNumVar(obj(subset_cost) + con(newPatt)));
			   const char * cc;

			   ostringstream ostr;
			   ostr << i;
			   ostr << s;
			   string s = "x"+ ostr.str();				// name the variable xiT,
			   cc = s.c_str();						    // where i=driver, T=subset
			   var[var.getSize()-1].setName(cc);
			   newPatt.clear();
			   cout << endl;
		   }
	   }

   }

   model.add(obj);
   model.add(con);

   IloCplex cplex(model);
   cplex.exportModel("lpex1.lp");

   try {
	   // obtain optimal solution.
	   if ( !cplex.solve() ) {
		   env.error() << "Failed to optimize LP" << endl;
		   throw(-1);
	   }

	   IloNumArray vals(env);
	   env.out() << "Solution status = " << cplex.getStatus() << endl;
	   env.out() << "Solution value  = " << cplex.getObjValue() << endl;
	   cplex.getValues(vals, var);
	   env.out() << "Values        = " << vals << endl;
	   cplex.getSlacks(vals, con);
	   env.out() << "Slacks        = " << vals << endl;
	   cplex.getDuals(vals, con);
	   env.out() << "Duals         = " << vals << endl;
	   cplex.getReducedCosts(vals, var);
	   env.out() << "Reduced Costs = " << vals << endl;
   }
   catch (IloException& e) {
	   cerr << "Concert exception caught: " << e << endl;
   }
   catch (...) {
	   cerr << "Unknown exception caught" << endl;
   }

   // ---------- Change Formulation -------------------- //  --- UNTESTED!!!!

   IloNumArray vals(env);
   unsigned int column_num = service_subsets.size();

   for ( ; ; ){
	   cplex.getDuals(vals, con);
	   // findSubsets(vals);												// find new subsets (columns) and adds it to service_subsets
	   //findSubsetsExhaust(vals);   // TESTING HERE ...
	   //AddNewSubsetExhaustive(vals);
	   AddNewSubsetGreedy(vals);
	   if (column_num == service_subsets.size())						// if after running findSubsets, the # of columns don't change
		   break;														// then break loop, we're at optimal

	   newPatt.clear();

	   for (unsigned int s=column_num; s<service_subsets.size(); s++){      // for each subset S in the service_subsets
		   for (int i=0; i<num_worker; i++){ // for each worker i
			   bool b = true;
			   for(unsigned int j = 0; j < service_subsets[s].size(); j++){
				   if(!serviceable[i][service_subsets[s][j]])
				   {
					   b = false;
					   break;
				   }
			   }
			   if(b){			// check if i can service subset S
				   for (int w=0; w<num_worker; w++){
					   if (i==w){
						   cout << 1;
						   newPatt.add((IloNum)-1.0);
					   }
					   else{
						   cout << 0;
						   newPatt.add((IloNum)0.0);
					   }
				   }
				   cout << " ";
				   for (int j=1; j<=num_job; j++){
					   cout << subsets_matrix[s][j];
					   newPatt.add((IloNum)subsets_matrix[s][j]);
				   }
				   double subset_cost = cost_of_subset[s];
					
				   var.add(IloNumVar(obj(subset_cost) + con(newPatt)));
				   const char * cc;

				   ostringstream ostr;
				   ostr << i;
				   ostr << s;
				   string s = "x"+ ostr.str();				// name the variable xiT,
				   cc = s.c_str();						    // where i=driver, T=subset
				   var[var.getSize()-1].setName(cc);
				   newPatt.clear();
				   cout << endl;
			   }
		   }
		   cout << endl;
	   }

	   try {
		   // obtain optimal solution.
		   if ( !cplex.solve() ) {
			   env.error() << "Failed to optimize LP" << endl;
			   throw(-1);
		   }
		   IloNumArray vals(env);
		   env.out() << "Solution status = " << cplex.getStatus() << endl;		// prints out solution
		   env.out() << "Solution value  = " << cplex.getObjValue() << endl;
		   cplex.getValues(vals, var);
		   env.out() << "Values        = " << vals << endl;
		   cplex.getSlacks(vals, con);
		   env.out() << "Slacks        = " << vals << endl;
		   cplex.getDuals(vals, con);
		   env.out() << "Duals         = " << vals << endl;
		   cplex.getReducedCosts(vals, var);
		   env.out() << "Reduced Costs = " << vals << endl;
	   }
	   catch (IloException& e) {
		   cerr << "Concert exception caught: " << e << endl;
	   }
	   catch (...) {
		   cerr << "Unknown exception caught" << endl;
	   }

	   column_num = service_subsets.size();
   }
   cout << "done";
   env.end();

   return 0;
}  // END main

double MST(vector<int> subset) // could be done more efficiently using heap
{
	double c = 0;

	// used - depo, free - rest
	list<int> used, free;
	used.push_back(0);
	for(int i = 0; i < subset.size(); i++)
		free.push_back(subset[i]);

	for(int i = 0; i < subset.size(); i++)
	{
		// find cheapest edge from used to free
		list<int>::iterator bestj = used.begin(), bestk = free.begin();
		for (list<int>::iterator j = used.begin(); j != used.end(); j++)
			for (list<int>::iterator k = free.begin(); k != free.end(); k++)
				if(cost[*j][*k] < cost[*bestj][*bestk])
				{
					bestj = j;
					bestk = k;
				}

		// use edge, move vertex from free to used
		c += cost[*bestj][*bestk];
		used.push_back(*bestk);
		free.erase(bestk);
	}

	return c;
}

bool AddNewSubsetExhaustive(IloNumArray duals)
{
	vector<char> mask(num_job, 0);
	while(true)
	{
		// get next mask configuration
		int i = 0;
		mask[i]++;
		while(mask[i] > 1)
		{
			if(i == num_job - 1)
				break;
			mask[i] = 0;
			mask[++i]++;
		}
		if(mask[i] > 1)
			break;

		// get subset from mask
		vector<int> subset;
		subset.reserve(num_job);
		for(int j = 0; j < num_job; j++)
			if(mask[j])
				subset.push_back(j + 1);

		double b, c;
		bool calculated = false;
		for(int w = 0; w < num_worker; w++)
		{
			// check serviceability
			bool isServiceable = true;
			for(int j = 0; j < subset.size(); j++)
				if(!(serviceable[w][subset[j]]))
				{
					isServiceable = false;
					break;
				}

			if(isServiceable)
			{
				if(!calculated)
				{
					// calculate sum of betas
					b = 0;
					for(int j = 0; j < subset.size(); j++)
						b += duals[num_worker + subset[j] - 1];

					// calculate cost of tree
					c = MST(subset);

					calculated = true;
				}

				// add if satisfies
				if(duals[w] - b + c < -0.000000001)
				{
					vector<int> line(num_job + 1);
					subsets_matrix.push_back(line);
					subsets_matrix[service_subsets.size()][0] = 1;
					for(int j = 0; j < num_job; j++)
						subsets_matrix[service_subsets.size()][j + 1] = mask[j];
					service_subsets.push_back(subset);
					cost_of_subset.push_back(c);

					return true;
				}
			}
		}
	}

	return false;
}

bool AddNewSubsetGreedy(IloNumArray duals)
{
	for(int w = 0; w < num_worker; w++)
	{
		vector<int> subset;
		subset.reserve(num_job);
		vector<char> mask(num_job, 0);
		IloNum b = (IloNum) 0;
		for(int i = 0; i < num_job; i++)
		{
			int bestj = -1;
			for(int j = 0; j < num_job; j++)
				if(!mask[j] && serviceable[w][j + 1] && (bestj == -1 || duals[num_worker + j] > duals[num_worker + bestj]))
					bestj = j;

			if(bestj == -1)
				break;

			subset.push_back(bestj + 1);
			mask[bestj] = 1;

			b += duals[num_worker + bestj];

			IloNum c = (IloNum)MST(subset);

			if(duals[w] - b + c < -0.000001)
			{
				vector<int> line(num_job + 1);
				subsets_matrix.push_back(line);
				subsets_matrix[service_subsets.size()][0] = 1;
				for(int j = 0; j < num_job; j++)
					subsets_matrix[service_subsets.size()][j + 1] = mask[j];
				service_subsets.push_back(subset);
				cost_of_subset.push_back(c);
				
				printf("%f \n", duals[w] - b + c);
				return true;
			}
		}
	}

	return false;
}

static void printSubsets(){
	   // print matrix
	   cout << endl << "Subset Matrix" << endl;;
	   for (unsigned int i=0; i<service_subsets.size(); i++){
		   for (int j=1; j<=num_job; j++){
			   cout << subsets_matrix[i][j];
		   }
		   cout << endl;
	   }
	   cout << endl;

	   // print subsets
	   cout << "Subsets so far" << endl;
	   for (unsigned int s=0; s<service_subsets.size(); s++){
		   for (unsigned int j=0; j<service_subsets[s].size(); j++){
			   cout << service_subsets[s][j];
		   }
		   cout << endl;
	   }
	   cout << endl;

}
