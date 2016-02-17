/*
 * variables.h
 *
 *  Created on: Mar 23, 2012
 *      Author: eia2
 */

#ifndef VARIABLES_H_
#define VARIABLES_H_

#include <ilcplex/ilocplex.h>
//#include "/cs/grad3/eia2/workspace/NEW_MSP_IPCUT/src/EdgeManager.h"
#include <map>


typedef IloArray < IloBoolVarArray > BoolVarMatrix ;
typedef IloArray < BoolVarMatrix> ThreeDMatrix;

const int MAX_JOB = 1000;
const int MAX_WORKER = 100;
static int jobid[MAX_JOB];
static int num_job;
static int num_worker;
static int job_skill[MAX_JOB]; //start from index 1;
//static int worker_skill[MAX_WORKER][100]; //start from index 0;
static double cost[MAX_JOB][MAX_JOB]; //store cij.
static int skilllow[MAX_WORKER];
static int skillhigh[MAX_WORKER];
//static EdgeManager EdgeMatrix;

#define nEdges ((num_job*(num_job-1))/2)


#endif /* VARIABLES_H_ */
