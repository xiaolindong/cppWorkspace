//============================================================================
// Name        : Kemeny_project.cpp
// Author      : Ehsan
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "KemenyFunctions.h"
#include "variables.h"
#include "InputPreparation.h"
#include <ilcplex/ilocplex.h>
#include "node.h"
#include <vector>
#include <set>
#include <list>
#include <map>
#include <stack>
#include <ctime>
#include <math.h>


using namespace std;

int main()
{

	nVoters = 4;
	nCandidates = 8;
	ofstream result("kemeny.txt");
	preparKemenyRanking();
	Kemeny_ILP(result);
	return 0;
}
