#include "heuristic.h"

solution Heuristic::SolveIP(){
	solution sol(E.K(),E.Voters(),E.Candidates());
	try {
		IloEnv env;
		IloModel model(env);
		IloCplex cplex(model);
		IloBoolVarArray y(env,E.Candidates());
		BoolVarMatrix x(env,E.Candidates());
		for(int i =0; i < E.Candidates(); i++)
			x[i] = IloBoolVarArray(env,E.Voters());
		//create obj function
		IloExpr obj(env);
		for(int i = 0; i < E.Candidates(); i++)
			for(int j = 0; j < E.Voters(); j++)
				obj += (E.Ranking()[i][j]-1) * x[i][j] ;
		model.add(IloMinimize(env, obj));
		//add constraints
		IloExpr constraint(env);
		for (int i = 0; i < E.Candidates(); i++)
			constraint += y[i];
		model.add(constraint == E.K());
		for (int j = 0; j < E.Voters(); j++){
			IloExpr constraint(env);
			for(int i = 0; i < E.Candidates(); i++)
				constraint+=x[i][j];
			model.add(constraint == 1);
		}
		for(int i = 0; i < E.Candidates(); i++)
			for (int j = 0; j < E.Voters(); j++)
				model.add(x[i][j]-y[i] <= 0);
		time_t start, end;
		time(&start);
		if ( !cplex.solve() ) {
			env.error() << "Failed to optimize LP." << endl;
			throw(-1);
		}
		time(&end);
		sol.obj_value = cplex.getObjValue();
		IloNumArray vals(env);
		cplex.getValues(vals,y);
		int counter_candid =0;
		int counter_left_out  = 0;
		for(int i = 0; i < E.Candidates();i++)
			if(vals[i] == 1)
				sol.candidates[counter_candid++] = i;
			else if(vals[i] == 0)
				sol.left_out_candidates[counter_left_out++] = i	;
		vals.clear();
		for (int i = 0; i < E.Candidates(); i++){
			cplex.getValues(vals,x[i]);
			for(int  j = 0; j < E.Voters(); j++)
				if (vals[j] == 1)
					sol.assignment[j] = i;
		}
		cout << "The running time is : " << difftime(end, start) << endl;
		cout << "The obj value is : " << sol.obj_value << endl;
		cout << "The optimal candidate subset is = " << sol.candidates ;
		cout << "The optimal assignment is = " << sol.assignment << endl;
		env.end();
	}
	catch (IloException& e){
		cerr << "Concert exception caught: " << e << endl;
	}
	catch (...) {
		cerr << "Unknown exception caught" << endl;
	}
}

solution Heuristic::SolveLP(){
}


solution Heuristic::LocalSearch(){
	solution sol(E.K(), E.Voters(),E.Candidates());
	sol = GenerateFeasibleSolution(sol);
	ComputeOptimalAssignment(sol);
	SolutionValue(sol);
	vector<int> best_solution(sol.candidates.begin(), sol.candidates.end());
	cout << "best solution " << best_solution << endl;
	double best_sol_value = sol.obj_value;
	bool local_optima = false;
	while(!local_optima){
		cout << "current sol is = " << sol.candidates << endl;
		cout << "assignment is = " << sol.assignment << endl;
		cout << "current obj value is : " << sol.obj_value << endl;
		int i = 0;
		bool better_candid = false;
		while (i <  E.K() && !better_candid){
			int j = 0; 
			while (j < E.Candidates() - E.K()){
				int temp = sol.candidates[i];
				sol.candidates[i] = sol.left_out_candidates[j];
				ComputeOptimalAssignment(sol);
				SolutionValue(sol);
				if( sol.obj_value < best_sol_value){
					sol.left_out_candidates[j] = temp;
					best_solution = sol.candidates;
					best_sol_value = sol.obj_value;
					better_candid = true;
					break;
				}
				else 
					sol.candidates[i] = temp;
				++j;
			}
			++i;
		}
		if(!better_candid)
			local_optima = true;		
	}
	cout << " Heuristic obj value is = " << best_sol_value << endl;
	cout << " Heuristic subset is " << best_solution << endl;
	return sol;
}
solution& Heuristic::GenerateFeasibleSolution(solution& sol){
	srand(time(NULL));
	vector<bool> candid_selected(E.Candidates());
	int candid_indx = 0;
	for (int i = 0; i < E.Candidates(); i++)
		candid_selected[i] = false;
	int k_indx = 0;
	while(k_indx < E.K()){
		candid_indx = rand() % E.Candidates();
		if (!candid_selected[candid_indx]){
			candid_selected[candid_indx] = true;
			sol.candidates[k_indx++] = candid_indx;
		}
	}
	//keep  non-selected candidates into a proper vector
	int counter = 0;
	for(int i = 0; i < E.Candidates(); i++)
		if(!candid_selected[i])
			sol.left_out_candidates[counter++] =  i;
	//sol.candidates[0] = 0;
	//sol.candidates[1] = 1;
	//sol.candidates[2] = 4;
	//sol.candidates[3] = 8;
	//sol.candidates[4] = 7;
	return sol;
}
int Heuristic::IndexCandidate(int candidate, int voter){
	int index = -1;
	int i = 0;
	while(i < E.Candidates())
		if(E.Ranking()[i][voter] == candidate){
			index = i;
			break;
		}
		else 
			++i;
	assert(index > E.Candidates || index < 0 );
	return index;
}

void Heuristic::ComputeOptimalAssignment(solution& sol){
	int size = sol.assignment.size();
	for(int i=0; i < size;i++)
		sol.assignment[i] = BestCandidate(sol,i);
}
void Heuristic::SolutionValue(solution& sol){
	int size = sol.assignment.size();
	double sum = 0;
	for(int i = 0 ; i < size; i++)
		sum += E.Ranking()[sol.assignment[i]][i]-1;
	sol.obj_value = sum;
}
int Heuristic::BestCandidate(solution& sol,int voter ){
	int minVal = E.Candidates() + 1;
	int size = sol.candidates.size();
	int min_indx= -1;       
	for (int i = 0; i < size; i++){
	//	int index = IndexCandidate(sol.candidates[i], voter);
		if (E.Ranking()[sol.candidates[i]][voter] < minVal){
			minVal = E.Ranking()[sol.candidates[i]][voter];
			min_indx = sol.candidates[i];
		}
	}
	assert(min_indx > size || min_indx < 0);
	return min_indx;
}

