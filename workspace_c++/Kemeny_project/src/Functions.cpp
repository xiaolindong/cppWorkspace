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
		//	myfile.close();
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




void RadixSortOnCandidates(ofstream& output, int * voters, const Ranking& e)
{
	int temp_rank[e.Candidates()][e.Voters()];

	//copy rank_score
	for (int i = 0; i < e.Candidates(); i++)
		for (int j = 0; j < e.Voters(); j++)
			temp_rank[i][j] = e.Rank()[i][j];


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
			output << setw(5)<< left << e.Rank() << " ";
		output << endl ;
	}



}



ostream& operator <<(ostream& out, const vector<dicycle> s)
{
	int counter = 0;
	vector<dicycle>::const_iterator i;
	for(i = s.begin(); i != s.end(); i++){
		out << " ( "<< i->i << " , " << i->j << " , "  << i->k << " ) | ";  // increase by 1 in order to match the convention indexing scheme
	}
	return out;
}



ostream& operator <<(ostream& out, const vector<pair<int,int> > s)
{
	int counter = 0;
	vector<pair<int,int> >::const_iterator i;
	for(i = s.begin(); i != s.end(); i++){
		out << " ( "<< i->first << " , " << i->second << " ) | ";  // increase by 1 in order to match the convention indexing scheme
	}
	return out;
}




ostream& operator <<(ostream& out, const vector<double> s)
{
	int counter = 0;
	vector<double>::const_iterator i;
	for(i = s.begin(); i != s.end(); i++){
		out << *i << " ";  // increase by 1 in order to match the convention indexing scheme
	}
	return out;
}


double diffclock(clock_t clock1,clock_t clock2)
{
	double diffticks=clock1-clock2;
	double diffms=(diffticks)/(CLOCKS_PER_SEC/1000);
	return diffms;
}


ostream& operator <<(ostream& out, const vector<int> s)
{
	int counter = 0;
	vector<int>::const_iterator i;
	for(i = s.begin(); i != s.end(); i++){
		if ((counter++)%30 == 0)
			out << endl;
		out << *i << " ";  // increase by 1 in order to match the convention indexing scheme
	}
	out << endl;
	return out;
}

/*
ostream& operator <<(ostream& out, const vector<double> s)
{
	int counter = 0;
	vector<int>::const_iterator i;
	for(i = s.begin(); i != s.end(); i++){
		if ((counter++)%30 == 0)
			out << endl;
		out << *i << " ";  // increase by 1 in order to match the convention indexing scheme
	}
	out << endl;
	return out;
}

*/



ostream& operator <<(ostream& out,  vector<vector<int> > s)
{
	vector<vector<int> >::iterator i;
	for(i = s.begin(); i != s.end(); i++)
	{
		vector<int>::iterator j;
		for(j = i->begin(); j != i->end(); j++)
		{
			out<< *j << " ";
		}
		out << endl;
	}
	return out;
}

/*
ostream& operator<<(ostream& out, vector<pair<int,int> >& p)
{				
	out << "the pair fixed vector is :" << endl;
	vector<pair<int, int> >::iterator it_p;
	for(it_p = p.begin(); it_p != p.end(); it_p++)
		out << "i =  " << (*it_p).first << " ,  j = " << (*it_p).second << endl;
	return out; 
}
*/


ostream& operator <<(ostream& out,  vector<vector<double> > s)
{
	vector<vector<double> >::iterator i;
	for(i = s.begin(); i != s.end(); i++)
	{
		vector<double>::iterator j;
		for(j = i->begin(); j != i->end(); j++)
		{
			out<< *j << " ";
		}
		out << endl;
	}
	return out;
}


ostream& operator <<(ostream& out,  vector<vector<vector<int> > > s)
{
	out << "initial points are :" << endl;
	vector<vector<vector<int> > >::iterator k;
	for(k = s.begin(); k != s.end(); k++)
	{
		vector<vector<int> >::iterator i;
		for(i = k->begin(); i != k->end(); i++)
		{
			vector<int>::iterator j;
			for(j= i->begin(); j != i->end(); j++)
				out<< *j << " ";
			out<< endl;
		}
		out << endl << endl;
	}
	out << endl;
	return out;
}


