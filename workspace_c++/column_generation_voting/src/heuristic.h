
#ifndef HEURISTIC_H_
#define HEURISTIC_H_



#include "master_header.h"
#include "election_class.h"

struct solution{
	double obj_value;
	vector<int> candidates;
	vector<int> left_out_candidates;
	vector<int> assignment;
	solution( int k, int nvoter, int nCandidates, double v = 0 ): obj_value(v){
		candidates.resize(k);
		for(int i = 0; i < k; i++)
			candidates[i] = 0;
		assignment.resize(nvoter);
		for(int i = 0; i < nvoter; i++)
			assignment[i] = 0;
		left_out_candidates.resize(nCandidates-k);
		for(int i =0 ; i < nCandidates-k; i++)
			left_out_candidates[i] = 0;
	}
};



class Heuristic
{
	election &E;
public :
	Heuristic(election& e): E(e){};
	~Heuristic(){};
	solution LocalSearch();
	solution SolveIP();
	solution SolveLP();
	solution& GenerateFeasibleSolution(solution& sol);
	void ComputeOptimalAssignment(solution& sol);
	int BestCandidate(solution& sol, int voter);
	void SolutionValue(solution& sol);
	int IndexCandidate(int candidate, int voter);
};




#endif /* HEURISTIC_H_ */
