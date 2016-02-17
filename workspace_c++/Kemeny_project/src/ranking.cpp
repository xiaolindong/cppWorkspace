/*
 * Ranking.cpp
 *
 *  Created on: March 19, 2014
 *      Author: eia2
 */


#include "ranking.h"

inline int  min(int a , int b) {return a > b ? b : a; }
inline int max(int a, int b) {return a > b ? a : b;}

Ranking::~Ranking()
{
	for (int i = 0; i < nCandidates; i++)
			delete m[i];
		delete [] m;
	result.close();
	if (mem_releas == 'c')
	{
		for (int i = 0; i < nCandidates; i++)
			delete rank_score[i];
		delete [] rank_score;
		for (int i = 0; i < row_size_diff_matrix; i++)
			delete diff_matrix[i];
		delete [] diff_matrix;
	}
	else if (mem_releas == 'f')
		inputRankFile.close();
}




void Ranking::Initiate_m_Matrix_From_File()
{
	string line;
	//cout << line << endl;
	getline(inputRankFile, line);
	nCandidates = STLConvert(line);
	cout << "number of candidates are : " << nCandidates <<endl;


	m = new int *[nCandidates];
		for (int i = 0; i < nCandidates; i++)
			m[i] = new int[nCandidates];

	int i =0;
	while (getline(inputRankFile, line))
	{
		int j = 0;
		vector<string> token;
			Tokenize(line,token);
		vector<string>::iterator it;
		for(it = token.begin(); it != token.end(); it++)
		{
			int m_ij = STLConvert(*it);
			m[i][j++]= m_ij;
		}
		i++;
	}
	inputRankFile.close();
/*	for (int i = 0; i < nCandidates; i++)
	{
		for (int j = 0; j < nCandidates; j++)
			cout << m[i][j] << " " ;
		cout << endl;

	}*/



}


void Ranking::Read_Rank_From_File_Kemeny()
{

	vector<string> tokens = ConvertRankToVector(inputRankFile);

	vector<string>::iterator the_iterator = tokens.begin();

	SetVoters( STLConvert(tokens.at(0)));
	SetCandidates( STLConvert(tokens.at(1)));

	the_iterator += 2;
	cout << nVoters <<" , " <<  nCandidates << endl;


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
	Initials_Needed_For_Read_From_Input_File();
	PreparArrayOfConflicts();   
}


void Ranking::Print_m_Matrix()
{
	cout << endl << "****** The matrix m is : ******* "  << endl;
	for (int i = 0; i < nCandidates; i++)
	{
		for (int j = 0; j < nCandidates; j++)
			cout << m[i][j] << " ";
		cout << endl;
	}
	cout <<  "***************** End of matrix m ***********" << endl;
}


void Ranking::PrintDiffMatrix()
{
	cout << endl;
	for (int i = 0; i < row_size_diff_matrix; i++)
	{
		for (int j = 0; j < nCandidates; j++)
			cout << diff_matrix[i][j] << " ";
		cout << endl;
	}
}


ostream& operator <<(ostream& out, const Ranking& e)
{
	out << e.Voters() << endl;
	out << e.Candidates() << endl;
	for(int i = 0; i < e.Candidates(); i++)
	{
		for(int j = 0; j < e.Voters() ; j++)
			out <<e.Rank()[i][j] <<  " ";
		out << endl;
	}
	return out;
}





void Ranking::Initials_Needed_For_Read_From_Input_File()
{
	row_size_diff_matrix = ((nCandidates*(nCandidates-1))/2)*nVoters;

	m = new int *[nCandidates];
	for (int i = 0; i < nCandidates; i++)
		m[i] = new int[nCandidates];

	diff_matrix =new int *[row_size_diff_matrix];
	for (int i = 0; i < row_size_diff_matrix; i++)
		diff_matrix[i] = new int [nCandidates];

}

void Ranking::Initial_m_Matrix()
{
	for (int i = 0; i < nCandidates; i++)
		for (int j = 0; j < nCandidates; j++)
			m[i][j] = 0;
}


