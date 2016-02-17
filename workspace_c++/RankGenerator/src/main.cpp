#include <iostream>
#include <cstdlib>
#include <time.h>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <assert.h>
#include <string>
#include <iostream>
using namespace std;
int main(int argc, char* argv[])
{
	srand(time(NULL));
	int nVoters, nCandidates,k;
	istringstream a(argv[1]);
	istringstream b(argv[2]);
	istringstream c(argv[3]);
	a >> nVoters ;
	b >> nCandidates;
	c >> k;
	for(int n = 0; n < 10 ; n++){
		ofstream ranking;
		string name("./data/E");
		stringstream s;
		s << nVoters << "_" << nCandidates << "_" << k << "_" << n <<".txt";
		name += s.str();
		ranking.open(name.c_str());
		assert(nVoters > k || nCandidates > k);
		vector<vector<int> > matrix(nVoters);
		for(int i = 0; i < nVoters; i++)
			matrix[i].resize(nCandidates);
		vector<int> r(nCandidates);
		for(int i = 0; i < nCandidates; i++)
			r[i] = i;
		random_shuffle(r.begin(), r.end());
		for(int i = 0; i < nVoters; i++){
			matrix[i] = r;
			random_shuffle(r.begin(), r.end());
		}
		ranking << nVoters << endl;
		ranking << nCandidates << endl;
		ranking << k << endl;
		for(int i=0; i < nCandidates; i++)
		{
			for(int j=0; j < nVoters; j++)
				ranking<< matrix[j][i] << " ";
			ranking<<endl;
		}
		ranking.close();
	}
	return 0;
}