vector<vector<int> > OrderToMatrix(vector<int> order){
	int size = order.size();
	//creat  the ranking matrix
	vector<vector<int> > matrix;
	matrix.resize(size);
	for(int i =0; i < size; i++)
		matrix[i].resize(size);
	for(int i =0; i < size; i++)
		for(int j =0; j < size; j++)
			matrix[i][j] = 0;
	//initiate the matrix
	for(int i = 0 ; i < size; i++)
		for(int j = i+1; j < size; j++)
			matrix[order[i]][order[j]] = 1;
	return matrix;
}

ostream& operator <<(ostream& out, const TS t)
{
	out << "orders value : " << t.a; 
	out << t.v << endl;
}

ostream& operator <<(ostream& out, const multiset<TS> t)
{
	multiset<TS>::const_iterator it;
	for(it = t.begin(); it != t.end(); it++)
		out << *it << endl;
	return out;
}

vector<int> MatrixToOrdering(vector<vector<double> >& matrix, int dim ){
	vector<int> ordering,row_sum;
	ordering.resize(dim);
	row_sum.resize(dim);
	for(int i = 0; i < dim ; i++){
		ordering[i] = i;
		row_sum[i] = 0;
	}
	for(int i = 0; i < dim; i++)
	{
		int sum =0;
		for(int j = 0; j < dim ; j++)
			sum+= matrix[i][j];
		row_sum[i] = sum;
	}
	InsertionSort(row_sum,ordering);
	return ordering;
}

vector<int> MatrixToOrdering(vector<vector<int> >& matrix, int dim ){
	vector<int> ordering,row_sum;
	ordering.resize(dim);
	row_sum.resize(dim);
	for(int i = 0; i < dim ; i++){
		ordering[i] = i;
		row_sum[i] = 0;
	}
	for(int i = 0; i < dim; i++)
	{
		int sum =0;
		for(int j = 0; j < dim ; j++)
			sum+= matrix[i][j];
		row_sum[i] = sum;
	}
	InsertionSort(row_sum,ordering);
	return ordering;
}
vector<int> MatrixToOrdering(vector<vector<double> >& matrix, vector<int>& scores,int dim ){
	vector<int> ordering,row_sum;
	ordering.resize(dim);
	row_sum.resize(dim);
	for(int i = 0; i < dim ; i++){
		ordering[i] = i;
		row_sum[i] = 0;
	}
	for(int i = 0; i < dim; i++)
	{
		int sum =0;
		for(int j = 0; j < dim ; j++)
			sum+= matrix[i][j];
		row_sum[i] = sum;
	}
	InsertionSort(row_sum,ordering);
	scores = row_sum;
	return ordering;
}


void InsertionSort(vector<int>& num, vector<int>& index){
	int i, j, key_index, key, numLength = num.size();
	for(j = 1; j < numLength; j++){    // Start with 1 (not 0)
		key = num[j];
		key_index =index[j];
		for(i = j - 1; (i >= 0) && (num[i] < key); i--){  // Smaller values move u
			num[i+1] = num[i];
			index[i+1] = index[i];
		}
		num[i+1] = key;    //Put key into its proper location
		index[i+1] = key_index;
	}
}


int FindPosition(int* a, int elem,int size)
{
	for(int i =0; i < size; i++)
		if (a[i] == elem)
			return i;
	return -1;
}

int FindPositionVector(vector<int>& a, int elem,int size)
{
	//  cout << "elem is " << elem << endl;
	// cout << "vector in Findpos is :" << a << endl;
	for(int i =0; i < size; i++)
		if (a[i] == elem)
			return i;
	// cout << "  -1  " << endl;
	return -1;
}



vector<int> ObjectPosition(vector<int>& order){
	int size = order.size();
	vector<int> pos(size);
	for(int i = 0; i < size; i++)
		pos[order[i]] = i;
	return pos;
}


