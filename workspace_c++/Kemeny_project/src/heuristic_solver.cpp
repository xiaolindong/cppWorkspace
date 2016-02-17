/*
 * heuristic_solver.cpp
 *  Created on: Jan 14, 2014
 *  Author: eia2
 */
#include "heuristic_solver.h"
#define RC_EPS 1.0e-10





struct trpl{
	int i,j,k;
	double value;
	trpl(int w,int s,int t, double v):i(w),j(s),k(t),value(v){}
	bool operator <(trpl a) const{
		if (value < a.value)
			return true;
		else return false;	
	}
};

int HeurSolver::ExistViolateDicycle(IloModel model,IloCplex cplex,NumVarMatrix x){
	std::priority_queue<trpl> pq;
	int nCut=500000, counter = 0;
	vector<vector<double> > x_copy(R.Candidates());
	for(int i =0 ; i < R.Candidates();i++)
		x_copy[i].resize(R.Candidates());
	for(int i = 0; i < R.Candidates(); i++)
		for(int j =0; j < R.Candidates(); j++)
			x_copy[i][j] = cplex.getValue(x[i][j]);
	for (int i = 0; i < R.Candidates(); i++)
		for (int j = 0; j < R.Candidates(); j++)
			for (int k = 0; k < R.Candidates(); k++)
				if ((i != j) && (j != k) && ( k != i))
					if (x_copy[i][j]+ x_copy[j][k]+ x_copy[k][i] -2 > RC_EPS  ){
						double angle = (R.Get_m_Array()[i][j] + R.Get_m_Array()[j][k]+ R.Get_m_Array()[k][i])/sqrt(3);
						trpl t(i,j,k,angle);
						pq.push(t);
					}
	while (counter++ < nCut){
		if (!pq.empty()){
			trpl t = pq.top();
			pq.pop();
			model.add(x[t.i][t.j] + x[t.j][t.k] + x[t.k][t.i] <= 2);
		}
		else 
			break;
	}
	trpl t = pq.top();
	cout << "top is " << t.value << endl;
	cout << "pq szie is " << pq.size() << endl;
	cout <<"counter isssss :" << counter << endl;
	return counter;
}


/*
   int HeurSolver::ExistViolateDicycle(IloModel model,IloCplex cplex,NumVarMatrix x){
   int nCut=1000000, counter = 0;
   vector<vector<double> > x_copy(R.Candidates());
   for(int i =0 ; i < R.Candidates();i++)
   x_copy[i].resize(R.Candidates());
   for(int i = 0; i < R.Candidates(); i++)
   for(int j =0; j < R.Candidates(); j++)
   x_copy[i][j] = cplex.getValue(x[i][j]);
   for (int i = 0; i < R.Candidates(); i++)
   for (int j = 0; j < R.Candidates(); j++)
   for (int k = 0; k < R.Candidates(); k++)
//if ((i != j) && (j != k) && ( k != i))
if (x_copy[i][j]+ x_copy[j][k]+ x_copy[k][i] -2 > RC_EPS  ){
double angle = (R.Get_m_Array()[i][j] + R.Get_m_Array()[j][k]+ R.Get_m_Array()[k][i])/sqrt(3);
if (angle > 100){
double violated = x_copy[i][j]+ x_copy[j][k]+ x_copy[k][i];
model.add(x[i][j] + x[j][k] + x[k][i] <= 2);
//	cout << "The angel is : " << angle << endl;
//  check to see if ncut upper bound is reached
if (++counter > nCut) {
return counter;
}
}
}
cout <<"counter isssss :" << counter << endl;
return counter;
}

*/
solution HeurSolver::SolveLPIterat(){
	int size = R.Candidates();
	solution s(size);
	try{
		IloEnv env;
		IloModel model(env);
		IloCplex cplex(model);
		NumVarMatrix x(env, size);
		for(int i = 0; i < size; i++)
			x[i] = IloNumVarArray(env, size, 0,1,ILOFLOAT);
		//generate the obj_function
		IloExpr obj_fun(env);
		for(int i = 0; i < size; i++)
			for(int j = 0; j < size; j++)
				//if (i != j)
				obj_fun += (R.Get_m_Array()[i][j] * x[i][j]);
		model.add(IloMaximize(env, obj_fun));
		//add simple constraints 
		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				if (i != j)
					model.add(x[i][j] + x[j][i] == 1);
				else
					model.add(x[i][i] == 0);	
		cplex.setParam(IloCplex::RootAlg, IloCplex::Barrier);
		//cplex.setOut(env.getNullStream());
		//track the running time
		//cplex.setParam(IloCplex::TiLim, 10);
		time_t start, end;
		time(&start);
		if ( !cplex.solve() ) {
			env.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}
		// run separation and add constraints iteratively
		while (ExistViolateDicycle(model,cplex, x) != 0)
			if ( !cplex.solve() ) {
				env.error() << "Failed to optimize LP." << endl;
				throw (-1);
			}
		if ( !cplex.solve() ){
			env.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}
		//compute the running time
		//cout << "s is " << s << " and e is : " << e << endl;
		time(&end);
		s.time = difftime(end,start);
		s.obj_value = cplex.getObjValue();
		cout << "The LP Iterat OBJ is = " << s.obj_value << " , time is = " << s.time << endl;
		for(int i =0; i < size; i++)
			for(int j =0; j < size; j++)
				s.sol_vector[i][j] = cplex.getValue(x[i][j]);
		cout << "Time is = " << s.time << endl;
		cout << "Regular LP OBJ Value is  = " << s.obj_value << endl;
		cout << "The LP solution is : " << s.sol_vector << endl;	


		env.end();
	}
	catch ( IloException & e){
		cerr << "Concert exception caught: " << e << endl;
	} catch (...){
		cerr << "Unknown exception caught" << endl;
	}
}



solution HeurSolver::SolveLP(vector<vector<double> >& matrix){
	int size = matrix.size();
	solution s(size);
	try{
		IloEnv env;
		IloModel model(env);
		IloCplex cplex(model);
		NumVarMatrix x(env, size);
		for(int i = 0; i < size; i++)
			x[i] = IloNumVarArray(env, size, 0,1,ILOFLOAT);
		//generate the obj_function
		IloExpr obj_fun(env);
		for(int i = 0; i < size; i++)
			for(int j = 0; j < size; j++)
				if (i != j)
					obj_fun += (matrix[i][j] * x[i][j]);
		model.add(IloMaximize(env, obj_fun));
		//add simple constraints 
		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				if (i != j)
					model.add(x[i][j] + x[j][i] == 1);
				else
					model.add(x[i][i] == 0);
		// add 3-dicycles constraints
		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				for (int k = 0; k < size; k++)
					if ((i != j) && (j != k) && ( k != i))
						model.add(x[i][j] + x[j][k] + x[k][i] <= 2);
		//time setting
		time_t start, end;
		time(&start);
		if ( !cplex.solve() ) {
			env.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}
		time(&end);
		//fill the solution
		s.time = difftime(end,start);
		s.obj_value = cplex.getObjValue();	
		for(int i =0; i < size; i++)
			for(int j =0; j < size; j++)
				s.sol_vector[i][j] = cplex.getValue(x[i][j]);
		env.end();
	}
	catch ( IloException & e){
		cerr << "Concert exception caught: " << e << endl;
	} catch (...){
		cerr << "Unknown exception caught" << endl;
	}
	return s;
}
solution HeurSolver::SolveILP(vector<vector<double> >& matrix){
	int size = matrix.size();
	solution s(size);
	try{
		IloEnv env;
		IloModel model(env);
		IloCplex cplex(model);
		BoolVarMatrix x(env, size);
		for(int i = 0; i < size; i++)
			x[i] = IloBoolVarArray(env, size);
		//generate the obj_function
		IloExpr obj_fun(env);
		for(int i = 0; i < size; i++)
			for(int j = 0; j < size; j++)
				if (i != j)
					obj_fun += (matrix[i][j] * x[i][j]);
		model.add(IloMaximize(env, obj_fun));
		//add simple constraints 
		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				if (i != j)
					model.add(x[i][j] + x[j][i] == 1);
				else
					model.add(x[i][i] == 0);
		// add 3-dicycles constraints
		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				for (int k = 0; k < size; k++)
					if ((i != j) && (j != k) && ( k != i))
						model.add(x[i][j] + x[j][k] + x[k][i] <= 2);
		//time setting
		time_t start, end;
		time(&start);
		if ( !cplex.solve() ) {
			env.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}
		time(&end);
		//fill the solution
		s.time = difftime(end,start);
		s.obj_value = cplex.getObjValue();	
		for(int i =0; i < size; i++)
			for(int j =0; j < size; j++)
				s.sol_vector[i][j] = cplex.getValue(x[i][j]);
		env.end();
	}
	catch ( IloException & e){
		cerr << "Concert exception caught: " << e << endl;
	} catch (...){
		cerr << "Unknown exception caught" << endl;
	}
	return s;
}


void HeurSolver::InitialConstraintManager(vector<vector<dicycle> >& ConstManager, EdgeManager& Edge, CycleManager& Cycle, int size_edge){
	for(int i=0; i < size_edge; i++ ){
		pair<int,int> e = Edge.GetEdgeByLabel(i);
		int temp[3];
		temp[0]= e.first;
		temp[1] = e.second;
		for (int k = 0; k < R.Candidates(); k++)
			if(k != temp[0] && k!=temp[1]){
				temp[2]	= k;
				int j =0;
				while (j < 3){
					if((temp[j%3] < temp[(j+1)%3] && temp[(j+1)%3] < temp[(j+2)%3]) ||
							(temp[j%3] > temp[(j+1)%3] && temp[(j+1)%3] > temp[(j+2)%3])){
						dicycle d(temp[j%3],temp[(j+1)%3],temp[(j+2)%3]);
						ConstManager[i].push_back(d);
						break;
					} else j++;
				}
			}
	}
}

void HeurSolver:: PrintConstManager(vector<vector<dicycle> >& ConstManager,EdgeManager& Edge){
	vector<vector< dicycle> >::const_iterator it = ConstManager.begin();
	int i =0;
	for( it = ConstManager.begin(); it!=ConstManager.end(); it++){
		pair<int,int> e = Edge.GetEdgeByLabel(i++);
		cout << "y_( " << e.first << " , " << e.second << " )  " ;
		for(vector<dicycle>::const_iterator it_j = it->begin(); it_j != it->end(); it_j++ ){
			cout << "+ z_( " << it_j->i << " , " << it_j->j << " , " << it_j->k << " )  " ;
		}
		cout << " <= 0 " << endl;
	}
}

double HeurSolver::ComputeTeta(vector<vector<dicycle> >& ConstManager, dual_sol_vectors& dual_solution, 
		dual_sol_vectors& drp_solution, EdgeManager& Edge, CycleManager& Cycle){
	double teta = 100000000;;
	// y_ij + sum_{k}z_ijk <= c_ji constraints
	cout << "#================ Teta Computation ===========#" << endl;
	int cm_size = ConstManager.size();
	//cout << setfill(' ') << setw(5);
	cout.precision(3);
	cout << "----------------------------------------------------------------------" << endl;
	cout << setw(7) << "Pair " << setw(8) << " LHS " << setw(14) << "dual_sol"<< setw(10) << "drp_sol" 
		<< setw(11) <<  "Teta_coef" << setw(8) << "w_ij" << setw(11) << "teta" << endl;	
	cout << "----------------------------------------------------------------------" << endl;
	for(int i =0; i < cm_size; i++){
		pair<int,int> e = Edge.GetEdgeByLabel(i);
		cout << setw(3)<< "(" << e.first << "," << e.second <<")" << setw(8);
		double LHS = dual_solution.y[i];
		double teta_coef = drp_solution.y[i];
		vector<dicycle>::const_iterator it;
		for(it = ConstManager[i].begin(); it != ConstManager[i].end(); it++){
			int label_cycle = Cycle.GetLabel(it->i, it->j, it->k);
			LHS += dual_solution.z[label_cycle];
			teta_coef += drp_solution.z[label_cycle];
		}
		cout <<  LHS << setw(10) << dual_solution.y[i] << setw(10) << drp_solution.y[i] << setw(11) 
			<<  teta_coef << setw(10) << R.Get_m_Array()[e.second][e.first] 
			<< setw(13) << static_cast<double>((R.Get_m_Array()[e.second][e.first]-LHS)/teta_coef) << endl; 
		if (teta_coef > 0)
			if ((R.Get_m_Array()[e.second][e.first]-LHS)/teta_coef < teta ) // need to be done carfeully
				// if the teta_coef is minus then inseatd of upper bund on theta we have lower bound ???
				teta = static_cast<double>((R.Get_m_Array()[e.second][e.first]-LHS)/teta_coef); 
	}
	cout << "----------------------------------------------------------------------" << endl;
	cout << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "dicycle" << setw(13) << "dual_sol" << setw(10) << "drp_sol" << setw(10) << "teta" << endl;	
	cout << "-------------------------------------------------" << endl;
	//z_ijk >= 0 constraints
	for(int i =0; i < dual_solution.z.size(); i++){
		cycle c = Cycle.GetCycleByLabel(i) ;
		cout << "(" << c.first.first << "," << c.first.second << "," << c.second << ")" << setw(11);
		cout <<  dual_solution.z[i] << setw(10) << drp_solution.z[i] << setw(13)
			<< static_cast<double>(dual_solution.z[i]/drp_solution.z[i])  << endl; 
		if (drp_solution.z[i] < 0)
			if (dual_solution.z[i]/abs(drp_solution.z[i]) < teta)
				teta =static_cast<double>(dual_solution.z[i]/abs(drp_solution.z[i]));
	}
	cout << "-------------------------------------------------" << endl;
	cout << "teta is = " << teta << endl << endl; 
	return teta;
}

bool HeurSolver::DualFeasibile(vector<vector<dicycle> >& ConstManager, dual_sol_vectors& dual_solution, 
		EdgeManager& Edge, CycleManager& Cycle, vector<pair<int,int> >& tight_edges, vector<dicycle>& tight_cycles)
{
	int cm_size = ConstManager.size();
	for(int i =0; i < cm_size; i++){
		pair<int,int> e = Edge.GetEdgeByLabel(i);
		double LHS = dual_solution.y[i];
		vector<dicycle>::const_iterator it;
		for(it = ConstManager[i].begin(); it != ConstManager[i].end(); it++){
			int label_cycle = Cycle.GetLabel(it->i, it->j, it->k);
			LHS += dual_solution.z[label_cycle];
		}
		if (LHS -  R.Get_m_Array()[e.second][e.first] > RC_EPS ){ 
			cout << "The pair related to infeasibility is = (" << e.first << "," << e.second << ")" << endl; 
			cout << "LHS = " << LHS << " and c_ji is = " << R.Get_m_Array()[e.second][e.first]<< endl;
			return false;
		}
		else if(LHS == R.Get_m_Array()[e.second][e.first]) 
			tight_edges.push_back(e);
	}
	for(int i =0; i < dual_solution.z.size(); i++){
		cycle c = Cycle.GetCycleByLabel(i);
		dicycle d(c.first.first,c.first.second,c.second);
		if (dual_solution.z[i] < 0){ 
			cout << "dual z_i is " << dual_solution.z[i] << endl;
			return false;
		}
		else if (dual_solution.z[i] == 0) 
			tight_cycles.push_back(d);
	}
	return true;
}





void HeurSolver::PrintVector(vector<double>& s)
{
	int counter = 0;
	vector<double>::const_iterator i;
	for(i = s.begin(); i != s.end(); i = i+2){
		cout << *i << "  ";  // increase by 1 in order to match the convention indexing scheme
	}
}



double HeurSolver::SolutionValue(dual_sol_vectors& v){
	double obj_val_drp = 0;
	for(int i =0; i < v.y.size();i = i+2)
		obj_val_drp += v.y[i];
	for(int i= 0; i < v.z.size(); i++)
		obj_val_drp += v.z[i];
	return obj_val_drp;
}

