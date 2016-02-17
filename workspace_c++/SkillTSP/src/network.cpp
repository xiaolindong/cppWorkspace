/*
 * Ranking.cpp
 *
 *  Created on: March 19, 2014
 *      Author: eia2
 */


#include "network.h"

//Network::~Network(){}
void Network::GenerateNetwork(){}
void Network::ReadNetwork(){
    fin >> nJobs >> nWorkers;
    for(int i = 0; i < nJobs; i++){
        fin >> jobs[i].jobID;
        fin >> jobs[i].jobSkill;
    }
    for(int i = 0; i < nWorkers; i++)
        fin >> workers[i].skilllow >> workers[i].skillhigh;
    
    for(int i = 0; i < nJobs; i++)
        for(int j = 0; j < nJobs; j++)
            fin >> cost[i][j];
}
//void Network::PrintNetwork(){}
ostream& operator <<(ostream& out, const Network& n)
{
	out << n.get_n_jobs() << endl;
	out << n.get_n_workers() << endl;
	for(int i = 0; i < n.get_n_jobs() ; i++)
	{
		for(int j = 0; j < n.get_n_jobs() ; j++)
			out << n.Cost()[i][j] <<  " ";
		out << endl;
	}
	return out;
}
