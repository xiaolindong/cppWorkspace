/*
 * Functions.h
 *
 *  Created on: Jan 5, 2014
 *      Author: ehsan
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include "master_header.h"
#include "ranking.h"

int STLConvert(string strIntString );
void Tokenize(const string& str,vector<string>& tokens);
string intToString(int x);
vector<string> ConvertRankToVector(ifstream& myfile);
int fact(int n);
void PermLexUnrank(int *a, int n , int r);
void ConverRanksArrayToScoreArray(int rank[], int nCandidates);
int nK_combination(int n, int k);
void K_Subset_Lex_Unrank(vector<vector<int> >& subsets, int r, int m, int k);
bool BelongToArray(int a[], int size,int r);
void sort( double a[], int size);
void sort(double a[], int b[], int size);
void RadixSortOnCandidates(ofstream& output, int * voters);
double diffclock(clock_t clock1,clock_t clock2);
ostream& operator <<(ostream& out, const vector<dicycle> s);
ostream& operator <<(ostream& out, const vector<pair<int,int> > s);
ostream& operator <<(ostream& out, const vector<double> s);
ostream& operator <<(ostream& out, const vector<int> s);
ostream& operator <<(ostream& out,  vector<vector<int> > s);
ostream& operator <<(ostream& out,  vector<vector<double> > s);
ostream& operator <<(ostream& out,  vector<vector<vector<int> > > s);
ostream& operator <<(ostream& out, const TS t);
ostream& operator <<(ostream& out, const multiset<TS> t);
//ostream& operator<<(ostream& out, vector<pair<int,int> >& p);
vector<vector<int> > OrderToMatrix(vector<int> order);
void InsertionSort(vector<int>& num, vector<int>& index);
vector<int> MatrixToOrdering(vector<vector<int> >& matrix, int dim );
vector<int> MatrixToOrdering(vector<vector<double> >& matrix, int dim );
vector<int> MatrixToOrdering(vector<vector<double> >& matrix, vector<int>& scores, int dim );
int FindPosition(int* a, int elem, int size);
int FindPositionVector(vector<int>& a, int elem, int size);
vector<int> ObjectPosition(vector<int>& order);
#endif /* FUNCTIONS_H_ */