solution HeurSolver::SolvePrimalDual(){
   solution s(R.Candidates());
	 EdgeManager Edge;
    CycleManager Cycle;
    Edge.Initial(R.Candidates());
    Edge.Print();
    Cycle.Initial(R.Candidates());
    Cycle.Print();
    vector<vector<dicycle> > ConstManager;
    int size_edge = R.Candidates()*(R.Candidates()-1);
    ConstManager.resize(size_edge);
    InitialConstraintManager(ConstManager, Edge, Cycle,size_edge);
    //PrintConstManager(ConstManager, Edge);
	dual_sol_vectors dual_solution(R.Candidates());
	dual_sol_vectors drp_solution(R.Candidates());
	vector<pair<int,int> > tight_edges;
	vector<dicycle> tight_cycles;
	try{
		if (!DualFeasibile(ConstManager, dual_solution,Edge,Cycle, tight_edges, tight_cycles)){
			cerr << "dual infesiblity in presolve" << endl;
			throw(-1);
		}
		int iteration = 0;
		double teta =0;
		while (true){	
			cout << "#####################################################"<< endl;
			cout << "iteration = " << iteration++ << endl;
			cout << "The dual soluiton value is = " << SolutionValue(dual_solution) << endl;
			SolveDRP(tight_edges,tight_cycles,Edge,Cycle,drp_solution);
		/*	
			cout << "#-----------------------------------------------------#"<< endl;
			cout << "  Variables values begin of iteration " << iteration << endl;
			cout << "dual solution vector  y = { " << dual_solution.y << " } "<<  endl;
		    cout << "dual solution vector z = { "	<< dual_solution.z << "} " <<endl;
			cout << "drp solution vector  y = { " << drp_solution.y << " }" << endl;
		    cout << "drp solution vector z = { " << drp_solution.z << " }" << endl;
			cout << "tight edges are : " << tight_edges << endl;
			cout << "tight cycles are : " << tight_cycles << endl;
			cout << "Teta = " << teta << endl;
			cout << "#--------------------------------------------------------#" << endl;
		*/	

			//check if the drp_solution value is 0
			if (SolutionValue(drp_solution)== 0){
				cout << "optimal dual is found, drp solution value is 0 " << endl;
				break;
			}
			teta = ComputeTeta(ConstManager,dual_solution,drp_solution, Edge, Cycle);			
			//cout << "teta is :" << teta << endl;
			if (teta <=0){
				cerr << "unacceptable teta = " << teta <<  endl;
				throw(-2);
			}
			//update the dual solution 
			for(int i =0; i < dual_solution.y.size(); i++)
				dual_solution.y[i] += teta*drp_solution.y[i];
			for(int i =0; i < dual_solution.z.size(); i++)
				dual_solution.z[i] += teta*drp_solution.z[i];
			cout << "dual solution "  << endl << " y = { " ;
			PrintVector(dual_solution.y); 
			cout << " } " << endl;;
		    cout << " z = { "	<< dual_solution.z << "} " <<endl;
			cout << "drp solution " << endl << " y = { " ;
			 PrintVector(drp_solution.y);
			cout  << " }" << endl;
		    cout << " z = { " << drp_solution.z << " }" << endl;
			cout << "tight edges are : " << tight_edges << endl;
			cout << "tight cycles are : " << tight_cycles << endl;
			cout << "Teta = " << teta << endl;	
			cout << "#####################################################"<< endl;
			tight_edges.clear();
			tight_cycles.clear();
			if (!DualFeasibile(ConstManager, dual_solution,Edge,Cycle, tight_edges, tight_cycles)){
				cerr << "dual infesiblity" << endl;
				throw(-1);
			}
		}
		cout << "The dual optimal obj value is : "  << SolutionValue(dual_solution) << endl; ;
	}catch(int e){
		cout << "AN exception occured. Exception Nr." << e << endl;
	}
	
	//check the dual solution feasibility and compute the tight edges and cycles
	//while not drp zero solution do
	//	solve the drp
	//	check if the drp solutio is 0  
	//	compute the teta 
	//	compuet the dual solution
	//	clear the tight cycle and edge tight vectors
	//	check the dual feasibility
	//	 updated tight cycle and edges
return s;	

}

void HeurSolver::Combinatorial_DRP_Solve(vector<pair<int,int> >& tight_edges, vector<dicycle>& tight_cycles, 
		EdgeManager& Edge, CycleManager& Cycle, dual_sol_vectors& drp_solution){
}



void HeurSolver::SolveDRP(vector<pair<int,int> >& tight_edges, vector<dicycle>& tight_cycles, 
		EdgeManager& Edge, CycleManager& Cycle, dual_sol_vectors& drp_solution){
	solution s(R.Candidates());
	static int iter= 0;
	try{
		IloEnv env;
		IloModel model(env);
		IloCplex cplex(model);
		NumVarMatrix y(env, R.Candidates());
		for(int i = 0; i < R.Candidates(); i++)
			y[i] = IloNumVarArray(env, R.Candidates(),-IloInfinity,1.0,ILOFLOAT);
		for(int i =0; i < R.Candidates(); i++)
			for(int j =0; j < R.Candidates(); j++){
				stringstream s;
				s << i << "," << j;
				string name("y_");
				name += s.str();
				const char* n_y = name.c_str();	
				y[i][j].setName(n_y);
				//delete n_y;
			}
		ThreeDMatrix z(env,R.Candidates());
		for (int i = 0; i < R.Candidates(); i++){
			z[i] = NumVarMatrix(env,R.Candidates());
			for(int j =0; j < R.Candidates(); j++)
				z[i][j]= IloNumVarArray(env,R.Candidates(),-IloInfinity,1.0,ILOFLOAT);

		}
		for(int i =0; i < R.Candidates(); i++)
			for(int j =0; j < R.Candidates(); j++)
				for(int k =0; k < R.Candidates(); k++){
					stringstream s;
					s << i << "," << j << ","<<  k;
					string name("z_");
					name += s.str();
					const char* n_z = name.c_str();	
					z[i][j][k].setName(n_z);
					//delete n_z;
				}
		//generate the obj_function
		IloExpr obj_fun(env);
		for(int i = 0; i < R.Candidates(); i++)
			for(int j = i+1; j < R.Candidates(); j++)
				obj_fun +=  y[i][j];
		for(int i =0; i < R.Candidates(); i++)
			for(int j = 0; j < R.Candidates(); j++)
				for(int k = 0; k < R.Candidates(); k++)
					if (i!=j && j!=k && k!=i)
						if (!( (j > i && j >k) || (j < i && j < k)) )
							obj_fun += z[i][j][k];
		model.add(IloMaximize(env, obj_fun));
		for(int i =0; i < R.Candidates(); i++)
			for(int j = i+1; j < R.Candidates(); j++)
				model.add(y[i][j] - y[j][i] == 0);
		for(int i = 0; i < R.Candidates(); i++)
			for(int j =0; j < R.Candidates(); j++)
				for(int k = 0; k < R.Candidates(); k++)
					if((j  >= i && j >= k) || ( j < i && j < k ))
						model.add(z[i][j][k] == 0);
		for(int i = 0; i < R.Candidates(); i++)
			model.add(y[i][i] == 0);
		//add tight dual constraints in here
		//add tight cycle constraints
		for(vector<dicycle>::iterator it = tight_cycles.begin(); it != tight_cycles.end(); it++ ){
			model.add(z[it->i][it->j][it->k] >= 0);
			if (iter !=0)
				model.add(z[it->i][it->j][it->k] == 0);
		}
		// add tight edges contraint
		for(vector<pair<int,int> >::iterator it = tight_edges.begin(); it != tight_edges.end(); it++){
			IloExpr expr(env);
			expr += y[it->first][it->second];
			int temp[3];
			temp[0]= it->first;
			temp[1] = it->second;
			for (int k = 0; k < R.Candidates(); k++){
				temp[2]	= k;
				int i =0;
				while (i < 3)
					if((temp[i%3] < temp[(i+1)%3] && temp[(i+1)%3] < temp[(i+2)%3]) || 
							(temp[i%3] > temp[(i+1)%3] && temp[(i+1)%3] > temp[(i+2)%3])){
						expr += z[temp[i%3]][temp[(i+1)%3]][temp[(i+2)%3]];
						break;
					}else 
						i++;
			}
			model.add(expr <= 0);
		}
		stringstream st;
		st << iter++;
		string name("./drp_lp/drp_");
		name += st.str();
		name += ".lp";
		const char* n = name.c_str();	
		cplex.exportModel(n);
		//delete n;
		//n = NULL;
		//delete n;
		time_t start, end;
		time(&start);
		cplex.setOut(env.getNullStream());
		if ( !cplex.solve() ) {
			env.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}
		time(&end);
		//create the solution
		s.time = difftime(end,start);
		s.obj_value = cplex.getObjValue();	
		// update te drp_solution struct
		for(map<cycle,int>::iterator it = Cycle.GetIterator(); it !=Cycle.EndIterator(); it++ ){
			drp_solution.z[it->second] = cplex.getValue(z[it->first.first.first][it->first.first.second][it->first.second]);
			//env.out() <<  cplex.getValue(z[it->first.first.first][it->first.first.second][it->first.second]) << endl; 
			//cout << "( " << it->first.first.first << " , " << it->first.first.second << ", " << it->first.second << " ) -> ( " << it->second << " )" <<  endl;	
		}
		for(map<pair<int,int>,int>::iterator it = Edge.GetIterator(); it !=Edge.EndIterator(); it++){
			drp_solution.y[it->second] = cplex.getValue(y[it->first.first][it->first.second]);
			//env.out() <<   cplex.getValue(y[it->first.first][it->first.second]) << endl;
			//cout << "( " << it->first.first << " , " << it->first.second << " ) -> ( " << it->second << " )" <<  endl;		
		}
		cout << "DRP LP OBJ Value is  = " << s.obj_value << endl;
		//cout << "The LP solution is : " << s.sol_vector << endl;	
		env.end();
	}
	catch ( IloException & e){
		cerr << "Concert exception caught: " << e << endl;
	} catch (...){
		cerr << "Unknown exception caught" << endl;
	}

}


solution HeurSolver::SolveLPDual(){
	solution s(R.Candidates());
	try{
		IloEnv env;
		IloModel model(env);
		IloCplex cplex(model);
		NumVarMatrix y(env, R.Candidates());
		for(int i = 0; i < R.Candidates(); i++)
			y[i] = IloNumVarArray(env, R.Candidates(),-IloInfinity,IloInfinity,ILOFLOAT);
		for(int i =0; i < R.Candidates(); i++)
			for(int j =0; j < R.Candidates(); j++){
				stringstream s;
				s << i+1 << "," << j+1;
				string name("y_");
				name += s.str();
				const char* n = name.c_str();	
				y[i][j].setName(n);
			}
		ThreeDMatrix z(env,R.Candidates());
		for (int i = 0; i < R.Candidates(); i++){
			z[i] = NumVarMatrix(env,R.Candidates());
			for(int j =0; j < R.Candidates(); j++)
				//	z[i][j]= IloNumVarArray(env,R.Candidates(),-IloInfinity,0,ILOFLOAT);
				z[i][j]= IloNumVarArray(env,R.Candidates(),0,IloInfinity,ILOFLOAT);

		}
		for(int i =0; i < R.Candidates(); i++)
			for(int j =0; j < R.Candidates(); j++)
				for(int k =0; k < R.Candidates(); k++){
					stringstream s;
					s << i+1 << "," << j+1 << ","<<  k+1;
					string name("z_");
					name += s.str();
					const char* n = name.c_str();	
					z[i][j][k].setName(n);
				}
		//generate the obj_function
		IloExpr obj_fun(env);
		for(int i = 0; i < R.Candidates(); i++)
			for(int j = i+1; j < R.Candidates(); j++)
				obj_fun +=  y[i][j];
		for(int i =0; i < R.Candidates(); i++)
			for(int j = 0; j < R.Candidates(); j++)
				for(int k = 0; k < R.Candidates(); k++)
					if (i!=j && j!=k && k!=i)
						if (!( (j > i && j >k) || (j < i && j < k)) )
							//	obj_fun += 2*(z[i][j][k]);
							obj_fun += z[i][j][k];
		model.add(IloMaximize(env, obj_fun));
		//add simple constraints 
		//IloRangeArray c(env); 
		for(int i =0; i < R.Candidates(); i++)
			for(int j = i+1; j < R.Candidates(); j++)
				model.add(y[i][j] - y[j][i] == 0);

		/*	
			for (int i = 0; i < R.Candidates(); i++)
			for (int j = 0; j < R.Candidates(); j++)
			if (i != j){
			IloExpr exp(env);
			exp = y[i][j];
			for(int k = 0; k < R.Candidates(); k++)
			if( i !=k && j != k )
			exp += z[i][j][k];
			model.add(exp <= R.Get_m_Array()[j][i] );
			}		
			*/
		for(int i = 0; i < R.Candidates(); i++)
			for(int j =0 ; j < R.Candidates(); j++){
				//	model.add(z[i][i][i] == 0);
				//	model.add(z[i][i][j] == 0);	
				//	model.add(z[i][j][i] == 0);
				//	model.add(z[j][i][i] == 0);
			}
		for(int i = 0; i < R.Candidates(); i++)
			for(int j =0; j < R.Candidates(); j++)
				for(int k = 0; k < R.Candidates(); k++)
					if((j  >= i && j >= k) || ( j < i && j < k ))
						model.add(z[i][j][k] == 0);
		for (int i = 0; i < R.Candidates(); i++)
			for (int j = i +1; j < R.Candidates(); j++){
				IloExpr exp(env);
				IloExpr exp_r(env);
				exp = y[i][j];
				exp_r = y[j][i];
				for(int k = 0; k < R.Candidates(); k++)
					if( i !=k && j != k){
						if (k > j){
							exp += z[i][j][k];
							exp_r += z[k][j][i];
						}else if( i<k && k<j){
							exp += z[j][k][i];
							exp_r += z[i][k][j];
						}else{
							exp += z[k][i][j];
							exp_r += z[j][i][k];
						}
					}
				model.add(exp <= R.Get_m_Array()[j][i] );		
				model.add(exp_r <= R.Get_m_Array()[i][j] );			
			}
		for(int i = 0; i < R.Candidates(); i++)
			model.add(y[i][i] == 0);
		// add z_ijk =0	

		//time setting
		cplex.exportModel("dual.lp");
		time_t start, end;
		time(&start);
		cplex.setOut(env.getNullStream());
		if ( !cplex.solve() ) {
			env.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}
		time(&end);
		//fill the solution
		s.time = difftime(end,start);
		s.obj_value = cplex.getObjValue();	

		/*for(int i =0; i < R.Candidates(); i++)
		  for(int j =0; j < R.Candidates(); j++)
		  s.sol_vector[i][j] = cplex.getValue(x[i][j]);*/
		//cout << "Time is = " << s.time << endl;
		cout << "Regular LP OBJ Value is  = " << s.obj_value << endl;
		//cout << "The LP solution is : " << s.sol_vector << endl;	
		IloNumArray duals(env);
		//cplex.getDuals(duals, c);
		//env.out() << duals;
		env.end();
	}
	catch ( IloException & e){
		cerr << "Concert exception caught: " << e << endl;
	} catch (...){
		cerr << "Unknown exception caught" << endl;
	}
	return s;
}





/////

solution HeurSolver::SolveLP(){
	solution s(R.Candidates());
	try{
		IloEnv env;
		IloModel model(env);
		IloCplex cplex(model);
		NumVarMatrix x(env, R.Candidates());
		for(int i = 0; i < R.Candidates(); i++)
			x[i] = IloNumVarArray(env, R.Candidates(),0,1,ILOFLOAT);
		//generate the obj_function
		IloExpr obj_fun(env);
		for(int i = 0; i < R.Candidates(); i++)
			for(int j = 0; j < R.Candidates(); j++)
				//	if (i != j)
				obj_fun += (R.Get_m_Array()[j][i] * x[i][j]);
		model.add(IloMinimize(env, obj_fun));
		//add simple constraints 
		IloRangeArray c(env); 
		for (int i = 0; i < R.Candidates(); i++)
			for (int j = 0; j < R.Candidates(); j++)
				if (i != j)
					c.add(x[i][j] + x[j][i] == 1);
				else
					c.add(x[i][i] == 0);
		// add 3-dicycles constraints
		/* for (int i = 0; i < R.Candidates(); i++)
		   for (int j = 0; j < R.Candidates(); j++)
		   for (int k = 0; k < R.Candidates(); k++)
		   if ((i != j) && (j != k) && ( k != i))
		   c.add(x[i][j] + x[j][k] + x[k][i] <= 2);
		   */
		//alternative formulation, constraint x_ij + x_jk + x_ki >= 1
		for (int i = 0; i < R.Candidates(); i++)
			for (int j = 0; j < R.Candidates(); j++)
				for (int k = 0; k < R.Candidates(); k++)
					if ((i != j) && (j != k) && ( k != i))
						c.add(x[i][j] + x[j][k] + x[k][i] >=  1);


		model.add(c);
		//Parameter settings
		//if(R.Candidates() > 110)
		//	cplex.setParam(IloCplex::TiLim, 100);
		//cplex.setParam(IloCplex::NodeAlg, IloCplex::Dual);i
		cplex.setParam(IloCplex::RootAlg, IloCplex::Barrier);
		//time setting
		time_t start, end;
		time(&start);
		cplex.setOut(env.getNullStream());
		if ( !cplex.solve() ) {
			env.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}
		time(&end);
		//fill the solution
		s.time = difftime(end,start);
		s.obj_value = cplex.getObjValue();	
		for(int i =0; i < R.Candidates(); i++)
			for(int j =0; j < R.Candidates(); j++)
				s.sol_vector[i][j] = cplex.getValue(x[i][j]);
		//cout << "Time is = " << s.time << endl;
		cout << "Regular LP OBJ Value is  = " << s.obj_value << endl;
		//cout << "The LP solution is : " << s.sol_vector << endl;	
		IloNumArray duals(env);
		cplex.getDuals(duals, c);
		//env.out() << duals;
		env.end();
	}
	catch ( IloException & e){
		cerr << "Concert exception caught: " << e << endl;
	} catch (...){
		cerr << "Unknown exception caught" << endl;
	}
	return s;
}


