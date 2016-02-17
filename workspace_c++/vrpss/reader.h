#ifndef READER_H
#define READER_H

#include <fstream>

#include "variables.h"

/**************read data from tsp.txt******************/
void readfile(){
	ifstream fin;
	fin.open("tsp1.txt");

	fin>>num_job;
	fin>>num_worker;
	cout<<num_job<<" "<<num_worker<<endl;

	int job_skill[num_job + 1];
	for(int i = 1; i <= num_job; i++){
		int jobid;
		fin>>jobid;
		fin>>job_skill[i];
		cout<<jobid<<" "<<job_skill[i]<<endl;
	}
	cout<<endl;

	for(int i = 0; i < num_worker; i++){
		int skilllow, skillhigh;
		fin>>skilllow>>skillhigh;
		cout<<skilllow<<" "<<skillhigh<<endl;

		for(int j = 1; j <= num_job; j++)
			serviceable[i][j] = skilllow <= job_skill[j] && job_skill[j] <= skillhigh;
	}

	for(int i = 0; i <= num_job; i++){
		for(int j = 0; j <= num_job; j++)
		{
			fin>>cost[i][j];
			cout<<cost[i][j]<<" ";
		}
		cout<<endl;
	}
}

#endif
