//============================================================================
// Name        : column_generation_voting.cpp
// Author      : Ehsan
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
// -------------------------------------------------------------- -*- C++ -*-
// File: cutstock.cpp
// Version 12.2
// --------------------------------------------------------------------------
// Licensed Materials - Property of IBM
// 5725-A06 5725-A29 5724-Y48 5724-Y49 5724-Y54 5724-Y55
// Copyright IBM Corporation 2000, 2010. All Rights Reserved.
//
// US Government Users Restricted Rights - Use, duplication or
// disclosure restricted by GSA ADP Schedule Contract with
// IBM Corp.
// --------------------------------------------------------------------------
//

#include "election_class.h"
#include "colgen.h"
#include "ilp_solver.h"
#include "dynamic_solver.h"
#include "experiments.h"


using namespace std;

int main(int arg, char* argv[])
{

	//Test_Single_Peakedness();
	//Test_Single_Peakedness(40000);
	//Test_Single_Peakedness_Input_File();
	//Test_Single_Peak_3_Weights();
	TestHeuristic(argv[1]);
	//Test_Column_Generation(argv[1]);
	//Test_Column_Generation();
	//Compare_Gap_Colgen_LP(2000);

	//Test_ILP(100);

	//Test_CFL_ColGen(100);
	//Test_CFL_ColGen_Input_File();


	return 0;
}