/*

   solution HeurSolver::SolveLP(){
   solution s(R.Candidates());
   try{
   IloEnv env;
   IloModel model(env);
   IloCplex cplex(model);
   NumVarMatrix x(env, R.Candidates());
   for(int i = 0; i < R.Candidates(); i++)
   x[i] = IloNumVarArray(env, R.Candidates(),0,1,ILOFLOAT);
//generate the obj_function
IloExpr obj_fun(env);
for(int i = 0; i < R.Candidates(); i++)
for(int j = 0; j < R.Candidates(); j++)
//	if (i != j)
obj_fun += (R.Get_m_Array()[j][i] * x[i][j]);
model.add(IloMinimize(env, obj_fun));
//add simple constraints 
for (int i = 0; i < R.Candidates(); i++)
for (int j = 0; j < R.Candidates(); j++)
if (i != j)
model.add(x[i][j] + x[j][i] == 1);
else
model.add(x[i][i] == 0);
// add 3-dicycles constraints
for (int i = 0; i < R.Candidates(); i++)
for (int j = 0; j < R.Candidates(); j++)
for (int k = 0; k < R.Candidates(); k++)
if ((i != j) && (j != k) && ( k != i))
model.add(x[i][j] + x[j][k] + x[k][i] <= 2);
//Parameter settings
//if(R.Candidates() > 110)
//	cplex.setParam(IloCplex::TiLim, 100);
//cplex.setParam(IloCplex::NodeAlg, IloCplex::Dual);i
cplex.setParam(IloCplex::RootAlg, IloCplex::Barrier);
//time setting
time_t start, end;
time(&start);
cplex.setOut(env.getNullStream());
if ( !cplex.solve() ) {
env.error() << "Failed to optimize LP." << endl;
throw (-1);
}
time(&end);
//fill the solution
s.time = difftime(end,start);
s.obj_value = cplex.getObjValue();		
cout << "Regular LP OBJ Value is  = " << s.obj_value << endl;
for(int i =0; i < R.Candidates(); i++)
for(int j =0; j < R.Candidates(); j++)
s.sol_vector[i][j] = cplex.getValue(x[i][j]);
//cout << "Time is = " << s.time << endl;
//cout << "The LP solution is : " << s.sol_vector << endl;	
env.end();
}
catch ( IloException & e){
cerr << "Concert exception caught: " << e << endl;
} catch (...){
cerr << "Unknown exception caught" << endl;
}
return s;
}
*/


solution HeurSolver::SolveILP(){
	solution s(R.Candidates());
	try{
		IloEnv env;
		IloModel model(env);
		IloCplex cplex(model);
		BoolVarMatrix x(env, R.Candidates());
		for(int i = 0; i < R.Candidates(); i++)
			x[i] = IloBoolVarArray(env, R.Candidates());
		//generate the obj_function
		IloExpr obj_fun(env);
		for(int i= 0; i < R.Candidates(); i++)
			for(int j = 0; j < R.Candidates(); j++)
				//	if (i != j)
				obj_fun += (R.Get_m_Array()[i][j] * x[i][j]);
		model.add(IloMaximize(env, obj_fun));
		//add simple constraints 
		for (int i = 0; i < R.Candidates(); i++)
			for (int j = 0; j < R.Candidates(); j++)
				if (i != j)
					model.add(x[i][j] + x[j][i] == 1);
				else
					model.add(x[i][i] == 0);
		// add 3-dicycles constraints
		for (int i = 0; i < R.Candidates(); i++)
			for (int j = 0; j < R.Candidates(); j++)
				for (int k = 0; k < R.Candidates(); k++)
					if ((i != j) && (j != k) && ( k != i))
						model.add(x[i][j] + x[j][k] + x[k][i] <= 2);
		//time setting
		cplex.setParam(IloCplex::TiLim, 300);
		cplex.setOut(env.getNullStream());
		time_t start, end;
		time(&start);
		if ( !cplex.solve() ) {
			env.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}
		time(&end);
		//fill the solution
		s.time = difftime(end,start);
		s.obj_value = cplex.getObjValue();	
		for(int i =0; i < R.Candidates(); i++)
			for(int j =0; j < R.Candidates(); j++)
				s.sol_vector[i][j] = cplex.getValue(x[i][j]);
		cout << "Time is = " << s.time << endl;
		cout << "OBJ Value is  = " << s.obj_value << endl;	
		vector<int> order = MatrixToOrdering(s.sol_vector, R.Candidates());
		//cout << "opt order is " << endl;
		//cout << order << endl;
		env.end();
	}
	catch ( IloException & e){
		cerr << "Concert exception caught: " << e << endl;
	} catch (...){
		cerr << "Unknown exception caught" << endl;
	}
	return s;
}
solution HeurSolver::LPRoundingHeuristic(solution& lp){
	solution s(R.Candidates());
	try {
		IloEnv env;
		//create the model and variables for heuristic
		IloModel heuristic_model(env);
		IloCplex heuristic_cplex(heuristic_model);
		BoolVarMatrix x_heur(env, R.Candidates());
		for(int i =0; i < R.Candidates(); i++)
			x_heur[i] = IloBoolVarArray(env, R.Candidates());
		//set the parameters for the model 
		heuristic_cplex.setParam(IloCplex::RootAlg, IloCplex::Primal);
		//create and add the objective function
		IloExpr obj_fun(env);
		for(int i = 0; i < R.Candidates(); i++)
			for(int j = 0; j < R.Candidates(); j++)
				if (i != j)
					obj_fun += (R.Get_m_Array()[i][j] * x_heur[i][j]);
		heuristic_model.add(IloMinimize(env, obj_fun));
		//add simple constraints 
		for (int i = 0; i < R.Candidates(); i++)
			for (int j = 0; j < R.Candidates(); j++)
				if (i != j)
					heuristic_model.add(x_heur[i][j] + x_heur[j][i] == 1);
				else
					heuristic_model.add(x_heur[i][i] == 0);
		// add 3-dicycles constraints
		for (int i = 0; i < R.Candidates(); i++)
			for (int j = 0; j < R.Candidates(); j++)
				for (int k = 0; k < R.Candidates(); k++)
					if ((i != j) && (j != k) && ( k != i))
						heuristic_model.add(x_heur[i][j] + x_heur[j][k] + x_heur[k][i] <= 2);
		//fix varibles based on the lp solution
		for (int i = 0; i < R.Candidates(); i++)
			for(int j =0; j < R.Candidates(); j++)
				if(lp.sol_vector[i][j] == 1)
					heuristic_model.add(x_heur[i][j] == 1);
				else if (lp.sol_vector[i][j] == 0 )
					heuristic_model.add(x_heur[i][j] == 0);
		heuristic_cplex.setOut(env.getNullStream());
		time_t start, end;
		time(&start);
		if ( !heuristic_cplex.solve() ) {
			env.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}
		time(&end);
		//fill the solution
		s.time = difftime(end, start);
		s.obj_value = heuristic_cplex.getObjValue();	
		for(int i =0; i < R.Candidates(); i++)
			for(int j =0; j < R.Candidates(); j++)
				s.sol_vector[i][j] = heuristic_cplex.getValue(x_heur[i][j]);
		env.end();
	}
	catch ( IloException & e){
		cerr << "Concert exception caught: " << e << endl;
	} catch (...){
		cerr << "Unknown exception caught" << endl;
	}
	return s;
}

vector<int> HeurSolver::IterativeFixing(solution& lp, vector<int>& init_order, double threshold, int iteration)
{
	if (iteration == 6)
		return init_order;
	else
		try {
			//cout << "****** init_order value is " << OrderingValue(init_order) << endl;
			solution s(R.Candidates());
			IloEnv env;
			IloModel model(env);
			IloCplex cplex(model);
			BoolVarMatrix x(env, R.Candidates());
			for(int i =0; i < R.Candidates(); i++)
				x[i] = IloBoolVarArray(env, R.Candidates());
			//cplex.setParam(IloCplex::RootAlg, IloCplex::Primal);
			cplex.setOut(env.getNullStream());
			//	cplex.setParam(IloCplex::TiLim, 50);
			IloNumVarArray stVar(env);
			for(int i = 0; i < R.Candidates(); i++)
				for(int j = 0; j < R.Candidates(); j++)
					stVar.add(x[i][j]);	
			IloNumArray stVal = OrderToNumArray(env,init_order);
			//create and add the objective function
			IloExpr obj_fun(env);
			for(int i = 0; i < R.Candidates(); i++)
				for(int j = 0; j < R.Candidates(); j++)
					if (i != j)
						obj_fun += (R.Get_m_Array()[i][j] * x[i][j]);
			model.add(IloMaximize(env, obj_fun));
			//add simple constraints 
			for (int i = 0; i < R.Candidates(); i++)
				for (int j = 0; j < R.Candidates(); j++)
					if (i != j)
						model.add(x[i][j] + x[j][i] == 1);
					else
						model.add(x[i][i] == 0);
			// add 3-dicycles constraints
			for (int i = 0; i < R.Candidates(); i++)
				for (int j = 0; j < R.Candidates(); j++)
					for (int k = 0; k < R.Candidates(); k++)
						if ((i != j) && (j != k) && ( k != i))
							model.add(x[i][j] + x[j][k] + x[k][i] <= 2);
			int nConstFixed =0;
			//fix varibles based on the lp solution
			for (int i = 0; i < R.Candidates(); i++)
				for(int j =0; j < R.Candidates(); j++)
					if(lp.sol_vector[i][j] >= threshold){
						nConstFixed++;
						model.add(x[i][j] == 1);
					} else if (lp.sol_vector[i][j] <= 1-threshold){
						model.add(x[i][j] == 0);
						nConstFixed++;
					}

			cout << "Percentage of constraints fixed : " << double(nConstFixed)/(R.Candidates()*R.Candidates()) *100  << endl;
			//	cplex.setParam(IloCplex::EpGap, 0.1);
			cplex.addMIPStart(stVar, stVal, IloCplex::MIPStartAuto,  "secondMIPStart");
			time_t start, end;
			time(&start);
			if ( !cplex.solve() ) {
				env.error() << "Failed to optimize LP." << endl;
				throw (-1);
			}
			time(&end);
			//fill the solution
			s.time = difftime(end, start);
			s.obj_value = cplex.getObjValue();
			cout << "The initial sol is " << s.obj_value << " and threshold is : " << threshold <<  endl;
			for(int i =0; i < R.Candidates(); i++)
				for(int j =0; j < R.Candidates(); j++)
					s.sol_vector[i][j] = cplex.getValue(x[i][j]);
			env.end();
			init_order.clear();
			init_order = MatrixToOrdering(s.sol_vector, R.Candidates());
			//cout << "****** init_order value is " << OrderingValue(init_order) << endl;
			return IterativeFixing(lp,init_order, threshold + 0.05, iteration + 1);
		}
	catch(IloException& e ){
		cerr << "Concert exception caught : " << e << endl;
	}catch (...){
		cerr << "Unknown exception caught : " << endl;
	}	
}


solution HeurSolver::CreateInitialSolution(solution& lp)
{
	cout << "Initial solution creation started" << endl;
	solution s(R.Candidates());
	int nConstFixed = 0;
	IloEnv env;
	IloModel model(env);
	IloCplex cplex(model);
	BoolVarMatrix x(env, R.Candidates());
	for(int i =0; i < R.Candidates(); i++)
		x[i] = IloBoolVarArray(env, R.Candidates());
	//set the parameters for the model 
	//cplex.setParam(IloCplex::RootAlg, IloCplex::Primal);
	cplex.setOut(env.getNullStream());
	cplex.setParam(IloCplex::TiLim, 250);
	//create and add the objective function
	IloExpr obj_fun(env);
	for(int i = 0; i < R.Candidates(); i++)
		for(int j = 0; j < R.Candidates(); j++)
			if (i != j)
				obj_fun += (R.Get_m_Array()[i][j] * x[i][j]);
	model.add(IloMaximize(env, obj_fun));
	//add simple constraints 
	for (int i = 0; i < R.Candidates(); i++)
		for (int j = 0; j < R.Candidates(); j++)
			if (i != j)
				model.add(x[i][j] + x[j][i] == 1);
			else
				model.add(x[i][i] == 0);
	// add 3-dicycles constraints
	for (int i = 0; i < R.Candidates(); i++)
		for (int j = 0; j < R.Candidates(); j++)
			for (int k = 0; k < R.Candidates(); k++)
				if ((i != j) && (j != k) && ( k != i))
					model.add(x[i][j] + x[j][k] + x[k][i] <= 2);
	//fix varibles based on the lp solution
	for (int i = 0; i < R.Candidates(); i++)
		for(int j =0; j < R.Candidates(); j++)
			if(lp.sol_vector[i][j] >= .8){
				model.add(x[i][j] == 1);
				nConstFixed++;
			} else if (lp.sol_vector[i][j] <= 0.2 ){
				model.add(x[i][j] == 0);
				nConstFixed++;
			}
	cout << "Percentage of constraints fixed : " << double(nConstFixed)/(R.Candidates()*R.Candidates()) *100  << endl;
	time_t start, end;
	time(&start);
	if ( !cplex.solve() ) {
		env.error() << "Failed to optimize LP." << endl;
		throw (-1);
	}
	time(&end);
	//fill the solution
	s.time = difftime(end, start);
	s.obj_value = cplex.getObjValue();
	cout << "The initial sol is " << s.obj_value << endl;
	for(int i =0; i < R.Candidates(); i++)
		for(int j =0; j < R.Candidates(); j++)
			s.sol_vector[i][j] = cplex.getValue(x[i][j]);
	env.end();
	return s;
}
double HeurSolver::OrderingValue(vector<int> ordering){
	int size = ordering.size();
	double sum = 0;
	for(int i = 0 ; i < size; i++)
		for(int j = i+1; j < size ; j++)
			sum += R.Get_m_Array()[ordering[i]][ordering[j]];
	return sum;
}
IloNumArray HeurSolver::OrderToNumArray(IloEnv env, vector<int> order){
	IloNumArray sVal(env);
	int size = order.size();
	for(int i = 0; i < size; i++)
		for(int j = 0; j < size; j++)
			if (i == j )
				sVal.add(0);
			else if ( i < j )
				sVal.add(1);
			else 
				sVal.add(0);
	return sVal;
}
vector<vector<int> > HeurSolver::GetSubRanking(vector<int> partial_order){
	// get the subatrix related to ranking of candidates in the ve
	//vector of partial_order
	int size = partial_order.size();
	vector<vector<int> > rank;
	rank.resize(size);
	for(int i = 0; i < size; i++)
		rank[i].resize(size);
	for(int i = 0; i < size; i++)
		for(int j =0; j < size; j++)
			rank[i][j] =0;
	for(int i = 0; i < size; i++)
		for(int j = 0; j < size ; j++)
			rank[i][j] = R.Get_m_Array()[partial_order[i]][partial_order[j]];
	return rank;
}

