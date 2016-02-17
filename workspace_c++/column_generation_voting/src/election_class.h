/*
 * election_class.h
 *
 *  Created on: Dec 6, 2013
 *      Author: eia2
 */

#ifndef ELECTION_CLASS_H_
#define ELECTION_CLASS_H_

#define RC_EPS 1.0e-1000000


#include "Functions.h"
#include "master_header.h"


typedef IloArray<IloBoolVarArray> BoolVarMatrix;
typedef IloArray<IloNumVarArray> NumVarMatrix;



//void PermLexUnrank(int *a, int n , int r);


class election
{
	int nVoters, nCandidates, k;
	int ** rank_score;
	bool capacitated;
	double optValCap, optValUnCap;
	double val_ip, val_lp,val_colgen, val_dynamic;
	ifstream iElectFile;
	ofstream result;

public :

	election(char* s, bool cap = false):
		capacitated(cap),
		iElectFile(s),
		optValCap(-1),
		optValUnCap(-1),
		val_ip(0),
		val_lp(0),
		val_colgen(0),
		val_dynamic(0),
		result("solution_file.dat")
		
	{};




	election(int n, int m, int k , bool cap = false):
		nVoters(n), nCandidates(m), k(k), capacitated(cap),
		result("solution_file.dat")
 	 {
		rank_score = new int *[nCandidates];
		for (int i = 0; i < nCandidates; i++)
			rank_score[i] = new int[nVoters];
		optValCap = optValUnCap = -1;
		val_ip = 0;
		val_lp = 0;
		val_colgen = 0;
		val_dynamic = 0;

  	 }
	//election(const election& E) {};
	~election();

	int Candidates() const {return nCandidates;}
	int Voters() const {return nVoters;}
	int K() const {return k;}
	int** Ranking() const {return rank_score;}
	double OptValCap() const  {return optValCap;}
	double GetValIP() const {return val_ip;}
	double GetValLP() const {return val_lp;}
	double GetValColgen() const {return val_colgen;}
	double GetValDynamic() const {return val_dynamic;}
	double OptValUnCap() const {return optValUnCap ;}
	bool IsCapacitated() {return capacitated;}
	void SetOptValCap(double v) { optValCap = v;}
	void SetOptValUnCap(double v) { optValUnCap = v;}
	void SetCandidates(int m) { nCandidates = m;}
	void SetVoters(int n) {nVoters = n;}
	void SetK(int d) {k = d;}
	void SetValIP(double v) {val_ip = v;}
	void SetValLP(double v) {val_lp = v;}
	void SetValColgen(double v) {val_colgen = v;}
	void SetValDynamic(double v){val_dynamic = v;}
	ofstream& eFile() {return result;}
	void ResultFileClose() {result.close();}

	void Generate_Rank_General_Election();
	void Read_Rank_From_File_General_Election();

	void Generate_Rank_SinglePeaked_Election();
	void Read_Rank_From_File_SinglePeaked_Election();
	void Generate_Rank_SinglePeaked_Approval_Election();
	void Convert_Ranks_To_Scores();
	void Generate_Rank_SinglePeaked_3_Weights();

	void PrintRankingToFile();


	class ColGenSolver;
	friend class ColgenSolver;

	class ILPSolver;
	friend class ILPSolver;

	class DynamicSolver;
	friend class DynamicSolver;

};

ostream& operator <<(ostream& out, const election& e);

void CreateSinglePeakPermMatrix(int a[4][3]);
void Create_3_Weights_Rank(int rank[], int nCandidates); 




#endif /* ELECTION_CLASS_H_ */
