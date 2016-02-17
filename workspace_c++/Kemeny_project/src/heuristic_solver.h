/*
 * heuristic_solver.h
 *
 *  Created on: March 28, 2014
 *      Author: eia2
 */

#ifndef HEURISTIC_SOLVER_H_
#define HEURISTIC_SOLVER_H_


#include "ranking.h"
#include "Functions.h"
#include "pair_search_tree.h"
typedef IloArray<IloBoolVarArray> BoolVarMatrix;
typedef IloArray<IloNumVarArray> NumVarMatrix;
typedef IloArray<NumVarMatrix> ThreeDMatrix;

struct dual_sol_vectors{
	vector<double> y;
	vector<double> z;
	dual_sol_vectors(int n){
		y.resize(n*(n-1));
		for(int i = 0; i < n*(n-1);i++)
			y[i] =0;
		z.resize((n*(n-1)*(n-2))/3);
		for(int i =0; i < (n*(n-1)*(n-2))/3;i++)
			z[i] = 0;
	}
};

struct  solution{ 
	//const char* mod;
	double obj_value;
	double time;
	vector<vector<double> > sol_vector;
	solution(int n):/*mod("lp"),*/time(0),obj_value(0){
		sol_vector.resize(n);
		for(int i = 0; i < n; i++)
			sol_vector[i].resize(n);
	}
};

class HeurSolver
{    
	Ranking  &R;
public :
	HeurSolver(Ranking& r):R(r){}
	~HeurSolver(){};

	solution SolveILP();
	solution SolveILP(vector<vector<double> >& matrix);
	solution SolveLP();
	solution SolveLPDual();
	solution SolveLP(vector<vector<double> >& matrix);	
	solution SolveLPIterat();
	solution  SolvePrimalDual();
	void SolveDRP(vector<pair<int,int> >& tight_edges, vector<dicycle>& tight_cycle, EdgeManager& Edge, CycleManager& Cycle,dual_sol_vectors& drp_solution);
	void Combinatorial_DRP_Solve(vector<pair<int,int> >& tight_edges, vector<dicycle>& tight_cycle, EdgeManager& Edge, CycleManager& Cycle,dual_sol_vectors& drp_solution);
	solution LPRoundingHeuristic(solution& lp);
	void MIP_Heuristic();
	void MIP_Heuristic_150();
	void MIP_Heuristic_200();
	void MIP_Heuristic_500();
	solution CreateInitialSolution(solution& lp);
	double OrderingValue(vector<int> ordering);
	double OrderingValue(vector<int>& order, double obj_val, int object, int old_pos, int new_pos);
	vector<vector<int> > GetSubRanking(vector<int> partial_order);
	void CreateModel(IloEnv env,IloModel model, BoolVarMatrix x);
	void CreateModel(IloEnv env,IloModel model, NumVarMatrix x);
	vector<int> SolveSubProblem(vector<vector<int> > matrix, vector<int> cand_subset, IloNumArray st);
	vector<int> ImproveByCplex(vector<int> cand_subset, IloNumArray start_val);
	vector<int> LocalSearch(vector<int> cand_subset, IloNumArray start_val);
	vector<int> LocalSearch(vector<vector<int> > matrix, vector<int> cand_subset, IloNumArray start_val,  solve_status status, int p);
	vector<int> LastStepMIP(vector<int>& order);
	IloNumArray OrderToNumArray(IloEnv env, vector<int> order);
	void LPHeuristic();
	solution FixnSolve(vector<vector<double> >& diff_score_matrix, vector<int>& current_solution, int size, int threshold);
	void LPHeuristic(solution& lp, vector<int>& init_ordering);
	int ExistViolateDicycle(IloModel model,IloCplex cplex , NumVarMatrix x);
	void UpdateVarHistory(vector<vector<int> >& history, vector<int>& order);
	vector<int> LinKernighanLocalSearch(vector<int>& order, int size);
	vector<int> Preprocessing();
	void KernelSearch();
	vector<int> IterativeFixing(solution& lp, vector<int>& init_order, double threshold, int iteration);

	void InitialConstraintManager(vector<vector<dicycle> >& ConstManager, EdgeManager& Edge, CycleManager& Cycle, int size_edge);
	void PrintConstManager(vector<vector<dicycle> >& ConstManager, EdgeManager& Edge);
	double ComputeTeta(vector<vector<dicycle> >& ConstManager, dual_sol_vectors& dual_solution, dual_sol_vectors& drp_solution, 
		EdgeManager& Edge, CycleManager& Cycle);
	bool DualFeasibile(vector<vector<dicycle> >& ConstManager, dual_sol_vectors& dual_solution, 
		EdgeManager& Edge, CycleManager& Cycle, vector<pair<int,int> >& tight_edges, vector<dicycle>& tight_cycles);
	double SolutionValue(dual_sol_vectors& v);
	void PrintVector( vector<double>& s );

};

#endif /* HEURISTIC_SOLVER_H_ */