void HeurSolver::UpdateVarHistory(vector<vector<int> >& history, vector<int>& order){

}
/*
   duoble HeurSolver::OrderingValue(vector<int>& order, double obj_val, int object, int old_pos, int new_pos){
   double sum = obj_value;
   if(old_pos < new_pos)
   for(int i = old_pos; i < new_pos; i++)
   sum+= R.Get_m_Array()[order[new_pos]][i] - R.Get_m_Array()[i][order[new_pos]];
   else if (new_pos < old_pos)
   for(int i = new_pos+1; i < old_pos; i++)
   sum+= R.Get_m_Array()[i][order[new_pos]]-R.Get_m_Array()[order[new_pos]][i] ;
   return sum;	
   }

   vector<int> HeurSolver::LinKernighanLocalSearch2(vector<int>& order, int size){
   vector<pair<int,int> > inserts;
   vector<int> obj_positions = ObjectPosition(order);
   vector<int> v(order.begin(),order.end());
   vector<double> gain(size);
   for(int i = 0; i < size ; i++)
   gain[i] = 0;
   vector<<bool> obj_history(size);
   for(int i = 0; i < size; i++)
   obj_history[i] = false;
   int m = 0;
   double obj_value = OrderingValue(order);
   while(m < size){
   int gain_max = -1000000;
   int gain_object = -1;
   int gain_pos = -1;
   for(int i = 0; i < size; i++){
   for(int pos = 0; pos < size; pos++)
   if(i != pos && !obj_history[order[i]]){
   int g = OrderingValue(order,obj_value, order[i], i, pos) - obj_value;
   if( g > gain_max){
   gain_max = g;
   gain_pos = pos;
   gain_object = i;
   }
   }
   }
   obj_history[order[gain_object]] = true;
   inserts.push_back(make_pair(i,gain_pos));
   gain[i] = gain_max;
   m++;
   }
   double gain_sum =gain[0];
   double max = gain[0];
   int index_max = 0;
   for(int i = 1; i < size; i++){
   gain_sum += gain[i];
   if(gain_sum > max){
   max = gain_sum;
   index_max = i;
   }
   }
   if (max > 0){
   cout << "counter is = " << counter++ << endl;
   cout << "seuccessfull" << endl;
   for(int i =0; i <= index_max; i++ )
   swap(v[switches[i].first],v[switches[i].second]);
   cout << "value in lin kernighan is " << OrderingValue(v) << endl;
   return order;
   }
   return v;
   }
   */
vector<int> HeurSolver::LinKernighanLocalSearch(vector<int>& order, int size){
	static int counter =0;	
	cout << "counter is " << counter++ << endl;
	vector<pair<int,int> > switches;
	vector<int> v(order.begin(),order.end());
	vector<double> gain(size);
	for(int i = 0; i < size ; i++)
		gain[i] = 0;
	vector<vector<int> > pair_history(size);
	for(int i =0 ; i < size; i++)
		pair_history[i].resize(size);
	for(int i = 0; i<size; i++)
		for(int j =0; j< size; j++) 
			pair_history[i][j] = 0;
	int m = 0;
	while(m <= floor(size/2)){
		int first_indx = rand()%size;
		int second_indx = rand()%size;
		if (first_indx  == second_indx)
			while(first_indx == second_indx)
				second_indx = rand()%size;
		if(pair_history[first_indx][second_indx] == 0){
			pair_history[first_indx][second_indx] = 1;
			pair_history[second_indx][first_indx] = 1; 	
			int temp = OrderingValue(order);
			swap(order[first_indx], order[second_indx]);
			gain[m] = OrderingValue(order) - temp;
			if (first_indx < second_indx)
				switches.push_back(make_pair(first_indx,second_indx));
			else 
				switches.push_back(make_pair(second_indx, first_indx));
			m++;
		}
	}
	double gain_sum =gain[0];
	double max = gain[0];
	int index_max = 0;
	for(int i = 1; i < size; i++){
		gain_sum += gain[i];
		if(gain_sum > max){
			max = gain_sum;
			index_max = i;
		}
	}
	if (max > 0){
		cout << "counter is = " << counter++ << endl;
		cout << "seuccessfull" << endl;
		for(int i =0; i <= index_max; i++ )
			swap(v[switches[i].first],v[switches[i].second]);
		cout << "value in lin kernighan is " << OrderingValue(v) << endl;
		return order;
	}
	return v;
}
/*
   vector<int> HeurSolver::LastStepMIP(vector<int>& order){
   int size = R.Candidates();
   vector<int> vec(size);
   try
   {
   IloEnv env;
   IloModel model(env);
   IloCplex cplex(model);
   IloNumVarArray start_var(env);
   BoolVarMatrix x(env,size);
   for(int i = 0; i < size; i++)
   x[i] = IloBoolVarArray(env, size);
   for(int i = 0; i < size; i++)
   for(int j = 0; j < size; j++)
   start_var.add(x[i][j]);
   IloExpr obj_fun(env);
   for(int i = 0; i < size; i++)
   for(int j = 0; j < size; j++)
//if (i != j)
obj_fun += (matrix[i][j] * x[i][j]);
model.add(IloMaximize(env, obj_fun));
//add simple constraints 
for (int i = 0; i < size; i++)
for (int j = 0; j < size; j++)
if (i != j)
model.add(x[i][j] + x[j][i] == 1);
else
model.add(x[i][i] == 0);
// add 3-dicycles constraints
for (int i = 0; i < size; i++)
for (int j = 0; j < size; j++)
for (int k = 0; k < size; k++)
if ((i != j) && (j != k) && ( k != i))
model.add(x[i][j] + x[j][k] + x[k][i] <= 2);

//create the windows and size and stuff

//	cplex.setParam(IloCplex::EpGap, 0.1);
//cplex.addMIPStart(start_var, start_val, IloCplex::MIPStartAuto,  "secondMIPStart");
time_t start, end;
time(&start);
//cplex.setParam(IloCplex::TiLim, 200);
if ( !cplex.solve() ) {
env.error() << "Failed to optimize LP." << endl;
throw (-1);
}
time(&end);
double run_time = difftime(end, start);
cout << "The subproblem running time is : " << run_time << endl;
// prepare the output ranking	
vector<vector<int> > sol;
sol.resize(size);
for(int i = 0; i < size; i++)
sol[i].resize(size);
//put the output of cplex to 2D vector
for(int i = 0; i < size ; i ++)
for(int j = 0; j < size ; j++)
if (cplex.getValue(x[i][j] == 1))
sol[i][j] = 1;
else 
sol[i][j] = 0;
//change  the 2D vector solution to ordering
vector<int> temp = MatrixToOrdering(sol, size);
for(int i =0; i < size ; i++)
vec[i] = cand_subset[temp[i]];
cout << "********lollaala********" << endl;
env.end();
}
catch ( IloException & e){
cerr << "Concert exception caught in Local search: " << e << endl;
} catch (...){
	cerr << "Unknown exception caught in local search" << endl;
}
return vec;
}

*/

vector<int> HeurSolver::Preprocessing(){
	vector<vector<int> > subranking;
	vector<int> partial_order;	
	vector<int> temp_2;
	IloEnv env;

	if (R.Candidates() <= 100 ){
		solution lp = SolveLP();	
		//cout << "LP solution is : " << lp.obj_value << endl;
		solution initial = CreateInitialSolution(lp);
		vector<int> init_ordering = MatrixToOrdering(initial.sol_vector, R.Candidates());
		int mid_point = floor(R.Candidates()/2);
		for(int i =0; i < mid_point; i++)
			partial_order.push_back(init_ordering[i]);
		subranking = GetSubRanking(partial_order);
		IloNumArray st = OrderToNumArray(env,partial_order);
		vector<int> temp = SolveSubProblem(subranking, partial_order,st);

		partial_order.clear();
		subranking.clear();
		for(int i = mid_point; i < R.Candidates(); i++)
			partial_order.push_back(init_ordering[mid_point]);
		subranking = GetSubRanking(partial_order);
		IloNumArray st_1 = OrderToNumArray(env,partial_order);
		vector<int> temp_1 = SolveSubProblem(subranking, partial_order,st_1);

		temp_2.reserve( temp.size() + temp_1.size()); // preallocate memory
		temp_2.insert( temp_2.end(), temp.begin(), temp.end() );
		temp_2.insert( temp_2.end(), temp_1.begin(), temp_1.end() );
		//cout << "combine  ordeing value before is :  " << OrderingValue(temp_2) << endl;

		partial_order.clear();
		subranking.clear();
		int one_fifth_point = floor(R.Candidates()/5);
		for(int i = one_fifth_point; i < 4*one_fifth_point; i++)
			partial_order.push_back(temp_2[i]);
		subranking = GetSubRanking(partial_order);
		IloNumArray st_3 = OrderToNumArray(env,partial_order);
		vector<int> temp_3 = SolveSubProblem(subranking, partial_order,st_3);

		for(int i = 0; i < temp_3.size(); i++)
			temp_2[i+one_fifth_point] = temp_3[i];
		temp_3.clear();	
		cout << "value after preprocess : " << OrderingValue(temp_2) << endl; 
		return temp_2;

	}else if(R.Candidates() > 100 && R.Candidates() <= 150){

		//	vector<int> init_ordering(R.Candidates());
		//	for(int i = 0; i < R.Candidates(); i++)
		//		init_ordering[i] = i;
		//	random_shuffle(init_ordering.begin(), init_ordering.end());

		solution lp = SolveLP();
		solution initial = CreateInitialSolution(lp);
		vector<int> init_ordering = MatrixToOrdering(initial.sol_vector, R.Candidates());
		int one_third_point = floor(R.Candidates()/3);
		int two_third_point =  2*one_third_point;
		for(int i = 0; i < one_third_point; i++)
			partial_order.push_back(init_ordering[i]);
		vector<vector<int> > subranking = GetSubRanking(partial_order);
		IloNumArray st = OrderToNumArray(env,partial_order);
		vector<int> temp = SolveSubProblem(subranking, partial_order,st);

		partial_order.clear();
		subranking.clear();
		for(int i = one_third_point; i < two_third_point; i++)
			partial_order.push_back(init_ordering[i]);
		subranking = GetSubRanking(partial_order);
		IloNumArray st_1 = OrderToNumArray(env,partial_order);
		vector<int> temp_1 = SolveSubProblem(subranking, partial_order,st_1);

		partial_order.clear();
		subranking.clear();
		for(int i = two_third_point; i < R.Candidates(); i++)
			partial_order.push_back(init_ordering[i]);
		subranking = GetSubRanking(partial_order);
		IloNumArray st_3 = OrderToNumArray(env,partial_order);
		vector<int> temp_3 = SolveSubProblem(subranking, partial_order,st_3);

		temp_2.reserve( temp.size() + temp_1.size()+ temp_3.size()); // preallocate memory
		temp_2.insert( temp_2.end(), temp.begin(), temp.end() );
		temp_2.insert( temp_2.end(), temp_1.begin(), temp_1.end() );
		temp_2.insert( temp_2.end(), temp_3.begin(), temp_3.end() );
		//cout << "combine  ordeing value before is :  " << OrderingValue(temp_2) << endl;	
		cout << "value after preprocess : " << OrderingValue(temp_2) << endl; 
		return temp_2;

	}else if (R.Candidates() > 150 && R.Candidates() <= 200 ){
		vector<int> init_ordering(R.Candidates());
		for(int i = 0; i < R.Candidates(); i++)
			init_ordering[i] = i;
		random_shuffle(init_ordering.begin(), init_ordering.end());
		int quarter = floor(R.Candidates()/4); 
		for(int i =0; i < quarter; i++)
			partial_order.push_back(init_ordering[i]);
		vector<vector<int> > subranking = GetSubRanking(partial_order);
		IloNumArray st = OrderToNumArray(env,partial_order);
		vector<int> temp = SolveSubProblem(subranking, partial_order,st);

		partial_order.clear();
		subranking.clear();
		for(int i = quarter; i < 2*quarter; i++)
			partial_order.push_back(init_ordering[i]);
		subranking = GetSubRanking(partial_order);
		IloNumArray st_1 = OrderToNumArray(env,partial_order);
		vector<int> temp_1 = SolveSubProblem(subranking, partial_order,st_1);

		partial_order.clear();
		subranking.clear();
		for(int i = 2*quarter; i < 3*quarter; i++)
			partial_order.push_back(init_ordering[i]);
		subranking = GetSubRanking(partial_order);
		IloNumArray st_3 = OrderToNumArray(env,partial_order);
		vector<int> temp_3 = SolveSubProblem(subranking, partial_order,st_3);

		partial_order.clear();
		subranking.clear();
		for(int i = 3*quarter; i < R.Candidates(); i++)
			partial_order.push_back(init_ordering[i]);
		subranking = GetSubRanking(partial_order);
		IloNumArray st_4 = OrderToNumArray(env,partial_order);
		vector<int> temp_4 = SolveSubProblem(subranking, partial_order,st_4);

		temp_2.reserve( temp.size() + temp_1.size()+ temp_3.size() + temp_4.size() ); // preallocate memory
		temp_2.insert( temp_2.end(), temp.begin(), temp.end() );
		temp_2.insert( temp_2.end(), temp_1.begin(), temp_1.end() );
		temp_2.insert( temp_2.end(), temp_3.begin(), temp_3.end() );
		temp_2.insert( temp_2.end(), temp_4.begin(), temp_4.end() );
		//cout << "combine  ordeing value before is :  " << OrderingValue(temp_2) << endl;	
		cout << "value after preprocess : " << OrderingValue(temp_2) << endl; 
		return temp_2;

	}else if (R.Candidates() > 200 && R.Candidates() <= 250){
		vector<int> init_ordering(R.Candidates());
		for(int i = 0; i < R.Candidates(); i++)
			init_ordering[i] = i;
		random_shuffle(init_ordering.begin(), init_ordering.end());	
		int break_point = floor(R.Candidates()/5);
		temp_2.reserve(R.Candidates());	
		for(int j = 0; j < 5; j++ ){	
			for(int i = j*break_point; i < (j+1)*break_point; i++)
				partial_order.push_back(init_ordering[i]);
			subranking = GetSubRanking(partial_order);
			IloNumArray st = OrderToNumArray(env,partial_order);
			vector<int> temp = SolveSubProblem(subranking, partial_order,st);
			temp_2.insert(temp_2.end(), temp.begin(),temp.end());
			partial_order.clear();
			subranking.clear();
		}
		return temp_2;	
	}else if (R.Candidates() == 500 ){
		vector<int> init_ordering(R.Candidates());
		for(int i = 0; i < R.Candidates(); i++)
			init_ordering[i] = i;
		random_shuffle(init_ordering.begin(), init_ordering.end());
		int break_point = floor(R.Candidates()/10);
		temp_2.reserve(R.Candidates());	
		for(int j = 0; j < 10; j++ ){	
			for(int i = j*break_point; i < (j+1)*break_point; i++)
				partial_order.push_back(init_ordering[i]);
			subranking = GetSubRanking(partial_order);
			IloNumArray st = OrderToNumArray(env,partial_order);
			vector<int> temp = SolveSubProblem(subranking, partial_order,st);
			temp_2.insert(temp_2.end(), temp.begin(),temp.end());
			partial_order.clear();
			subranking.clear();
		}
		cout << "value after preprocess : " << OrderingValue(temp_2) << endl; 
		return temp_2;	
	} 
	return temp_2;
}

