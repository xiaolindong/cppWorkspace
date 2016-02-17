#include<iostream>
#include<fstream>
#include<ostream>


using namespace std;

int main(int n_instance)
{
	for (int i = 0; i < n_instance; i++)
	{
		rand(time(NULL));
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
				rank_score[candidate][voter] = rank[candidate] ;
			}
		}
	}

	return 0;
}

