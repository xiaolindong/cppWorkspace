/*
 * Ranking.h
 *
 *  Created on: March 19, 2014
 *      Author: eia2
 */

#ifndef RANKING_H_
#define RANKING_H_

#include"master_header.h"
#include "Functions.h"
#include "map_data_structure.h"

class Ranking
{
	const char mem_releas;       // show if the rank is read from file, f, or created ,c (used in releasing memory)
	int nVoters, nCandidates,nInstances;
	int ** rank_score;
	int ** m;
	int ** diff_matrix;
	double optVal;
	int row_size_diff_matrix;
	MapDataStruct triple_map;
	ifstream inputRankFile;
	ofstream result;

public :
	Ranking(char* s):
		inputRankFile(s),
		optVal(-1),mem_releas('f'),
		result("solution_file.dat")
	{};

	Ranking():
		inputRankFile("../data/N-atp24"),
		optVal(-1),mem_releas('f'),
		result("solution_file.dat")
	{};
//	Ranking(ifstream& file):
//		inputRankFile(file) {};

	Ranking(int n, int c):
		nVoters(n), nCandidates(c), result("solution_file.dat"),mem_releas('c')
 	 {
		row_size_diff_matrix = ((nCandidates*(nCandidates-1))/2)*nVoters;

		rank_score = new int *[nCandidates];
		for (int i = 0; i < nCandidates; i++)
			rank_score[i] = new int[nVoters];

		m = new int *[nCandidates];
		for (int i = 0; i < nCandidates; i++)
			m[i] = new int[nCandidates];
	
		diff_matrix =new int *[row_size_diff_matrix];
		for (int i = 0; i < row_size_diff_matrix; i++)
			diff_matrix[i] = new int [nCandidates];
		
		optVal = -1;
		nInstances = 10;
  	 }

	~Ranking();

	int Candidates() const {return nCandidates;}
	int Voters() const {return nVoters;}
	int** Rank() const {return rank_score;}
	double OptVal() const  {return optVal;}
	void SetOptVal(double v) { optVal = v;}
	void SetCandidates(int m) { nCandidates = m;}
	void SetVoters(int n) {nVoters = n;}
	int GetRowSizeDiffMatrix() const {return row_size_diff_matrix;}
	int** GetDiffMatrix() const {return diff_matrix;}
	int** Get_m_Array() const {return m;}
	ofstream& eFile() {return result;}
	
	int FindRankinProfile(int voter, int candidate);
	void Initials_Needed_For_Read_From_Input_File();
	void PreparArrayOfConflicts();
	void Initial_m_Matrix();
	void Initial_Diff_Matrix();
	void FillOutDiffMatrix();
	void FillOutTripleMap();
	void PrintRankingToFile();
	void PrintDiffMatrix();
	void Print_m_Matrix();

	void Generate_Kemeny_Ranking();
	void Generate_Instances();
	void Read_Rank_From_File_Kemeny();
	void PreFunctionsCall();

	void Generate_Rank_SinglePeaked_Kemeny();
	void Read_Rank_From_File_SinglePeaked_Kemeny();
	void Initiate_m_Matrix_From_File();


	class ColGenSolver;
	friend class ColgenSolver;

	class OldILPSolver;
	friend class OldILPSolver;
	
	class NewILPSolver;
	friend class NewILPSolver;

};

ostream& operator <<(ostream& out, const Ranking& e);

#endif /* RANKING_H_ */