int Ranking::FindRankinProfile(int voter, int candidates)
	/**************************************************
	 * This function finds Ranking of a particular    *
	 * candidates in a a specified voter              *
	 *************************************************/
{
	for (int i = 0; i < nCandidates; i++)
		if (rank_score[i][voter] == candidates)
			return i;
	return -1;
}


void Ranking::PreparArrayOfConflicts()
/***********************************************************************
 * This function prepare the number of conflicts between each pair of  *
 * candidates with respect to voters profile 								 */	
{
	Initial_m_Matrix();
	for (int i = 0; i < nCandidates; i++)
		for (int j = i + 1; j < nCandidates; j++)
		{
			for (int voter = 0; voter < nVoters; voter++)
			{
				int rank_i = FindRankinProfile(voter, i);
				int rank_j = FindRankinProfile(voter, j);
				if (rank_i < rank_j)
					m[i][j]++;
			}
			m[j][i] = nVoters - m[i][j];
		}

}


void Ranking::Generate_Instances()
{
	srand(time(NULL));
	for (int i = 0; i < nInstances; i++)
	{
		stringstream ss;
		ss << i;
		string name("../data/kem_instance_");
		name += ss.str();
		name += ".txt";
		const char* n = name.c_str();
		ofstream file;
		file.open(n);
		file << nVoters << endl;
		file << nCandidates << endl;
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
		for(int i = 0; i < nCandidates ; i++)
		{
			for(int j = 0; j < nVoters ; j++)
				file << rank_score[i][j] <<  " ";
			file << endl;
		}
		file.close();
	}
}

void Ranking::Generate_Kemeny_Ranking()
	/*******************************************************************/
	/* This function Generates and prepare the Ranking of an election  */
	/*******************************************************************/
{


	//	srand(time(NULL));
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
	/*
	   for(int i = 0; i < nCandidates ; i++)
	   {
	   for(int j = 0; j < nVoters ; j++)
	   cout << rank_score[i][j] <<  " ";
	   cout << '\n';
	   }*/

	PreparArrayOfConflicts();
}




void Ranking::FillOutTripleMap()
{

	int label = 0;
	for (int v = 0; v < nVoters; v++)
	{
		for (int i = 0; i < nCandidates-1; i++)
			for (int j = i+1; j < nCandidates; j++)
				triple_map.AddTriple(v,i,j, label++);
	}

}

void Ranking::FillOutDiffMatrix()
{
	Initial_Diff_Matrix();
	for (int v = 0; v < nVoters; v++)
	{
		for(int i = 0; i < nCandidates-1; i++)
			for (int j = i+1; j < nCandidates; j++)
			{
				int row_index_dif_matrix;
				//cout << "rank i,v is : " << rank_score[i][v] << "and rank j, v is : " << rank_score[j][v] << " ";
				//	cout << endl;
				//row_index_dif_matrix = triple_map.GetLabel(v, rank_score[j][v], rank_score[i][v] );
				//diff_matrix[row_index_dif_matrix][rank_score[i][v]] = 1;
				//diff_matrix[row_index_dif_matrix][rank_score[j][v]] = -1;				


				//	if (rank_score[i][v] > rank_score[j][v])

				row_index_dif_matrix = triple_map.GetLabel(v, min( rank_score[j][v], rank_score[i][v]) , max(rank_score[j][v], rank_score[i][v]) );
				//	else
				//		row_index_dif_matrix = triple_map.GetLabel(v, rank_score[i][v], rank_score[j][v]);

				diff_matrix[row_index_dif_matrix][rank_score[i][v]] = 1;
				diff_matrix[row_index_dif_matrix][rank_score[j][v]] = -1;				
			}		
	}	

}



void Ranking::Initial_Diff_Matrix()
{
	for (int i = 0; i < row_size_diff_matrix; i++)
		for (int j = 0; j < nCandidates; j++ )
			diff_matrix[i][j] = 0;
}


void Ranking::PreFunctionsCall()
{
	FillOutTripleMap();
	//	triple_map.PrintMapData();
	FillOutDiffMatrix();
}