/*
   void HeurSolver::FirstNeighborhood(vector<int>& current_sol ){
//explore the first neigborhood until it can not be improved
int no_improve_upper_bound = 100;
int no_imrove_counter = 0;
while(no_improve_counter < no_improve_uuper_bound){
vector<int> temp = LinKernighanLocalSearch(current_sol, R.Candidates());
if(OrderingValue(temp) < OrderingValue(current_sol))
no_improve_count++;
else if (OrderingValue(temp) > OrderingValue(current_sol)){
current_sol = temp;
no_improve_counter = 0;
}
else {
if (temp != current_sol)
//tie breaking rule
no_imporve_counetr = 0;
}	
}
}


void HeurSolver::VariableNeighborhoodSearch(){
cout<<"#------ The Neighborhood search is starting -------#" << endl;
srand (time(0));
time_t start, end;
time(&start);
//track nuber of times we had progeressive move
vector<int> size_improvment_vector(R.Candidates());
for(int i= 0; i < R.Candidates(); i++)
size_improvment_vector[i] = -1;
int nEffectiveMove =0;
//keep how many times i progressive moves x_ij has value 1
vector<vector<int> > history(R.Candidates());
for(int i = 0; i < R.Candidates(); i++)
history[i].resize(R.Candidates());
for(int i = 0; i < R.Candidates(); i++)
for(int j =0 ; j < R.Candidates(); j++)
history[i][j] = 0;
solution lp = SolveLP();
solution initial = CreateInitialSolution(lp);
vector<int> current_sol = Preprocessing(initial);
int iteration = 0;								//number of local search iteration
while(iteration++ < 2000 ){
FirstNeighborhood(current_sol);	
}



//implement lin-kernighan type local search move

cout << "The LP solution is :" << lp.obj_value << endl;
cout << "The initial solution value is " << initial.obj_value << endl;
int k = 3;										// how many varibales are relexaed in subproblem
int p = 2;										// use for size of the subproblem
int seq_size= 6 ;								// size of  the subproblem which is p*k
int max_value = OrderingValue(temp_2);			// the max obj function value found so far
int improve_counter = 0;						// keep counter to see in how many past iteratio we did not have any improvement
int tabu_counter = 0;							// keep number of solution currently in tabue table
int  tabu_table_cap = 10;
int size_increase = 6;
solve_status status = fully;
vector<int> random_index;
ofstream file("../data/random_index.txt");
while(iteration++ < 5000)
{
time(&end);
if(difftime(end, start) > 100)
break;
if(iteration > 500 && iteration < 1000)
size_increase = 4;
else if (iteration > 1000 && iteration < 1500)
	size_increase = 3;
	else 
	size_increase = 2;
	//	seq_size = p*k;
	cout << endl << " and seq_size is : " << seq_size << endl;
if (seq_size > 56)
	break;
	partial_order.clear();
	subranking.clear();
	int indx_begin = rand()%(R.Candidates() - seq_size);
	cout << endl << "**** rand inedex is " << indx_begin << " **** " << endl;
	random_index.push_back(indx_begin);
	for(int i = 0; i < seq_size; i++)
	partial_order.push_back(temp_2[i+indx_begin]);
	subranking = GetSubRanking(partial_order);
	IloNumArray start_val = OrderToNumArray(env,partial_order);
	vector<int> ls = LocalSearch(subranking, partial_order, start_val, status,p);
	vector<int> v(temp_2.begin(), temp_2.end());
	for(int i = 0; i < seq_size; i++)
	v[i+indx_begin] = ls[i];
	int value = OrderingValue(v);
	cout << "$$$$$ value = " << value << " and max_value is = " << max_value << endl;
	if(value > max_value){
		nEffectiveMove++;
		size_improvment_vector[seq_size]++;
		for(int i = 0; i < seq_size; i++)
			temp_2[i+indx_begin] = ls[i];	
		//update the histpry vector
		cout << endl << "******improvmner*****" << endl << endl << endl;
		max_value = value;
		improve_counter =0;
		TS t(value, temp_2);
		if (tabu_counter == tabu_table_cap){
			multiset<TS>::iterator it = tabu_table.begin();
			tabu_table.erase(it);
			tabu_counter--;
		}
		tabu_table.insert(t);
		tabu_counter++;
	}
else if (value <= max_value){
	improve_counter++;
	cout << "$$$$the couneter improvement is = " << improve_counter << endl;
}
if (improve_counter >  100){
	//	k++;//	p++;
	seq_size += size_increase;
	improve_counter = 0;
}
cout << endl << " The solution after iteration " << iteration << " is "  << value << endl;
cout << endl << " The best solution found up to iteration " << iteration << " is "  << max_value << endl;
}
cout << "combine  ordeing value after last step  is : " << OrderingValue(temp_2) << endl;
vector<vector<int> > mat1 = OrderToMatrix(temp_2);
int s = 0;
for(int i = 0; i < R.Candidates(); i++)
for(int j =0; j < R.Candidates(); j++)
s+= mat1[i][j]*R.Get_m_Array()[i][j];
cout << "the matrix multiply sum is = " << s << endl;
//	LPHeuristic(lp, temp_2);


cout << "combine  ordeing value after last step  is : " << OrderingValue(temp_2) << endl;
//implement lin-kernighan type local search move
for(int i = 0; i < 2000; i++){
	vector<int> dd = LinKernighanLocalSearch(temp_2, R.Candidates());
	temp_2 = dd;
}


//solve by cplex improvement
IloNumArray start_val = OrderToNumArray(env,init_ordering);
vector<int> ls = ImproveByCplex(init_ordering, start_val);
cout << "improveBYcplex is " << OrderingValue(ls) << endl;
vector<vector<int> > mat = OrderToMatrix(ls);
int s = 0;
for(int i = 0; i < R.Candidates(); i++)
for(int j =0; j < R.Candidates(); j++)
s+= mat[i][j]*R.Get_m_Array()[i][j];
cout << "the matrix multiply sum is = " << s << endl;

vector<vector<int> > mat = OrderToMatrix(temp_2);
s = 0;
for(int i = 0; i < R.Candidates(); i++)
for(int j =0; j < R.Candidates(); j++)
s+= mat[i][j]*R.Get_m_Array()[i][j];
cout << "the matrix multiply sum is = " << s << endl;
cout << "init ordering value is : " << OrderingValue(init_ordering) << endl;
cout << "init ordering is : " << endl << init_ordering << endl;
cout << "combine  ordeing value befor the local search was : " << OrderingValue(temp_3) << endl;
cout << "combine  ordeing value after last step  is : " << OrderingValue(temp_2) << endl;
cout << " The final ordeering is : " << endl << temp_2 << endl;
cout << "The best solution found is : " << max_value << endl;
file << random_index << endl;
file.close();
double run_time = difftime(end, start);
double gap = ((lp.obj_value - max_value) / max_value) * 100;
cout << "the running time is : " << run_time << endl; 
cout << "the gap is = " << gap << endl; 
cout << "### The Iterative LP Roundig is ending " << endl;
}
*/

void HeurSolver::KernelSearch(){
	cout<<"#------ The Neighborhood search is starting -------#" << endl;
	srand (time(0));
	time_t start, end;
	time(&start);
	//track nuber of times we had progeressive move
	vector<int> size_improvment_vector(R.Candidates());
	for(int i= 0; i < R.Candidates(); i++)
		size_improvment_vector[i] = -1;
	int nEffectiveMove =0;
	//keep how many times i progressive moves x_ij has value 1
	vector<vector<int> > history(R.Candidates());
	for(int i = 0; i < R.Candidates(); i++)
		history[i].resize(R.Candidates());
	for(int i = 0; i < R.Candidates(); i++)
		for(int j =0 ; j < R.Candidates(); j++)
			history[i][j] = 0;
	vector<int> init_order(R.Candidates());
	for(int i = 0; i < R.Candidates(); i++)
		init_order[i] = i;
	random_shuffle(init_order.begin(), init_order.end());
	cout << "ordering value init " << OrderingValue(init_order) << endl;
	solution lp = SolveLP();
	//solution initial = CreateInitialSolution(lp);
	//	vector<int> current_sol = Preprocessing(initial);
	vector<int> final_order = IterativeFixing(lp,init_order,0.71,0);
	cout << "ordering value final " << OrderingValue(final_order) << endl;
}

void HeurSolver::MIP_Heuristic(){
	//cout << endl << "### The Iterative MIP (with intial solution) is starting....." << endl;
	srand (time(0));
	time_t start, end;
	time(&start);
	multiset<TS> tabu_table;
	IloEnv env;
	//track nuber of times we had progeressive move
	vector<int> size_improvment_vector(R.Candidates());
	for(int i= 0; i < R.Candidates(); i++)
		size_improvment_vector[i] = -1;
	int nEffectiveMove =0;
	//keep how many times i progressive moves x_ij has value 1
	vector<vector<int> > history(R.Candidates());
	for(int i = 0; i < R.Candidates(); i++)
		history[i].resize(R.Candidates());
	for(int i = 0; i < R.Candidates(); i++)
		for(int j =0 ; j < R.Candidates(); j++)
			history[i][j] = 0;
	vector<int> temp_2 = Preprocessing();
	vector<int> partial_order;	
	vector<vector<int> > subranking ;	
	/*
	   solution lp = SolveLP();
	   solution initial = CreateInitialSolution(lp);
	   cout << "The LP solution is :" << lp.obj_value << endl;
	   cout << "The initial solution value is " << initial.obj_value << endl;
	   vector<int> init_ordering = MatrixToOrdering(lp.sol_vector, R.Candidates());
	   cout << "init ordering value is : "  << OrderingValue(init_ordering) << endl;
	   vector<vector<int> > subranking;
	   vector<int> partial_order;	
	   vector<int> temp_2;
	   vector<int> temp_3;
	   if(R.Candidates() == 100){
	   for(int i =0; i < 50; i++)
	   partial_order.push_back(init_ordering[i]);
	   subranking = GetSubRanking(partial_order);
	   IloNumArray st = OrderToNumArray(env,partial_order);
	   vector<int> temp = SolveSubProblem(subranking, partial_order,st);

	   partial_order.clear();
	   subranking.clear();
	   for(int i =0; i < 50; i++)
	   partial_order.push_back(init_ordering[i+50]);
	   subranking = GetSubRanking(partial_order);
	   IloNumArray st_1 = OrderToNumArray(env,partial_order);
	   vector<int> temp_1 = SolveSubProblem(subranking, partial_order,st_1);

	   temp_2.reserve( temp.size() + temp_1.size()); // preallocate memory
	   temp_2.insert( temp_2.end(), temp.begin(), temp.end() );
	   temp_2.insert( temp_2.end(), temp_1.begin(), temp_1.end() );
	   cout << "combine  ordeing value before is :  " << OrderingValue(temp_2) << endl;

	   partial_order.clear();
	   subranking.clear();
	   for(int i = 20; i < 80; i++)
	   partial_order.push_back(temp_2[i]);
	   subranking = GetSubRanking(partial_order);
	   IloNumArray st_3 = OrderToNumArray(env,partial_order);
	   vector<int> temp_3 = SolveSubProblem(subranking, partial_order,st_3);
	   for(int i = 0; i < 60; i++)
	   temp_2[i+20] = temp_3[i];
	   temp_3.clear();
	   temp_3 = temp_2;
	   }
	   else if (R.Candidates() != 100)
	   temp_2 = init_ordering;

	   cout << "combine  ordeing value after last step  is : " << OrderingValue(temp_2) << endl;

	//implement lin-kernighan type local search move
	for(int i = 0; i < 2000; i++){
	vector<int> dd = LinKernighanLocalSearch(temp_2, R.Candidates());
	temp_2 = dd;
	}
	*/
	//vector<int> temp_2= init_ordering;
	//vector<vector<int> > subranking;
	//local search second phase
	int iteration = 0;								//number of local search iteration
	int k = 3;										// how many varibales are relexaed in subproblem
	int p = 2;										// use for size of the subproblem
	int seq_size= 6 ;								// size of  the subproblem which is p*k
	int max_value = OrderingValue(temp_2);			// the max obj function value found so far
	int improve_counter = 0;						// keep counter to see in how many past iteratio we did not have any improvement
	int tabu_counter = 0;							// keep number of solution currently in tabue table
	int  tabu_table_cap = 10;
	int size_increase = 6;
	solve_status status = fully;
	vector<int> random_index;
	//ofstream file("../data/random_index.txt");
	while(iteration++ < 5000)
	{
		time(&end);
		if(difftime(end, start) > 400)
			break;
		if(iteration > 500 && iteration < 1000)
			size_increase = 4;
		else if (iteration > 1000 && iteration < 1500)
			size_increase = 3;
		else 
			size_increase = 2;
		//	seq_size = p*k;
		//cout << endl << " and seq_size is : " << seq_size << endl;
		if (seq_size > 56)
			break;
		partial_order.clear();
		subranking.clear();
		int indx_begin = rand()%(R.Candidates() - seq_size);
		//cout << endl << "**** rand inedex is " << indx_begin << " **** " << endl;
		random_index.push_back(indx_begin);
		for(int i = 0; i < seq_size; i++)
			partial_order.push_back(temp_2[i+indx_begin]);
		subranking = GetSubRanking(partial_order);
		IloNumArray start_val = OrderToNumArray(env,partial_order);
		vector<int> ls = LocalSearch(subranking, partial_order, start_val, status,p);
		vector<int> v(temp_2.begin(), temp_2.end());
		for(int i = 0; i < seq_size; i++)
			v[i+indx_begin] = ls[i];
		int value = OrderingValue(v);
		//cout << "$$$$$ value = " << value << " and max_value is = " << max_value << endl;
		if(value > max_value){
			nEffectiveMove++;
			size_improvment_vector[seq_size]++;
			for(int i = 0; i < seq_size; i++)
				temp_2[i+indx_begin] = ls[i];	
			//update the histpry vector
			//cout << endl << "******improvmner*****" << endl << endl << endl;
			max_value = value;
			improve_counter =0;
			TS t(value, temp_2);
			if (tabu_counter == tabu_table_cap){
				multiset<TS>::iterator it = tabu_table.begin();
				tabu_table.erase(it);
				tabu_counter--;
			}
			tabu_table.insert(t);
			tabu_counter++;
		}
		else if (value <= max_value){
			improve_counter++;
			//cout << "$$$$the couneter improvement is = " << improve_counter << endl;
		}
		if (improve_counter >  100){
			//	k++;//	p++;
			seq_size += size_increase;
			improve_counter = 0;
		}
		//cout << endl << " The solution after iteration " << iteration << " is "  << value << endl;
		//cout << endl << " The best solution found up to iteration " << iteration << " is "  << max_value << endl;
	}
	//cout << "combine  ordeing value after last step  is : " << OrderingValue(temp_2) << endl;
	vector<vector<int> > mat1 = OrderToMatrix(temp_2);
	int s = 0;
	for(int i = 0; i < R.Candidates(); i++)
		for(int j =0; j < R.Candidates(); j++)
			s+= mat1[i][j]*R.Get_m_Array()[i][j];
	//cout << "the matrix multiply sum is = " << s << endl;
	//	LPHeuristic(lp, temp_2);


	//cout << "combine  ordeing value after last step  is : " << OrderingValue(temp_2) << endl;
	//implement lin-kernighan type local search move
	//for(int i = 0; i < 2000; i++){
	//	vector<int> dd = LinKernighanLocalSearch(temp_2, R.Candidates());
	//	temp_2 = dd;
	//	}

	/*
	//solve by cplex improvement
	IloNumArray start_val = OrderToNumArray(env,init_ordering);
	vector<int> ls = ImproveByCplex(init_ordering, start_val);
	cout << "improveBYcplex is " << OrderingValue(ls) << endl;
	vector<vector<int> > mat = OrderToMatrix(ls);
	int s = 0;
	for(int i = 0; i < R.Candidates(); i++)
	for(int j =0; j < R.Candidates(); j++)
	s+= mat[i][j]*R.Get_m_Array()[i][j];
	cout << "the matrix multiply sum is = " << s << endl;
	*/
	vector<vector<int> > mat = OrderToMatrix(temp_2);
	s = 0;
	for(int i = 0; i < R.Candidates(); i++)
		for(int j =0; j < R.Candidates(); j++)
			s+= mat[i][j]*R.Get_m_Array()[i][j];
	//	cout << "the matrix multiply sum is = " << s << endl;
	//	cout << "init ordering value is : " << OrderingValue(init_ordering) << endl;
	//	cout << "init ordering is : " << endl << init_ordering << endl;
	//	cout << "combine  ordeing value befor the local search was : " << OrderingValue(temp_3) << endl;
	//	cout << "combine  ordeing value after last step  is : " << OrderingValue(temp_2) << endl;
	//	cout << " The final ordeering is : " << endl << temp_2 << endl;
	cout << "The best solution found is : " << max_value << endl;
	//file << random_index << endl;
	//file.close();
	double run_time = difftime(end, start);
	//double gap = ((lp.obj_value - max_value) / max_value) * 100;
	cout << "the running time is : " << run_time << endl; 
	//cout << "the gap is = " << gap << endl; 
	cout << "### The Iterative LP Roundig is ending " << endl;
}
void HeurSolver::CreateModel(IloEnv env,IloModel model, BoolVarMatrix x)
{
	IloExpr obj_fun(env);
	for(int i = 0; i < R.Candidates(); i++)
		for(int j = 0; j < R.Candidates(); j++)
			//	if (i != j)
			obj_fun += (R.Get_m_Array()[i][j] * x[i][j]);
	model.add(IloMaximize(env, obj_fun));
	for (int i = 0; i < R.Candidates(); i++)
		for (int j = 0; j < R.Candidates(); j++)
			if (i != j)
				model.add(x[i][j] + x[j][i] == 1);
			else
				model.add(x[i][i] == 0);
	for (int i = 0; i < R.Candidates(); i++)
		for (int j = 0; j < R.Candidates(); j++)
			for (int k = 0; k < R.Candidates(); k++)
				if ((i != j) && (j != k) && ( k != i))
					model.add(x[i][j] + x[j][k] + x[k][i] <= 2);
}

