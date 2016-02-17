/*
 * Functions.cpp
 *
 *  Created on: Jan 14, 2014
 *      Author: eia2
 */

#include "Functions.h"






int STLConvert(string strIntString )
/**************************************/
/* This function converts a string to */
/* an integer                         */
/**************************************/
{
	int iConvertToMe;
	std::stringstream strStream(strIntString);
	strStream >> iConvertToMe;
	return iConvertToMe;
}




void Tokenize(const string& str,vector<string>& tokens)
/**************************************/
/* This function split a string to   */
/**************************************/
{
	string delimiters = " ";
	// Skip delimiters at beginning.
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	string::size_type pos     = str.find_first_of(delimiters, lastPos);
	while (string::npos != pos || string::npos != lastPos)
	{
		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
}





string intToString(int x)
{
	/**************************************/
	/* This function is similar to itoa() */
	/* "integer to alpha", a non-standard */
	/* C language function. It takes an   */
	/* integer as input and as output,    */
	/* returns a C++ string.              */
	/* itoa()  returned a C-string (null- */
	/* terminated)                        */
	/* This function is not needed because*/
	/* the following template function    */
	/* does it all                        */
	/**************************************/
	string r;
	stringstream s;
	s << x;
	r = s.str();
	return r;
}





vector<string> ConvertRankToVector(ifstream& myfile)
/*******************************************************************/
/* read nVoters, nCandidates, k and the ranking matrix from a file */
/*******************************************************************/
{
	string line;
	vector<string> tokens;

	if (myfile.is_open())
	{
		while ( myfile.good() )
		{
			getline (myfile,line);
			Tokenize(line, tokens);
		}
		myfile.close();
	}
	else
		cout << "Unable to open file";

	return tokens;
}






int fact(int n)
{
	if (n == 1)
		return 1;
	return n*fact(n-1);
}

void PermLexUnrank(int *a, int n , int r)
{
	a[n-1] = 1;
	for (int j = 1; j < n; j++)
	{
		int d = (r % fact(j+1))/ fact(j);
		r = r - d*fact(j);
		a[n-j-1] = d+1;
		for (int i = n-j; i < n; i++)
			if (a[i] > d)
				a[i]++;
	}
}





void ConverRanksArrayToScoreArray(int rank[], int nCandidates)
{
	int * t = new int [nCandidates];
	//int t[nCandidates];

	for (int i = 0; i < nCandidates; i++)
		t[rank[i]-1] = i+1;
	for (int i = 0; i < nCandidates; i++)
	{
		rank[i] = t[i];
		cout << t[i] << " ";
	}
	//cout << "Hi" << endl;
	delete [] t;
}



int nK_combination(int n, int k)
{
	if (n == k || k == 0)
		return 1;
	return nK_combination(n-1,k) + nK_combination(n-1, k-1);
}

void K_Subset_Lex_Unrank(vector<vector<int> >& subsets, int r, int m, int k)
{
	int x =1;
	vector<int > temp;
	for (int i = 1; i <= k; i++)
	{
		while(nK_combination(m-x, k-i) <= r)
		{
			r -= nK_combination(m-x,k-i);
			x++;
		}
		temp.push_back(x-1);  // x-1 --> decrease by 1 for consistency with index of the arrays
		//cout << x << " ";
		x++;
	}
	subsets.push_back(temp);
	//temp.~vector();

}

bool BelongToArray(int a[], int size,int r)
{
	for (int i = 0; i <= size; i++)
		if (a[i] == r)
			return true;
	return false;
}



void sort( double a[], int size)
{
     int i, j, key;
     for(j = 1; j < size; j++)    // Start with 1 (not 0)
    {
           key = a[j];
           for(i = j - 1; (i >= 0) && (a[i] < key); i--)   // bigger values move down
          {
                 a[i+1] = a[i];
          }
         a[i+1] = key;    //Put key into its proper location
     }

}

void sort(double a[], int b[], int size)
{
	int i, j, key_a, key_b;
	for(j = 1; j < size; j++)    // Start with 1 (not 0)
	{
		key_a = a[j];
		key_b = b[j];
		for(i = j - 1; (i >= 0) && (a[i] < key_a); i--)   // bigger values move down
		{
			a[i+1] = a[i];
			b[i+1] = b[i];
		}
		a[i+1] = key_a;    //Put key into its proper location
		b[i+1] = key_b;
	}
}




void RadixSortOnCandidates(ofstream& output, int * voters, const election& e)
{
	int temp_rank[e.Candidates()][e.Voters()];

	//copy rank_score
	for (int i = 0; i < e.Candidates(); i++)
		for (int j = 0; j < e.Voters(); j++)
			temp_rank[i][j] = e.Ranking()[i][j];


	//initial voters array index
	for (int i = 0; i < e.Voters(); i++)
		voters[i] = i ;

	//lexigraoghic sort
	for (int d = e.Candidates()-1; d >= 0; d--)
	{
		int i, j, index_key,key[e.Candidates()];
		for(j = 1; j < e.Voters(); j++)    // Start with 1 (not 0)
		{
			for (int t = 0; t < e.Candidates(); t++)
			{
				key[t] = temp_rank[t][j];
			}
			index_key = voters[j];
			for(i = j - 1; (i >= 0) && (temp_rank[d][i] > key[d]); i--)   // Smaller values move down
			{
				for (int t = 0; t < e.Candidates(); t++)
				{
					temp_rank[t][i+1] = temp_rank[t][i];
				}
				voters[i+1] = voters[i];

			}
			for (int t = 0; t < e.Candidates(); t++)
			{
				temp_rank[t][i+1] = key[t];
			}
			voters[i+1] = index_key;

		}
	}
	output <<  "voters lexicographic ordering is : ";
	for (int i = 0; i < e.Voters(); i++)
		output  << voters[i] << " " ;
	output<< endl << endl;


	output << setw(6)<< left << "  " ;
	for (int i = 0; i < e.Voters(); i++)
		output << setw(5)<< left << i << " ";
	output << endl;

	output << setw(6)<< left << "  " ;
	for (int i = 0; i < e.Voters(); i++)
		output << setw(5)<< left << "------";
	output << endl;
	for (int i = 0; i < e.Candidates(); i++)
	{
		output << setw(5)<< left << i << "|";
		for (int j = 0; j < e.Voters(); j++)
			output << setw(5)<< left << temp_rank[i][j] << " ";
		output << endl ;
	}
	output<< endl << endl;
	output << endl <<"rank_score is " << endl;

	output << setw(6)<< left << "  " ;
		for (int i = 0; i < e.Voters(); i++)
			output << setw(5)<< left << i << " ";
		output << endl;

		output << setw(6)<< left << "  " ;
		for (int i = 0; i < e.Voters(); i++)
			output << setw(5)<< left << "------";
		output << endl;
		for (int i = 0; i < e.Candidates(); i++)
		{
			output << setw(5)<< left << i << "|";
			for (int j = 0; j < e.Voters(); j++)
				output << setw(5)<< left << e.Ranking() << " ";
			output << endl ;
		}



}




ostream& operator <<(ostream& out,  const vector<int> s)
{
	vector<int>::const_iterator i;
	for(i = s.begin(); i != s.end(); i++)
		out << *i << " ";  // increase by 1 in order to match the convention indexing scheme
	out << endl;
	return out;
}

