/*
 * election_class.cpp
 *
 *  Created on: Jan 14, 2014
 *      Author: eia2
 */


#include "election_class.h"




election::~election()
{
	for (int i = 0; i < nCandidates; i++)
		delete rank_score[i];
	delete [] rank_score;
}





void election::Generate_Rank_General_Election() 
	/*******************************************************************/
	/* This function Generates and prepare the Ranking of an election  */
	/*******************************************************************/
{


	//srand(time(0));
	int * rank = new int[nCandidates];
	for (int i = 0 ; i < nCandidates ; i++)
		rank[i] = i;

	for (int voter = 0; voter < nVoters; voter++)
	{
		for (int i = 0; i < nCandidates; i++)
		{
			int r = static_cast<int>((nCandidates-i)*(rand()/(RAND_MAX + 1.0)) + i );
			int temp = rank[i];
			rank[i] = rank[r];
			rank[r] = temp;
		}

		for (int candidate = 0 ; candidate < nCandidates ; candidate ++)
			rank_score[candidate][voter] = rank[candidate]+1 ;
	}
/*	
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
			rank_score[candidate][voter] = rank[candidate]+1 ;
		}
	}
*/
	for(int i = 0; i < nCandidates ; i++)
	{
		for(int j = 0;j < nVoters ; j++)
			result << rank_score[i][j] <<  " ";
		result << endl;
	}


}

/*



   void election::Generate_Rank_General_Election()
   {
//	srand (time(NULL));
for (int v = 0 ; v < nVoters ; v++)
{
int * rank = new int[nCandidates];
int r = (int) (fact(nCandidates) * (rand() / (RAND_MAX + 1.0)));
PermLexUnrank(rank, nCandidates, r);
//ConverRanksArrayToScoreArray(rank, nCandidates);
for (int c = 0 ; c < nCandidates ; c++)
{
rank_score[c][v] = rank[c];
}
delete []rank;
}

for(int i = 0; i < nCandidates ; i++)
{
for(int j = 0; j < nVoters ; j++)
cout << setw(2) <<rank_score[i][j] <<  " ";
cout << endl;
}

}


*/

void election::Read_Rank_From_File_General_Election()
{

	vector<string> tokens = ConvertRankToVector(iElectFile);

	vector<string>::iterator the_iterator = tokens.begin();

	SetVoters( STLConvert(tokens.at(0)));
	SetCandidates( STLConvert(tokens.at(1)));
	SetK(STLConvert(tokens.at(2)));

	the_iterator += 3;
	cout << nVoters <<" , " <<  nCandidates << " , " << k << endl;


	rank_score = new int *[nCandidates];
	for(int i = 0; i < nCandidates; i++)
		rank_score[i] = new int [nVoters];
	int index_candidates = 0;
	int index_voters = 0;


	while( the_iterator != tokens.end() )
	{
		// cout << *the_iterator << endl;
		int u_ij = STLConvert(*the_iterator);
		rank_score[index_candidates][index_voters] = u_ij;
		++index_voters;
		if (index_voters == nVoters)
		{
			index_voters = 0;
			index_candidates++;
		}
		++the_iterator;
	}
	for(int i = 0; i < nCandidates; i++)
	{
		for(int j = 0; j < nVoters; j++)
		{
			cout << rank_score[i][j] <<  " ";
			result << rank_score[i][j] << " ";
		}
		cout << endl;
		result << endl;
	}
	//convert to score array
}




void init(int a[], int size)
{
	for(int i = 0; i < size; i++)
	{
		a[i] = i;
	}
}

void InsertionSort( float a[], int size)
{
	int i, j, key;
	for(j = 1; j < size; j++)    // Start with 1 (not 0)
	{
		key = a[j];
		for(i = j - 1; (i >= 0) && (a[i] > key); i--)   // Smaller values move down
		{
			a[i+1] = a[i];
		}
		a[i+1] = key;    //Put key into its proper location
	}

}


void InsertionSortwithIndexArray( float a[], int b[],int size)
{
	int i, j, key,indexkey;
	for(j = 1; j < size; j++)    // Start with 1 (not 0)
	{
		key = a[j];
		indexkey = b[j];
		for(i = j - 1; (i >= 0) && (a[i] > key); i--)   // Smaller values move down
		{
			a[i+1] = a[i];
			b[i+1] = b[i];
		}
		a[i+1] = key;    //Put key into its proper location
		b[i+1] = indexkey;


	}
	return;
}


