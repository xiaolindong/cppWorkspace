/*
 * variables.h
 *
 *  Created on: Oct 7, 2013
 *      Author: eia2
 */

#ifndef VARIABLES_H_
#define VARIABLES_H_


#include <ilcplex/ilocplex.h>
#include<vector>

int nVoters;                            // number of voters
int nCandidates;                        // number of candidates
int k; 									// number of candidates to be selected
int **m;								// keep the n number of voters prefer i to j
int **u;								// The representation matrix value
IloEnv env;
typedef IloArray<IloBoolVarArray> BoolVarMatrix;
typedef IloArray<IloNumVarArray> NumVarMatrix;


#endif /* VARIABLES_H_ */
