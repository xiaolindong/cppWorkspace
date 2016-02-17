/*
 * Functions.h
 *
 *  Created on: Jan 5, 2014
 *      Author: ehsan
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include "master_header.h"
#include "election_class.h"

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
ostream& operator <<(ostream& out,  const vector<int> s);




#endif /* FUNCTIONS_H_ */
