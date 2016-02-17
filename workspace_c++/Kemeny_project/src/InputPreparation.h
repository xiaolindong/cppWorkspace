/*
 * InputPreparation.h
 *
 *  Created on: Oct 7, 2013
 *      Author: eia2
 */

#ifndef INPUTPREPARATION_H_
#define INPUTPREPARATION_H_



#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <map>
#include <algorithm>
#include "variables.h"
#include <cmath>


using namespace std;



int FindRankinProfile(int voter, int candidates)
/**************************************************
 * This function finds ranking of a particular    *
 * candidates in a a specified voter              *
 *************************************************/
{
for (int i = 0; i < nCandidates; i++)
	if (u[i][voter] == candidates)
		return i;
return -1;
}


void preparArrayOfConflicts()
/***********************************************************************
 * This function prepare the number of conflicts between each pair of  *
 * candidates with respect to voters profile 						   *
/**********************************************************************/
{
	m = new int *[nCandidates];
	for(int i = 0; i < nCandidates ; i++)
		m[i] = new int [nCandidates];

	for (int i = 0; i < nCandidates; i++)
		for (int j = 0; j < nCandidates; j++)
			m[i][j] = 0;

	for (int i = 0; i < nCandidates; i++)
		for (int j = 0; j < nCandidates; j++)
			if(i != j)
				for (int voter = 0; voter < nVoters; voter++)
				{
					int rank_i = FindRankinProfile(voter, i);
					int rank_j = FindRankinProfile(voter, j);
					if (rank_i < rank_j)
						m[i][j]++;
					else
						m[j][i]++;
				}


}

void preparKemenyRanking()
/*******************************************************************/
/* This function Generates and prepare the ranking of an election  */
/*******************************************************************/
{

	u = new int *[nCandidates];
		for(int i = 0; i < nCandidates ; i++)
			u[i] = new int [nVoters];


	int * rank = new int[nCandidates];
	for (int i = 0 ; i < nCandidates ; i++)
		rank[i] = i;
	for (int voter = 0 ; voter < nVoters ; voter++)
	{
		int repeat = 1 + (int) (200 * (rand() / (RAND_MAX + 1.0)));
		for (int i = 0; i < repeat ; i++)
		{
			int index_first = (int) (nCandidates * (rand() / (RAND_MAX + 1.0))) ;
			int index_second = (int) (nCandidates * (rand() / (RAND_MAX + 1.0))) ;
			int temp = rank[index_first];
			rank[index_first] = rank[index_second];
			rank[index_second] = temp;
		}
		for (int candidate = 0 ; candidate < nCandidates ; candidate ++)
		{
			u[candidate][voter] = rank[candidate] + 1;
		}
	}
	/*
	for(int i = 0; i < nCandidates ; i++)
	{
		for(int j = 0; j < nVoters ; j++)
			cout << u[i][j] <<  " ";
		cout << '\n';
	}*/

	preparArrayOfConflicts();
}






#endif /* INPUTPREPARATION_H_ */
