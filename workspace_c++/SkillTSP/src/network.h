/*
 * Network.h
 *
 *  Created on: March 19, 2014
 *      Author: eia2
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include "../include/master_header.h"

struct Job{
    int jobID;
    int jobSkill;
    Job(int id=0, int s=0):jobID(id), jobSkill(s){};
};

struct Worker{
    int workerID;
    int skilllow;
    int skillhigh;
    Worker(int id=0, int l=0, int h =0):workerID(id), skilllow(l), skillhigh(h){};
};

class Network{
    int nJobs;
    int nWorkers;
    Job* jobs;
    double** cost;
    Worker* workers;
	ifstream fin;
public :
	Network(char* s): fin(s) {};
	Network(): fin("../data/tsp.txt"){};
	Network(int j, int w):
		nJobs(j), nWorkers(w){
         jobs = new Job[nJobs];
         workers = new Worker[nWorkers];
         cost = new double* [nJobs];
         for(int i = 0; i < nJobs; i++)
             cost[i] = new double[nJobs];
  	 }
	//~Network();
    void GenerateNetwork();
    void ReadNetwork();
    double** Cost() const {return cost;};
    int get_n_jobs() const  { return nJobs;};
    int get_n_workers() const  {return nWorkers;};
};
ostream& operator <<(ostream& out, const Network& e);

#endif /* Network_H_ */