void Generate_Candidate_Coordinate(float candidate_array[], int size)
	/*******************************************************************/
	/* This function Generates and prepare the ranking of an election  */
	/* the preference of voters have single peakedness property        */
	/* (candidates has an order) and voters preference are unimodal    */
	/*******************************************************************/
{
//	srand((unsigned)time(0));
	int i = 0;
	while (i < size)
	{
		bool flag = true;
		float temp = (rand() % 100 ) + 1;
		int j = 0;
		while (flag && j < i)
		{
			if (temp == candidate_array[j])
				flag = false;
			j++;
		}
		if (flag)
		{
			candidate_array[i] = temp;
			i++;
		}

	}
	InsertionSort(candidate_array, size);

}



void election::Convert_Ranks_To_Scores()
{
	for (int j = 0; j < nVoters; j++ )
	{
		int * t = new int [nCandidates];
		//int t[nCandidates];

		for (int i = 0; i < nCandidates; i++)
			t[rank_score[i][j]-1] = i+1;
		for (int i = 0; i < nCandidates; i++)
		{
			rank_score[i][j] = t[i];
			//cout << t[i] << " ";
		}
		//cout << "Hi" << endl;
		delete [] t;
	}
}

void CreateSinglePeakPermMatrix(int a[4][3])
{
	a[0][0] = 1;
	a[0][1] = 2;
	a[0][2] = 3;

	a[1][0] = 2;
	a[1][1] = 1;
	a[1][2] = 3;

	a[2][0] = 3;
	a[2][1] = 1;
	a[2][2] = 2;


	a[3][0] = 3;
	a[3][1] = 2;
	a[3][2] = 1;
}



void Create_3_Weights_Rank(int rank[], int nCandidates)
{

	int perm_matrix[4][3];
	CreateSinglePeakPermMatrix(perm_matrix);

	int f_index,e_index  = rand() % (nCandidates-3) + 1;
	while (abs(f_index - e_index) >= 2)
	{
		e_index = rand() % (nCandidates-3) + 1;
	}

	int perm_index = rand() % 4;
	for (int i = 0; i <= f_index; i++)
		rank[i] = perm_matrix[perm_index][0];
	for (int i = f_index+1; i <= e_index; i++)
		rank[i] = perm_matrix[perm_index][1];
	for (int i = e_index+1; i < nCandidates ; i ++)
		rank[i] = perm_matrix[perm_index][2];

}


void election::Generate_Rank_SinglePeaked_3_Weights()
{
	ofstream file("election_3_weights.dat");
	for (int j = 0; j < nVoters; j++)
	{
		int rank[nCandidates];
		Create_3_Weights_Rank(rank, nCandidates);
		for (int i = 0; i < nCandidates; i++)
			rank_score[i][j] = rank[i];
	}
	file << endl;
	cout << endl;
	for (int i = 0; i < nCandidates; i++)
	{
		for (int j = 0; j < nVoters; j++)
		{
			file << rank_score[i][j] << " ";
			cout << rank_score[i][j] << " ";
		}
		file << endl;
		cout << endl;
	}


}




void election::Generate_Rank_SinglePeaked_Election()
	/**
	 * ranks generates from 0 to m-1
	 */
{


	ofstream file("sp_election.dat");

	float candidate_array[nCandidates];
	Generate_Candidate_Coordinate(candidate_array, nCandidates);

	for (int j = 0; j < nVoters; j++)
	{
		double r = 0, voter_pos = 0;
		r = rand() % 1000 + 1;
		voter_pos = r / 10;

		int position_array[nCandidates];
		init(position_array,nCandidates);

		float distance_array[nCandidates];
		for(int i = 0; i < nCandidates; i++)
		{
			distance_array[i] = abs(voter_pos - candidate_array[i]);
		}
		InsertionSortwithIndexArray(distance_array, position_array, nCandidates);

		for (int i = 0; i < nCandidates; i++)
		{
			rank_score[i][j] = position_array[i] + 1;
		}
	}

	for(int i = 0; i < nCandidates; i++)
	{
		file << i << ' ';
		for(int j = 0; j < nVoters; j++)
		{
			file << rank_score[i][j] << ' ';
			cout << rank_score[i][j] <<  " ";
		}
		cout << endl;
		file << endl;
	}
	Convert_Ranks_To_Scores();
	file.close();
}





ostream& operator <<(ostream& out, const election& e)
{
	out << e.Voters() << endl;
	out << e.Candidates() << endl;
	out << e.K() << endl;
	for(int i = 0; i < e.Candidates(); i++)
	{
		for(int j = 0; j < e.Voters() ; j++)
			out <<e.Ranking()[i][j] <<  " ";
		out << endl;
	}
	return out;
}










