/*
 * dynamic_solver.cpp
 *
 *  Created on: Jan 14, 2014
 *      Author: eia2
 */

#include "dynamic_solver.h"


DynamicSolver::~DynamicSolver()
{
	for (int i = 0; i < E.Candidates();i++) 
	{
		delete dynamic_table[i];
		delete opt_path[i];
	}
	delete[] dynamic_table;
	delete[] opt_path;
	delete[] voters;

}



ostream& operator <<(ostream& out, const DynamicSolver& d)
{
	out << endl;
	for (int i =0; i < d.Candid();i++)
	{
		for (int j = 0; j < d.K(); j++)
			out << d.Table()[i][j] << " "; 
		out << endl;
	}
	return out;
}



void DynamicSolver::ExtractPrintDynamicSolution()
{
	cout << "hi3" << endl;
	int* solution = new int [E.K()];
	int c_indx = E.Candidates()-1, k_indx = E.K()-1, sol_indx = E.K()-1;	
	while (k_indx >= 0)
	{
		cout << "hi ehsan" << endl;
		assert(c_indx <= 0 || k_indx == 0);
		cout << "bye 1111" << endl;
		cout << "c_inx = " << c_indx << "and k_index is " << k_indx << endl;
		cout << "optpath is " << opt_path[c_indx][k_indx] << endl;
		while (opt_path[c_indx][k_indx] != '*'&&  opt_path[c_indx][k_indx] != '0' )
		{
			c_indx--;
			cout << "c_indx"<< c_indx << endl;
		}
		cout << "bye 2" << endl;
		assert(sol_indx >= 0);
		solution[sol_indx--] = c_indx;
		k_indx--, c_indx--;	
	}
	cout << "hi44" << endl;
	E.eFile() << endl <<"The selected candidates by dynamic program are: " ;
	cout << endl <<"The selected candidates by dynamic program are: " ;
	for (int i = 0; i < E.K(); i++ )
	{
		E.eFile() << solution[i] << " ";
		cout << solution[i]  << " ";
	}
	E.eFile() << endl;
	cout << endl;
	int indx = 0; 
	int bound = E.Voters() / E.K();
	for (int i = 0; i < E.K(); i++)
	{
		E.eFile() << "Assignment of candidate " << solution[i] << " are : ";
		cout << "Assignment of candidate " << solution[i] << " are : ";
		for (int j = 0; j < bound; j++)
		{
			E.eFile() << voters[i*bound + j] << " ";
			cout << voters[i*bound + j] << " ";
		}
		E.eFile() << endl;
		cout << endl;
	}
	
	delete[] solution;
}





int DynamicSolver::V(int c ,int beg_indx)
{
	// check if n/k is not an integer number
	int bound = E.Voters()/E.K();
	int sum = 0;
	for (int i = beg_indx; i < beg_indx + bound; i++)
	{
		sum += E.Ranking()[c][voters[i]]-1;
	} 
	return sum;
}

int min(int a , int b)
{
	return !(b<a)?a:b;
}

void DynamicSolver::FillTable()
// dynamic_table[i][j] means slecting' j+1' candidates while considering only up to candidate 'i'

{
	int bound = E.Voters()/E.K();
	dynamic_table[0][0] = V(0,0);
	opt_path[0][0] = '*';
	for (int i = 1; i < E.Candidates(); i++)
	{
		int a = V(i,0);
		int b = dynamic_table[i-1][0];
	//	cout << "A, B are : " << a << " ," << b << endl; 
		dynamic_table[i][0] = min(V(i,0), dynamic_table[i-1][0]);
		if (a > b)
		{
			opt_path[i][0]= '|';			
		//	cout << "A, B are new : " << a << " ," << b << endl; 
		//	cout << "error " << endl;
		}
		else 
			opt_path[i][0] = '*';
	}
	for(int j = 1; j < E.K(); j++)
		for (int i = j ; i < E.Candidates(); i++ )
			if (i==j)
			{
				dynamic_table[i][j] = dynamic_table[i-1][j-1] + V(i,j*bound);
				opt_path[i][j] = '\\';
			}
			else
			{
				int a = dynamic_table[i-1][j];
				int b = V(i,j*bound)+dynamic_table[i-1][j-1];
				dynamic_table[i][j] = min(a,b );
				if (a >= b)
					opt_path[i][j] = '*';
				else
					opt_path[i][j] = '|';
			}
}

void DynamicSolver::InitialDynTable()
{
	for (int i = 0; i < E.Candidates(); i++)
		for (int j = 0; j < E.K(); j++)
			dynamic_table[i][j] = 0; 
}



void DynamicSolver::LexicoSortOnVoters()
{
	InitialOptPath();
	InitialDynTable();
	int temp_rank[E.Candidates()][E.Voters()];

	//copy rank_score
	for (int i = 0; i < E.Candidates(); i++)
		for (int j = 0; j < E.Voters(); j++)
			temp_rank[i][j] = E.Ranking()[i][j];


	//initial voters array index
	for (int i = 0; i < E.Voters(); i++)
		voters[i] = i ;

	//lexigraoghic sort
	for (int d = E.Candidates()-1; d >= 0; d--)
	{
		int i, j, index_key,key[E.Candidates()];
		for(j = 1; j < E.Voters(); j++)    // Start with 1 (not 0)
		{
			for (int t = 0; t < E.Candidates(); t++)
			{
				key[t] = temp_rank[t][j];
			}
			index_key = voters[j];
			for(i = j - 1; (i >= 0) && (temp_rank[d][i] > key[d]); i--)   // Smaller values move down
			{
				for (int t = 0; t < E.Candidates(); t++)
				{
					temp_rank[t][i+1] = temp_rank[t][i];
				}
				voters[i+1] = voters[i];

			}
			for (int t = 0; t < E.Candidates(); t++)
			{
				temp_rank[t][i+1] = key[t];
			}
			voters[i+1] = index_key;
		}
	}

	cout << "The Lex order of voters are :" << endl;
	for (int i = 0; i < E.Voters(); i++)
		cout << voters[i] << " " ;
	cout << endl;
}


void DynamicSolver::PrintOptPath()
{

	cout << endl;
	for (int i = 0; i < E.Candidates(); i++)
	{
		for (int j = 0; j < E.K(); j++)
			cout << opt_path[i][j] << " ";
		cout << endl; 
	}
}

void DynamicSolver:: InitialOptPath()
{

	for (int i = 0; i < E.Candidates(); i++)
		for (int j = 0; j < E.K(); j++)
			opt_path[i][j] = '0';
}