void HeurSolver::MIP_Heuristic_150(){
	cout << endl << "### The Iterative MIP 150 (with intial solution) is starting....." << endl;
	srand (time(NULL));
	time_t start, end;
	time(&start);
	multiset<TS> tabu_table;
	IloEnv env;
	//	solution lp = SolveLP();
	//	solution initial = CreateInitialSolution(lp);
	//	cout << "The LP solution is :" << lp.obj_value << endl;
	//	cout << "The initial solution value is " << initial.obj_value << endl;
	//vector<int> init_ordering = MatrixToOrdering(lp.sol_vector, R.Candidates());

	vector<int> temp_2 = Preprocessing();
	vector<int> partial_order;	
	vector<vector<int> > subranking ;
	/*for(int i =0; i < 50; i++)
	  partial_order.push_back(init_ordering[i]);
	  vector<vector<int> > subranking = GetSubRanking(partial_order);
	  IloNumArray st = OrderToNumArray(env,partial_order);
	  vector<int> temp = SolveSubProblem(subranking, partial_order,st);

	  partial_order.clear();
	  subranking.clear();
	  for(int i =0; i < 50; i++)
	  partial_order.push_back(init_ordering[i+50]);
	  subranking = GetSubRanking(partial_order);
	  IloNumArray st_1 = OrderToNumArray(env,partial_order);
	  vector<int> temp_1 = SolveSubProblem(subranking, partial_order,st_1);

	  partial_order.clear();
	  subranking.clear();
	  for(int i = 0; i < 50; i++)
	  partial_order.push_back(init_ordering[i+ 100]);
	  subranking = GetSubRanking(partial_order);
	  IloNumArray st_3 = OrderToNumArray(env,partial_order);
	  vector<int> temp_3 = SolveSubProblem(subranking, partial_order,st_3);


	  vector<int> temp_2;
	  temp_2.reserve( temp.size() + temp_1.size()+ temp_3.size()); // preallocate memory
	  temp_2.insert( temp_2.end(), temp.begin(), temp.end() );
	  temp_2.insert( temp_2.end(), temp_1.begin(), temp_1.end() );
	  temp_2.insert( temp_2.end(), temp_3.begin(), temp_3.end() );
	  cout << "combine  ordeing value before is :  " << OrderingValue(temp_2) << endl;
	  */
	//vector<int> temp_2= init_ordering;
	//vector<vector<int> > subranking;
	//local search second phase
	int iteration = 0;								//number of local search iteration
	int k = 3;										// how many varibales are relexaed in subproblem
	int p = 2;										// use for size of the subproblem
	int seq_size= 6 ;								// size of  the subproblem which is p*k
	int max_value = -1;								// the max obj function value found so far
	int improve_counter = 0;						// keep counter to see in how many past iteratio we did not have any improvement
	int tabu_counter = 0;							// keep number of solution currently in tabue table
	int  tabu_table_cap = 10;
	int size_increase = 6;
	solve_status status = fully;
	vector<int> random_index;
	ofstream file("../data/random_index.txt");
	while(iteration++ < 5000)
	{
		time(&end);
		if(difftime(end, start) > 600)
			break;

		if(iteration > 500 && iteration < 1000)
			size_increase = 4;
		else if (iteration > 1000 && iteration < 1500)
			size_increase = 3;
		else 
			size_increase = 2;
		//	seq_size = p*k;
		cout << endl << " and seq_size is : " << seq_size << endl;
		if (seq_size > 56)
			break;
		partial_order.clear();
		subranking.clear();
		int indx_begin = rand()%(R.Candidates() - seq_size);
		random_index.push_back(indx_begin);
		for(int i = 0; i < seq_size; i++)
			partial_order.push_back(temp_2[i+indx_begin]);
		subranking = GetSubRanking(partial_order);
		IloNumArray start_val = OrderToNumArray(env,partial_order);
		vector<int> ls = LocalSearch(subranking, partial_order, start_val, status,p);
		vector<int> v(temp_2.begin(), temp_2.end());
		for(int i = 0; i < seq_size; i++)
			v[i+indx_begin] = ls[i];
		int value = OrderingValue(v);
		cout << "$$$$$ value = " << value << " and max_value is = " << max_value << endl;
		if(value > max_value){
			//update history vector
			for(int i = 0; i < seq_size; i++)
				temp_2[i+indx_begin] = ls[i];
			cout << endl << "******improvmner*****" << endl << endl << endl;
			max_value = value;
			improve_counter =0;
			TS t(value, temp_2);
			if (tabu_counter == tabu_table_cap){
				multiset<TS>::iterator it = tabu_table.begin();
				tabu_table.erase(it);
				tabu_counter--;
			}
			tabu_table.insert(t);
			tabu_counter++;
		}
		else if (value <= max_value){
			improve_counter++;
			cout << "$$$$the couneter improvement is = " << improve_counter << endl;
		}
		if (improve_counter == 100){
			//	k++;
			//	p++;
			seq_size += size_increase;
			improve_counter = 0;
		}
		cout << endl << " The solution after iteration " << iteration << " is "  << value << endl;
		cout << endl << " The best solution found up to iteration " << iteration << " is "  << max_value << endl;
	}
	cout << "combine  ordeing value after last step  is : " << OrderingValue(temp_2) << endl;
	vector<vector<int> > mat1 = OrderToMatrix(temp_2);
	int s = 0;
	for(int i = 0; i < R.Candidates(); i++)
		for(int j =0; j < R.Candidates(); j++)
			s+= mat1[i][j]*R.Get_m_Array()[i][j];
	cout << "the matrix multiply sum is = " << s << endl;
	//	LPHeuristic(lp, temp_2);
	cout << "combine  ordeing value after last step  is : " << OrderingValue(temp_2) << endl;
	/*
	//solve by cplex improvement
	IloNumArray start_val = OrderToNumArray(env,init_ordering);
	vector<int> ls = ImproveByCplex(init_ordering, start_val);
	cout << "improveBYcplex is " << OrderingValue(ls) << endl;
	vector<vector<int> > mat = OrderToMatrix(ls);
	int s = 0;
	for(int i = 0; i < R.Candidates(); i++)
	for(int j =0; j < R.Candidates(); j++)
	s+= mat[i][j]*R.Get_m_Array()[i][j];
	cout << "the matrix multiply sum is = " << s << endl;
	*/
	vector<vector<int> > mat = OrderToMatrix(temp_2);
	s = 0;
	for(int i = 0; i < R.Candidates(); i++)
		for(int j =0; j < R.Candidates(); j++)
			s+= mat[i][j]*R.Get_m_Array()[i][j];
	cout << "the matrix multiply sum is = " << s << endl;
	//	cout << "init ordering value is : " << OrderingValue(init_ordering) << endl;
	//	cout << "init ordering is : " << endl << init_ordering << endl;
	//	cout << "combine  ordeing value befor the local search was : " << OrderingValue(temp_3) << endl;
	cout << "combine  ordeing value after last step  is : " << OrderingValue(temp_2) << endl;
	cout << " The final ordeering is : " << endl << temp_2 << endl;
	cout << "The best solution found is : " << max_value << endl;
	file << random_index << endl;
	file.close();
	//double gap = ((lp.obj_value - max_value) / max_value) * 100;
	//cout << "the gap is = " << gap << endl; 
	double run_time = difftime(end, start);
	cout << "the running time is : " << run_time << endl; 
	cout << "### The Iterative LP Roundig is ending " << endl;
}

void HeurSolver::MIP_Heuristic_200(){
	cout << endl << "### The Iterative MIP 200 (with intial solution) is starting....." << endl;
	srand (time(NULL));
	time_t start, end;
	time(&start);
	multiset<TS> tabu_table;
	IloEnv env;
	//solution lp = SolveLP();
	//solution initial = CreateInitialSolution(lp);
	//cout << "The LP solution is :" << lp.obj_value << endl;
	//cout << "The initial solution value is " << initial.obj_value << endl;
	//vector<int> init_ordering = MatrixToOrdering(lp.sol_vector, R.Candidates());
	//	vector<int> init_ordering(R.Candidates());
	//	for(int i = 0; i < R.Candidates(); i++)
	//		init_ordering[i] = i;

	//	random_shuffle(init_ordering.begin(), init_ordering.end());

	vector<int> partial_order;	
	vector<vector<int> > subranking;
	vector<int> temp_2 = Preprocessing();

	/*for(int i =0; i < 50; i++)
	  partial_order.push_back(init_ordering[i]);
	  vector<vector<int> > subranking = GetSubRanking(partial_order);
	  IloNumArray st = OrderToNumArray(env,partial_order);
	  vector<int> temp = SolveSubProblem(subranking, partial_order,st);

	  partial_order.clear();
	  subranking.clear();
	  for(int i =0; i < 50; i++)
	  partial_order.push_back(init_ordering[i+50]);
	  subranking = GetSubRanking(partial_order);
	  IloNumArray st_1 = OrderToNumArray(env,partial_order);
	  vector<int> temp_1 = SolveSubProblem(subranking, partial_order,st_1);

	  partial_order.clear();
	  subranking.clear();
	  for(int i = 0; i < 50; i++)
	  partial_order.push_back(init_ordering[i+ 100]);
	  subranking = GetSubRanking(partial_order);
	  IloNumArray st_3 = OrderToNumArray(env,partial_order);
	  vector<int> temp_3 = SolveSubProblem(subranking, partial_order,st_3);

	  partial_order.clear();
	  subranking.clear();
	  for(int i = 0; i < 50; i++)
	  partial_order.push_back(init_ordering[i+ 150]);
	  subranking = GetSubRanking(partial_order);
	  IloNumArray st_4 = OrderToNumArray(env,partial_order);
	  vector<int> temp_4 = SolveSubProblem(subranking, partial_order,st_4);


	  vector<int> temp_2;
	  temp_2.reserve( temp.size() + temp_1.size()+ temp_3.size() + temp_4.size() ); // preallocate memory
	  temp_2.insert( temp_2.end(), temp.begin(), temp.end() );
	  temp_2.insert( temp_2.end(), temp_1.begin(), temp_1.end() );
	  temp_2.insert( temp_2.end(), temp_3.begin(), temp_3.end() );
	  temp_2.insert( temp_2.end(), temp_4.begin(), temp_4.end() );
	  cout << "combine  ordeing value before is :  " << OrderingValue(temp_2) << endl;
	  */
	//vector<int> temp_2= init_ordering;
	//vector<vector<int> > subranking;
	//local search second phase
	int iteration = 0;								//number of local search iteration
	int k = 3;										// how many varibales are relexaed in subproblem
	int p = 2;										// use for size of the subproblem
	int seq_size= 20 ;								// size of  the subproblem which is p*k
	int max_value = -1;								// the max obj function value found so far
	int improve_counter = 0;						// keep counter to see in how many past iteratio we did not have any improvement
	int tabu_counter = 0;							// keep number of solution currently in tabue table
	int  tabu_table_cap = 10;
	int size_increase = 6;
	solve_status status = fully;
	vector<int> random_index;
	//ofstream file("../data/random_index.txt");
	while(iteration++ < 50000)
	{
		time(&end);
		if(difftime(end, start) > 400)
			break;

		if(iteration > 500 && iteration < 1000)
			size_increase = 4;
		else if (iteration > 1000 && iteration < 1500)
			size_increase = 3;
		else 
			size_increase = 2;
		//	seq_size = p*k;
		//cout << endl << " and seq_size is : " << seq_size << endl;
		if (seq_size > 56)
			break;
		partial_order.clear();
		subranking.clear();
		int indx_begin = rand()%(R.Candidates() - seq_size);
		random_index.push_back(indx_begin);
		for(int i = 0; i < seq_size; i++)
			partial_order.push_back(temp_2[i+indx_begin]);
		subranking = GetSubRanking(partial_order);
		IloNumArray start_val = OrderToNumArray(env,partial_order);
		vector<int> ls = LocalSearch(subranking, partial_order, start_val, status,p);
		vector<int> v(temp_2.begin(), temp_2.end());
		for(int i = 0; i < seq_size; i++)
			v[i+indx_begin] = ls[i];
		int value = OrderingValue(v);
		//cout << "$$$$$ value = " << value << " and max_value is = " << max_value << endl;
		if(value > max_value){
			for(int i = 0; i < seq_size; i++)
				temp_2[i+indx_begin] = ls[i];
			//	cout << endl << "******improvmner*****" << endl << endl << endl;
			max_value = value;
			improve_counter =0;
			TS t(value, temp_2);
			if (tabu_counter == tabu_table_cap){
				multiset<TS>::iterator it = tabu_table.begin();
				tabu_table.erase(it);
				tabu_counter--;
			}
			tabu_table.insert(t);
			tabu_counter++;
		}
		else if (value <= max_value){
			improve_counter++;
			//	cout << "$$$$the couneter improvement is = " << improve_counter << endl;
		}
		if (improve_counter == 100){
			//	k++;
			//	p++;
			seq_size += size_increase;
			improve_counter = 0;
		}
		//cout << endl << " The solution after iteration " << iteration << " is "  << value << endl;
		//cout << endl << " The best solution found up to iteration " << iteration << " is "  << max_value << endl;
	}
	//cout << "combine  ordeing value after last step  is : " << OrderingValue(temp_2) << endl;
	vector<vector<int> > mat1 = OrderToMatrix(temp_2);
	int s = 0;
	for(int i = 0; i < R.Candidates(); i++)
		for(int j =0; j < R.Candidates(); j++)
			s+= mat1[i][j]*R.Get_m_Array()[i][j];
	//cout << "the matrix multiply sum is = " << s << endl;
	//	LPHeuristic(lp, temp_2);
	//cout << "combine  ordeing value after last step  is : " << OrderingValue(temp_2) << endl;
	/*
	//solve by cplex improvement
	IloNumArray start_val = OrderToNumArray(env,init_ordering);
	vector<int> ls = ImproveByCplex(init_ordering, start_val);
	cout << "improveBYcplex is " << OrderingValue(ls) << endl;
	vector<vector<int> > mat = OrderToMatrix(ls);
	int s = 0;
	for(int i = 0; i < R.Candidates(); i++)
	for(int j =0; j < R.Candidates(); j++)
	s+= mat[i][j]*R.Get_m_Array()[i][j];
	cout << "the matrix multiply sum is = " << s << endl;
	*/
	vector<vector<int> > mat = OrderToMatrix(temp_2);
	s = 0;
	for(int i = 0; i < R.Candidates(); i++)
		for(int j =0; j < R.Candidates(); j++)
			s+= mat[i][j]*R.Get_m_Array()[i][j];
	//cout << "the matrix multiply sum is = " << s << endl;
	//cout << "init ordering value is : " << OrderingValue(init_ordering) << endl;
	//cout << "init ordering is : " << endl << init_ordering << endl;
	//cout << "combine  ordeing value befor the local search was : " << OrderingValue(temp_3) << endl;
	//cout << "combine  ordeing value after last step  is : " << OrderingValue(temp_2) << endl;
	//cout << " The final ordeering is : " << endl << temp_2 << endl;
	cout << "The best solution found is : " << max_value << endl;
	//file << random_index << endl;
	//file.close();
	//double gap = ((lp.obj_value - max_value) / max_value) * 100;
	//cout << "the gap is = " << gap << endl; 
	double run_time = difftime(end, start);
	cout << "the running time is : " << run_time << endl; 
	//cout << "### The Iterative LP Roundig is ending " << endl;
}

