#ifndef VARIABLES_H
#define VARIABLES_H

//#include <ilcplex/ilocplex.h>

#include <vector>

using namespace std;

/*typedef IloArray<IloBoolVarArray> BoolVarMatrix;
typedef IloArray<BoolVarMatrix> ThreeDMatrix;*/

const int MAX_JOB = 1000;
const int MAX_WORKER = 100;

int num_job;
int num_worker;

/*int jobid[MAX_JOB];
int job_skill[MAX_JOB];
static int skilllow[MAX_WORKER];
static int skillhigh[MAX_WORKER];*/

bool serviceable[MAX_WORKER][MAX_JOB]; // shows whether the job is serviceable by the driver
double cost[MAX_JOB][MAX_JOB]; //store cij.

vector<vector<int> > service_subsets; // stores the current trees
vector<vector<int> > subsets_matrix; // stores subsets matrix
vector<double> cost_of_subset; // stores cost of each tree being used

/*vector<vector<int>> all_subsets;
double all_subsets_cost[MAX_JOB];*/

#endif
