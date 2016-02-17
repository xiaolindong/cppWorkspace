//============================================================================
// Name        : Kemeny_project.cpp
// Author      : Ehsan
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================



#include "experiments.h"

class tt
{
	ifstream f;
public:

	tt(ifstream& file ): f("input.txt"){};

};




int main(int arg, char* argv[])
{	
	cout << "arg = "  << arg << endl;
//	Compare_Gap_Triple_Old(100);
//	Test_Triple_Old();
//	Test_Triple_Old_Input_File();
//	Test_AllTripleFormul();
//	Test_AllTripleFormul( 100);
//	Test_AllTripleFormul_Input_File();
// 	Test_All_Formulation(10);
//	Compare_Lps_Run_time_Classic_Model(argv[1]);
//	Test_Target_Cuts(argv[1]);
	Mixed_Integer_Heuristic(argv[1]);
//	Compare_Lps_Run_time_All_Triple_Model();


	return 0;
}  