/*
   void HeurSolver::MIP_Heuristic_200(){
   cout << endl << "### The Iterative MIP 200 (with intial solution) is starting....." << endl;
   srand (time(NULL));
   time_t start, end;
   time(&start);
   multiset<TS> tabu_table;
   IloEnv env;
   solution lp = SolveLP();
   solution initial = CreateInitialSolution(lp);
   cout << "The LP solution is :" << lp.obj_value << endl;
   cout << "The initial solution value is " << initial.obj_value << endl;
//vector<int> init_ordering = MatrixToOrdering(lp.sol_vector, R.Candidates());

for(int i =0; i < 50; i++)
partial_order.push_back(init_ordering[i]);
vector<vector<int> > subranking = GetSubRanking(partial_order);
IloNumArray st = OrderToNumArray(env,partial_order);
vector<int> temp = SolveSubProblem(subranking, partial_order,st);

partial_order.clear();
subranking.clear();
for(int i =0; i < 50; i++)
partial_order.push_back(init_ordering[i+50]);
subranking = GetSubRanking(partial_order);
IloNumArray st_1 = OrderToNumArray(env,partial_order);
vector<int> temp_1 = SolveSubProblem(subranking, partial_order,st_1);

partial_order.clear();
subranking.clear();
for(int i = 0; i < 50; i++)
partial_order.push_back(init_ordering[i+ 100]);
subranking = GetSubRanking(partial_order);
IloNumArray st_3 = OrderToNumArray(env,partial_order);
vector<int> temp_3 = SolveSubProblem(subranking, partial_order,st_3);

partial_order.clear();
subranking.clear();
for(int i = 0; i < 50; i++)
partial_order.push_back(init_ordering[i+ 150]);
subranking = GetSubRanking(partial_order);
IloNumArray st_4 = OrderToNumArray(env,partial_order);
vector<int> temp_4 = SolveSubProblem(subranking, partial_order,st_4);


vector<int> temp_2;
temp_2.reserve( temp.size() + temp_1.size()+ temp_3.size() + temp_4.size() ); // preallocate memory
temp_2.insert( temp_2.end(), temp.begin(), temp.end() );
temp_2.insert( temp_2.end(), temp_1.begin(), temp_1.end() );
temp_2.insert( temp_2.end(), temp_3.begin(), temp_3.end() );
temp_2.insert( temp_2.end(), temp_4.begin(), temp_4.end() );
cout << "combine  ordeing value before is :  " << OrderingValue(temp_2) << endl;


//vector<int> temp_2= init_ordering;
//vector<vector<int> > subranking;
//local search second phase
int iteration = 0;								//number of local search iteration
int k = 3;										// how many varibales are relexaed in subproblem
int p = 2;										// use for size of the subproblem
int seq_size= 6 ;								// size of  the subproblem which is p*k
int max_value = -1;								// the max obj function value found so far
int improve_counter = 0;						// keep counter to see in how many past iteratio we did not have any improvement
int tabu_counter = 0;							// keep number of solution currently in tabue table
int  tabu_table_cap = 10;
int size_increase = 6;
solve_status status = fully;
vector<int> random_index;
ofstream file("../data/random_index.txt");
while(iteration++ < 5000)
{
if(iteration > 500 && iteration < 1000)
	size_increase = 4;
else if (iteration > 1000 && iteration < 1500)
	size_increase = 3;
	else 
	size_increase = 2;
	//	seq_size = p*k;
	cout << endl << " and seq_size is : " << seq_size << endl;
if (seq_size > 56)
	break;
	partial_order.clear();
	subranking.clear();
	int indx_begin = rand()%(R.Candidates() - seq_size);
	random_index.push_back(indx_begin);
	for(int i = 0; i < seq_size; i++)
	partial_order.push_back(temp_2[i+indx_begin]);
	subranking = GetSubRanking(partial_order);
	IloNumArray start_val = OrderToNumArray(env,partial_order);
	vector<int> ls = LocalSearch(subranking, partial_order, start_val, status,p);
	vector<int> v(temp_2.begin(), temp_2.end());
	for(int i = 0; i < seq_size; i++)
	v[i+indx_begin] = ls[i];
	int value = OrderingValue(v);
	cout << "$$$$$ value = " << value << " and max_value is = " << max_value << endl;
	if(value > max_value){
		for(int i = 0; i < seq_size; i++)
			temp_2[i+indx_begin] = ls[i];
		cout << endl << "******improvmner*****" << endl << endl << endl;
		max_value = value;
		improve_counter =0;
		TS t(value, temp_2);
		if (tabu_counter == tabu_table_cap){
			multiset<TS>::iterator it = tabu_table.begin();
			tabu_table.erase(it);
			tabu_counter--;
		}
		tabu_table.insert(t);
		tabu_counter++;
	}
else if (value <= max_value){
	improve_counter++;
	cout << "$$$$the couneter improvement is = " << improve_counter << endl;
}
if (improve_counter == 100){
	//	k++;
	//	p++;
	seq_size += size_increase;
	improve_counter = 0;
}
cout << endl << " The solution after iteration " << iteration << " is "  << value << endl;
cout << endl << " The best solution found up to iteration " << iteration << " is "  << max_value << endl;
}
cout << "combine  ordeing value after last step  is : " << OrderingValue(temp_2) << endl;
vector<vector<int> > mat1 = OrderToMatrix(temp_2);
int s = 0;
for(int i = 0; i < R.Candidates(); i++)
for(int j =0; j < R.Candidates(); j++)
s+= mat1[i][j]*R.Get_m_Array()[i][j];
cout << "the matrix multiply sum is = " << s << endl;
//	LPHeuristic(lp, temp_2);
cout << "combine  ordeing value after last step  is : " << OrderingValue(temp_2) << endl;

//solve by cplex improvement
IloNumArray start_val = OrderToNumArray(env,init_ordering);
vector<int> ls = ImproveByCplex(init_ordering, start_val);
cout << "improveBYcplex is " << OrderingValue(ls) << endl;
vector<vector<int> > mat = OrderToMatrix(ls);
int s = 0;
for(int i = 0; i < R.Candidates(); i++)
for(int j =0; j < R.Candidates(); j++)
s+= mat[i][j]*R.Get_m_Array()[i][j];
cout << "the matrix multiply sum is = " << s << endl;
*/
/*vector<vector<int> > mat = OrderToMatrix(temp_2);
  s = 0;
  for(int i = 0; i < R.Candidates(); i++)
  for(int j =0; j < R.Candidates(); j++)
  s+= mat[i][j]*R.Get_m_Array()[i][j];
  cout << "the matrix multiply sum is = " << s << endl;
  cout << "init ordering value is : " << OrderingValue(init_ordering) << endl;
  cout << "init ordering is : " << endl << init_ordering << endl;
  cout << "combine  ordeing value befor the local search was : " << OrderingValue(temp_3) << endl;
  cout << "combine  ordeing value after last step  is : " << OrderingValue(temp_2) << endl;
  cout << " The final ordeering is : " << endl << temp_2 << endl;
  cout << "The best solution found is : " << max_value << endl;
  file << random_index << endl;
  file.close();
  time(&end);
  double run_time = difftime(end, start);
  cout << "the running time is : " << run_time << endl; 
  cout << "### The Iterative LP Roundig is ending " << endl;
  }
  */
/*
   void HeurSolver::MIP_Heuristic_500(){
   cout << endl << "### The Iterative MIP (with intial solution) is starting....." << endl;
   srand (time(NULL));
   time_t start, end;
   time(&start);
   multiset<TS> tabu_table;
   IloEnv env;
   solution lp = SolveLP();
   solution initial = CreateInitialSolution(lp);
   cout << "The LP solution is :" << lp.obj_value << endl;
   cout << "The initial solution value is " << initial.obj_value << endl;
   vector<int> init_ordering = MatrixToOrdering(lp.sol_vector, R.Candidates());

   vector<int> partial_order;	
   for(int i =0; i < 50; i++)
   partial_order.push_back(init_ordering[i]);
   vector<vector<int> > subranking = GetSubRanking(partial_order);
   IloNumArray st = OrderToNumArray(env,partial_order);
   vector<int> temp = SolveSubProblem(subranking, partial_order,st);

   partial_order.clear();
   subranking.clear();
   for(int i =0; i < 50; i++)
   partial_order.push_back(init_ordering[i+50]);
   subranking = GetSubRanking(partial_order);
   IloNumArray st_1 = OrderToNumArray(env,partial_order);
   vector<int> temp_1 = SolveSubProblem(subranking, partial_order,st_1);

   vector<int> temp_2;
   temp_2.reserve( temp.size() + temp_1.size()); // preallocate memory
   temp_2.insert( temp_2.end(), temp.begin(), temp.end() );
   temp_2.insert( temp_2.end(), temp_1.begin(), temp_1.end() );
   cout << "combine  ordeing value before is :  " << OrderingValue(temp_2) << endl;

   partial_order.clear();
   subranking.clear();
   for(int i = 20; i < 80; i++)
   partial_order.push_back(temp_2[i]);
   subranking = GetSubRanking(partial_order);
   IloNumArray st_3 = OrderToNumArray(env,partial_order);
   vector<int> temp_3 = SolveSubProblem(subranking, partial_order,st_3);
   for(int i = 0; i < 60; i++)
   temp_2[i+20] = temp_3[i];
   temp_3.clear();
   temp_3 = temp_2;

//vector<int> temp_2= init_ordering;
//vector<vector<int> > subranking;
//local search second phase
int iteration = 0;								//number of local search iteration
int k = 3;										// how many varibales are relexaed in subproblem
int p = 2;										// use for size of the subproblem
int seq_size= 6 ;								// size of  the subproblem which is p*k
int max_value = -1;								// the max obj function value found so far
int improve_counter = 0;						// keep counter to see in how many past iteratio we did not have any improvement
int tabu_counter = 0;							// keep number of solution currently in tabue table
int  tabu_table_cap = 10;
int size_increase = 6;
solve_status status = fully;
vector<int> random_index;
ofstream file("../data/random_index.txt");
while(iteration++ < 5000)
{
if(iteration > 500 && iteration < 1000)
size_increase = 4;
else if (iteration > 1000 && iteration < 1500)
size_increase = 3;
else 
size_increase = 2;
//	seq_size = p*k;
cout << endl << " and seq_size is : " << seq_size << endl;
if (seq_size > 56)
	break;
	partial_order.clear();
	subranking.clear();
	int indx_begin = rand()%(R.Candidates() - seq_size);
	random_index.push_back(indx_begin);
	for(int i = 0; i < seq_size; i++)
	partial_order.push_back(temp_2[i+indx_begin]);
	subranking = GetSubRanking(partial_order);
	IloNumArray start_val = OrderToNumArray(env,partial_order);
	vector<int> ls = LocalSearch(subranking, partial_order, start_val, status,p);
	vector<int> v(temp_2.begin(), temp_2.end());
	for(int i = 0; i < seq_size; i++)
	v[i+indx_begin] = ls[i];
	int value = OrderingValue(v);
	cout << "$$$$$ value = " << value << " and max_value is = " << max_value << endl;
	if(value > max_value){
		for(int i = 0; i < seq_size; i++)
			temp_2[i+indx_begin] = ls[i];
		cout << endl << "******improvmner*****" << endl << endl << endl;
		max_value = value;
		improve_counter =0;
		TS t(value, temp_2);
		if (tabu_counter == tabu_table_cap){
			multiset<TS>::iterator it = tabu_table.begin();
			tabu_table.erase(it);
			tabu_counter--;
		}
		tabu_table.insert(t);
		tabu_counter++;
	}
else if (value <= max_value){
	improve_counter++;
	cout << "$$$$the couneter improvement is = " << improve_counter << endl;
}
if (improve_counter == 100){
	//	k++;
	//	p++;
	seq_size += size_increase;
	improve_counter = 0;
}
cout << endl << " The solution after iteration " << iteration << " is "  << value << endl;
cout << endl << " The best solution found up to iteration " << iteration << " is "  << max_value << endl;
}
cout << "combine  ordeing value after last step  is : " << OrderingValue(temp_2) << endl;
vector<vector<int> > mat1 = OrderToMatrix(temp_2);
int s = 0;
for(int i = 0; i < R.Candidates(); i++)
for(int j =0; j < R.Candidates(); j++)
s+= mat1[i][j]*R.Get_m_Array()[i][j];
cout << "the matrix multiply sum is = " << s << endl;
//	LPHeuristic(lp, temp_2);
cout << "combine  ordeing value after last step  is : " << OrderingValue(temp_2) << endl;

//solve by cplex improvement
IloNumArray start_val = OrderToNumArray(env,init_ordering);
vector<int> ls = ImproveByCplex(init_ordering, start_val);
cout << "improveBYcplex is " << OrderingValue(ls) << endl;
vector<vector<int> > mat = OrderToMatrix(ls);
int s = 0;
for(int i = 0; i < R.Candidates(); i++)
for(int j =0; j < R.Candidates(); j++)
s+= mat[i][j]*R.Get_m_Array()[i][j];
cout << "the matrix multiply sum is = " << s << endl;

vector<vector<int> > mat = OrderToMatrix(temp_2);
s = 0;
for(int i = 0; i < R.Candidates(); i++)
for(int j =0; j < R.Candidates(); j++)
s+= mat[i][j]*R.Get_m_Array()[i][j];
cout << "the matrix multiply sum is = " << s << endl;
cout << "init ordering value is : " << OrderingValue(init_ordering) << endl;
cout << "init ordering is : " << endl << init_ordering << endl;
cout << "combine  ordeing value befor the local search was : " << OrderingValue(temp_3) << endl;
cout << "combine  ordeing value after last step  is : " << OrderingValue(temp_2) << endl;
cout << " The final ordeering is : " << endl << temp_2 << endl;
cout << "The best solution found is : " << max_value << endl;
file << random_index << endl;
file.close();
time(&end);
double run_time = difftime(end, start);
cout << "the running time is : " << run_time << endl; 
cout << "### The Iterative LP Roundig is ending " << endl;
}
*/


void HeurSolver::CreateModel(IloEnv env,IloModel model, NumVarMatrix x)
{
	IloExpr obj_fun(env);
	for(int i = 0; i < R.Candidates(); i++)
		for(int j = 0; j < R.Candidates(); j++)
			//if (i != j)
			obj_fun += (R.Get_m_Array()[i][j] * x[i][j]);
	model.add(IloMaximize(env, obj_fun));
	for (int i = 0; i < R.Candidates(); i++)
		for (int j = 0; j < R.Candidates(); j++)
			if (i != j)
				model.add(x[i][j] + x[j][i] == 1);
			else
				model.add(x[i][i] == 0);
	for (int i = 0; i < R.Candidates(); i++)
		for (int j = 0; j < R.Candidates(); j++)
			for (int k = 0; k < R.Candidates(); k++)
				if ((i != j) && (j != k) && ( k != i))
					model.add(x[i][j] + x[j][k] + x[k][i] <= 2);
}

vector<int> HeurSolver::LocalSearch(vector<vector<int> > matrix, vector<int> cand_subset, IloNumArray start_val, solve_status status, int p)
{
	int size = cand_subset.size(); 
	//cout << "*** Ordeing value in local search is :" << OrderingValue(cand_subset) << " *** " << endl;
	vector<int> vec(cand_subset.begin(),cand_subset.end());
	//create and solve the model and do the local search
	try
	{
		IloEnv enviroment;
		IloModel model(enviroment);
		IloCplex cplex(model);
		IloNumVarArray start_var(enviroment);
		BoolVarMatrix x(enviroment,size);
		for(int i = 0; i < size; i++)
			x[i] = IloBoolVarArray(enviroment, size);
		for(int i = 0; i < size; i++)
			for(int j = 0; j < size; j++)
				start_var.add(x[i][j]);
		IloExpr obj_fun(enviroment);
		for(int i = 0; i < size; i++)
			for(int j = 0; j < size; j++)
				//if (i != j)
				obj_fun += (matrix[i][j] * x[i][j]);
		model.add(IloMaximize(enviroment, obj_fun));
		//add simple constraints 
		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				if (i != j)
					model.add(x[i][j] + x[j][i] == 1);
				else
					model.add(x[i][i] == 0);
		// add 3-dicycles constraints
		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				for (int k = 0; k < size; k++)
					if ((i != j) && (j != k) && ( k != i))
						model.add(x[i][j] + x[j][k] + x[k][i] <= 2);

		//fix variables
		if (status == restricted){
			int counter = 0;
			vector<int> indx;
			while (counter < size){
				indx.push_back(counter);
				counter+=p;
			}
			for(int i = 0; i < size; i++)
				if (find(indx.begin(), indx.end(), i) == indx.end())
					for(int j = i+1 ; j < size ; j++)
						if(find(indx.begin(), indx.end(), j) == indx.end())
						{
							model.add(x[i][j] == 1);
							model.add(x[j][i] == 0);
						}
		}
		//if (size > 40)
		//	cplex.setParam(IloCplex::EpGap, 0.1);
		cplex.addMIPStart(start_var, start_val, IloCplex::MIPStartAuto,  "secondMIPStart");
		cplex.setOut(enviroment.getNullStream());
		time_t start, end;
		time(&start);
		//cplex.setParam(IloCplex::TiLim, 200);
		if ( !cplex.solve() ) {
			enviroment.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}
		time(&end);
		//double run_time = difftime(end, start);
		//cout << "The subproblem running time is : " << run_time << endl;
		// prepare the output ranking	
		vector<vector<int> > sol;
		sol.resize(size);
		for(int i = 0; i < size; i++)
			sol[i].resize(size);
		//put the output of cplex to 2D vector
		for(int i = 0; i < size ; i ++)
			for(int j = 0; j < size ; j++)
				if (cplex.getValue(x[i][j] == 1))
					sol[i][j] = 1;
				else 
					sol[i][j] = 0;
		//change  the 2D vector solution to ordering
		vector<int> temp = MatrixToOrdering(sol, size);
		for(int i =0; i < size ; i++)
			vec[i] = cand_subset[temp[i]];
		//cout << "********lollaala********" << endl;
		enviroment.end();
	}
	catch ( IloException & e){
		cerr << "Concert exception caught in Local search: " << e << endl;
	} catch (...){
		cerr << "Unknown exception caught in local search" << endl;
	}
	return vec;
}


vector<int> HeurSolver:: SolveSubProblem(vector<vector<int> > matrix, vector<int> cand_subset, IloNumArray st){
	static int counter  = 0;
	int dim = cand_subset.size();
	vector<int> ordering;
	ordering.resize(dim);
	try{
		//cerate the models and variables
		IloEnv enviroment;
		IloModel model(enviroment);
		IloCplex cplex(model);
		IloNumVarArray st_var(enviroment);
		BoolVarMatrix x(enviroment, dim);
		for(int i = 0; i < dim ; i++)
			x[i] = IloBoolVarArray(enviroment, dim);
		for(int i = 0; i < dim; i++)
			for(int j = 0; j < dim; j++)
				st_var.add(x[i][j]);
		//create and add the objective function
		IloExpr obj_fun(enviroment);
		for(int i = 0; i < dim; i++)
			for(int j = 0; j < dim; j++)
				//if (i != j)
				obj_fun += (matrix[i][j] * x[i][j]);
		model.add(IloMaximize(enviroment, obj_fun));
		//add simple constraints 
		for (int i = 0; i < dim; i++)
			for (int j = 0; j < dim; j++)
				if (i != j)
					model.add(x[i][j] + x[j][i] == 1);
				else
					model.add(x[i][i] == 0);
		// add 3-dicycles constraints
		for (int i = 0; i < dim; i++)
			for (int j = 0; j < dim; j++)
				for (int k = 0; k < dim; k++)
					if ((i != j) && (j != k) && ( k != i))
						model.add(x[i][j] + x[j][k] + x[k][i] <= 2);
		if (++counter ==3){
			cplex.setParam(IloCplex::EpGap, 0.15);
			//cout << endl << "set the gap to 0.07" << endl;
		}
		else if (counter)
			cplex.setParam(IloCplex::EpGap, 0.5);
		cplex.setOut(enviroment.getNullStream());
		time_t start, end;
		time(&start);
		//cplex.setParam(IloCplex::TiLim, 200);
		//	cplex.addMIPStart(st_var, st, IloCplex::MIPStartAuto,  "secondMIPStart");
		if ( !cplex.solve() ) {
			enviroment.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}
		time(&end);
		double run_time = difftime(end, start);
		//cout << "The subproblem running time is : " << run_time << endl;
		// prepare the output ranking	
		vector<vector<int> > sol;
		sol.resize(dim);
		for(int i = 0; i < dim; i++)
			sol[i].resize(dim);
		//put the output of cplex to 2D vector
		for(int i = 0; i < dim ; i ++)
			for(int j = 0; j < dim ; j++)
				if (cplex.getValue(x[i][j] == 1))
					sol[i][j] = 1;
				else 
					sol[i][j] = 0;
		//change  the 2D vector solution to ordering
		vector<int> temp = MatrixToOrdering(sol, dim);
		for(int i =0; i < dim ; i++)
			ordering[i] = cand_subset[temp[i]];
		//cout << endl << "order before " << cand_subset << endl;
		//cout << "ordeing value before is " << OrderingValue(cand_subset) << endl ;
		//cout << endl << "order after " << ordering << endl;
		//cout << "ordering value after " << OrderingValue(ordering) << endl;
		//cout << ordering << endl;
		enviroment.end();
	}
	catch ( IloException & e){
		cerr << "Concert exception caught: " << e << endl;
	} catch (...)	{
		cerr << "Unknown exception caught" << endl;
	}
	return ordering;
}

vector<int> HeurSolver::LocalSearch( vector<int> cand_subset, IloNumArray start_val)
{
	int size = cand_subset.size();;
	vector<int> vec(size);
	try
	{
		IloEnv enviroment;
		IloModel model(enviroment);
		IloCplex cplex(model);
		IloNumVarArray start_var(enviroment);
		BoolVarMatrix x(enviroment,size);
		for(int i = 0; i < size; i++)
			x[i] = IloBoolVarArray(enviroment, size);
		for(int i = 0; i < size; i++)
			for(int j = 0; j < size; j++)
				start_var.add(x[i][j]);
		IloExpr obj_fun(enviroment);
		for(int i = 0; i < size; i++)
			for(int j = 0; j < size; j++)
				//if (i != j)
				obj_fun += (R.Get_m_Array()[i][j]  * x[i][j]);
		model.add(IloMaximize(enviroment, obj_fun));
		//add simple constraints 
		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				if (i != j)
					model.add(x[i][j] + x[j][i] == 1);
				else
					model.add(x[i][i] == 0);
		// add 3-dicycles constraints
		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				for (int k = 0; k < size; k++)
					if ((i != j) && (j != k) && ( k != i))
						model.add(x[i][j] + x[j][k] + x[k][i] <= 2);
		int relax_candid = rand() % (size/2);

		vector<int>::iterator it;
		for(int i = 0; i < size; i++)
			if (i != relax_candid)
				for(int j = i+1 ; j < size ; j++)
					if(j != relax_candid)
					{
						model.add(x[i][j] == 1);
						model.add(x[j][i] == 0);
					}
		//if(size > 40)
		//	cplex.setParam(IloCplex::EpGap, 0.1);
		cplex.addMIPStart(start_var, start_val, IloCplex::MIPStartAuto,  "secondMIPStart");
		cplex.setOut(enviroment.getNullStream());
		time_t start, end;
		time(&start);
		//cplex.setParam(IloCplex::TiLim, 200);
		if ( !cplex.solve() ) {
			enviroment.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}
		time(&end);
		double run_time = difftime(end, start);
		cout << "The subproblem running time is : " << run_time << endl;
		// prepare the output ranking	
		vector<vector<int> > sol;
		sol.resize(size);
		for(int i = 0; i < size; i++)
			sol[i].resize(size);
		//put the output of cplex to 2D vector
		for(int i = 0; i < size ; i ++)
			for(int j = 0; j < size ; j++)
				if (cplex.getValue(x[i][j] == 1))
					sol[i][j] = 1;
				else 
					sol[i][j] = 0;
		//change  the 2D vector solution to ordering
		vector<int> temp = MatrixToOrdering(sol, size);
		for(int i =0; i < size ; i++)
		{
			vec[i] = cand_subset[temp[i]];
		}
		enviroment.end();
	}
	catch ( IloException & e){
		cerr << "Concert exception caught in Local search: " << e << endl;
	} catch (...){
		cerr << "Unknown exception caught in local search" << endl;
	}
	return vec;
}

vector<int> HeurSolver::ImproveByCplex( vector<int> cand_subset, IloNumArray start_val)
{
	int size = cand_subset.size();;
	vector<int> vec(size);
	try
	{
		IloEnv enviroment;
		IloModel model(enviroment);
		IloCplex cplex(model);
		IloNumVarArray start_var(enviroment);
		BoolVarMatrix x(enviroment,size);
		for(int i = 0; i < size; i++)
			x[i] = IloBoolVarArray(enviroment, size);
		for(int i = 0; i < size; i++)
			for(int j = 0; j < size; j++)
				start_var.add(x[i][j]);
		IloExpr obj_fun(enviroment);
		for(int i = 0; i < size; i++)
			for(int j = 0; j < size; j++)
				//if (i != j)
				obj_fun += (R.Get_m_Array()[i][j]  * x[i][j]);
		model.add(IloMaximize(enviroment, obj_fun));
		//add simple constraints 
		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				if (i != j)
					model.add(x[i][j] + x[j][i] == 1);
				else
					model.add(x[i][i] == 0);
		// add 3-dicycles constraints
		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				for (int k = 0; k < size; k++)
					if ((i != j) && (j != k) && ( k != i))
						model.add(x[i][j] + x[j][k] + x[k][i] <= 2);
		//cplex.setParam(IloCplex::EpGap, 0.5);
		//cplex.setParam(IloCplex::PolishAfterTime,0);
		cplex.setParam(IloCplex::PolishTime, 100);
		cplex.setParam(IloCplex::TiLim, 100);
		cplex.setOut(enviroment.getNullStream());

		cplex.addMIPStart(start_var, start_val, IloCplex::MIPStartAuto,  "secondMIPStart");
		time_t start, end;
		time(&start);
		//cplex.setParam(IloCplex::TiLim, 200);
		if ( !cplex.solve() ) {
			enviroment.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}
		time(&end);
		double run_time = difftime(end, start);
		cout << "The subproblem running time is : " << run_time << endl;
		// prepare the output ranking	
		vector<vector<int> > sol;
		sol.resize(size);
		for(int i = 0; i < size; i++)
			sol[i].resize(size);
		//put the output of cplex to 2D vector
		for(int i = 0; i < size ; i ++)
			for(int j = 0; j < size ; j++)
				if (cplex.getValue(x[i][j] == 1))
					sol[i][j] = 1;
				else 
					sol[i][j] = 0;
		//change  the 2D vector solution to ordering
		vector<int> temp = MatrixToOrdering(sol, size);
		for(int i =0; i < size ; i++)
		{
			vec[i] = cand_subset[temp[i]];
		}
		enviroment.end();
	}
	catch ( IloException & e){
		cerr << "Concert exception caught in Local search: " << e << endl;
	} catch (...){
		cerr << "Unknown exception caught in local search" << endl;
	}
	return vec;
}

void HeurSolver::LPHeuristic(solution& lp, vector<int>& temp_2){
	cout << endl << "Lp Heur is starting" << endl;
	vector<int> scores(R.Candidates());
	vector<vector<double> > diff_score_matrix(R.Candidates());
	for(int i = 0; i < R.Candidates(); i++ )
		diff_score_matrix[i].resize(R.Candidates());	
	//initiate diff matrix to 0
	for(int i = 0; i < R.Candidates(); i++ )
		for(int j = 0; j < R.Candidates(); j++)
			diff_score_matrix[i][j] = 0;
	vector<int> current_solution = temp_2;
	int counter = 0;
	int threshold = 1;
	double pre_iterat_solution =0;
	while (counter++ < 3){
		for(int i = 0; i < R.Candidates(); i++ )
			for(int j = i+1; j < R.Candidates(); j++)
				diff_score_matrix[current_solution[i]][current_solution[j]] = scores[i] - scores[j];
		solution temp = FixnSolve(diff_score_matrix, current_solution,R.Candidates(), threshold);
		if (pre_iterat_solution == temp.obj_value)
			threshold+= 2;
		else 
			pre_iterat_solution = temp.obj_value;
		current_solution = MatrixToOrdering(temp.sol_vector, R.Candidates());
		cout << "iteration " << counter << " and solution is : " << temp.obj_value << endl;

	}
	temp_2 = current_solution;
}


void HeurSolver::LPHeuristic(){
	cout << endl << "### The LP Based Heuristic is running....." << endl;
	time_t start, end;
	time(&start);
	solution lp = SolveLP();
	//solution initial = CreateInitialSolution(lp);
	//solution ip = SolveILP();
	vector<int> scores(R.Candidates());
	vector<int> init_ordering = MatrixToOrdering(lp.sol_vector,scores, R.Candidates());
	cout << "init is  :  " << endl << init_ordering << endl;
	cout << "score is :  " << endl << scores << endl;
	cout << "The LP solution is :" << lp.obj_value << endl;
	//cout << "The initial solution value is : " << initial.obj_value << endl;
	cout << "The LP heuristic obj value is : " << OrderingValue(init_ordering) << endl;
	//cout << "The IP solution value is      : " << ip.obj_value << endl;
	vector<vector<double> > diff_score_matrix(R.Candidates());
	for(int i = 0; i < R.Candidates(); i++ )
		diff_score_matrix[i].resize(R.Candidates());	
	//initiate diff matrix to 0
	for(int i = 0; i < R.Candidates(); i++ )
		for(int j = 0; j < R.Candidates(); j++)
			diff_score_matrix[i][j] = 0;
	vector<int> current_solution = init_ordering;

	cout << "diff matrrix " << endl << diff_score_matrix << endl;	
	int counter = 0;
	int threshold = 1;
	double pre_iterat_solution =0;
	while (counter++ < 100){
		for(int i = 0; i < R.Candidates(); i++ )
			for(int j = i+1; j < R.Candidates(); j++)
				diff_score_matrix[current_solution[i]][current_solution[j]] = scores[i] - scores[j];
		solution temp = FixnSolve(diff_score_matrix, current_solution,R.Candidates(), threshold);
		if (pre_iterat_solution == temp.obj_value)
			threshold+= 2;
		else 
			pre_iterat_solution = temp.obj_value;
		current_solution = MatrixToOrdering(temp.sol_vector, R.Candidates());
		cout << "iteration " << counter << " and solution is : " << temp.obj_value << endl;

	}
}

solution HeurSolver::FixnSolve(vector<vector<double> >& diff_score_matrix, vector<int>& order, int size, int threshold)
{
	solution s(size);
	cout << "*** orderinf value is  : " << OrderingValue(order) << endl;
	vector<vector<int> > matrix = OrderToMatrix(order);
	try {
		IloEnv env;
		//create the model and variables for heuristic
		IloModel model(env);
		IloCplex cplex(model);
		BoolVarMatrix x(env, size);
		for(int i =0; i < size; i++)
			x[i] = IloBoolVarArray(env, size);
		IloNumVarArray start_var(env);
		IloNumArray start_val(env);
		for(int i = 0; i < size; i++)
			for(int j = 0; j < size; j++){
				start_var.add(x[i][j]);
				start_val.add(matrix[i][j]);
			}
		//IloNumArray start_val = OrderToNumArray(env,order);

		//set the parameters for the model 
		//cplex.setParam(IloCplex::RootAlg, IloCplex::Primal);
		//create and add the objective function
		IloExpr obj_fun(env);
		for(int i = 0; i < size; i++)
			for(int j = 0; j < size; j++)
				if (i != j)
					obj_fun += (R.Get_m_Array()[i][j] * x[i][j]);
		model.add(IloMaximize(env, obj_fun));
		//add simple constraints 
		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				if (i != j)
					model.add(x[i][j] + x[j][i] == 1);
				else
					model.add(x[i][i] == 0);
		// add 3-dicycles constraints
		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				for (int k = 0; k < size; k++)
					if ((i != j) && (j != k) && ( k != i))
						model.add(x[i][j] + x[j][k] + x[k][i] <= 2);
		//fix varibles based on the lp solution
		for (int i = 0; i < size; i++)
			for(int j =0; j < size; j++)
				if(diff_score_matrix[i][j] > threshold){
					model.add(x[i][j] == 1);
					model.add(x[j][i] == 0);
				}
		cplex.setOut(env.getNullStream());
		//	cplex.setParam(IloCplex::EpGap, 0.39);
		cplex.addMIPStart(start_var, start_val, IloCplex::MIPStartAuto,  "secondMIPStart");
		time_t start, end;
		time(&start);
		if ( !cplex.solve() ) {
			env.error() << "Failed to optimize LP." << endl;
			throw (-1);
		}
		time(&end);
		//fill the solution
		s.time = difftime(end, start);
		s.obj_value = cplex.getObjValue();	
		for(int i =0; i < size; i++)
			for(int j =0; j < size; j++)
				s.sol_vector[i][j] = cplex.getValue(x[i][j]);
		env.end();
	}
	catch ( IloException & e){
		cerr << "Concert exception caught in Local search: " << e << endl;
	} catch (...){
		cerr << "Unknown exception caught in local search" << endl;
	}

	return s;
}

