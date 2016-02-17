/*
 * experiments.cpp
 *
 *
 *
 */

#include "experiments.h"


void Compare_Gap_Triple_Old(int n)
{



	double max_lp_triple_gap, max_lp_old_gap = -1;
	int index_triple, index_old_lp = -1;
	ofstream file("result_triple_vs_old.txt");
	srand(time(NULL));
	for (int i = 0 ; i < n ; i ++)
	{
		file << endl<< "******* Instance #" << i << " *******"<< endl;
		Ranking kem_rank(5,10 );
		kem_rank.Generate_Kemeny_Ranking();
		file << "Ranking Input is :" << endl << kem_rank << endl;
		kem_rank.PreFunctionsCall();

		TripleFormulKemeny triplekem(kem_rank);
		triplekem.SolveILP();

		file << endl;
		file << " Triple Formulation " << endl;
		file << "The IP solution is : " << triplekem.GetValIP() << endl;
		file << "The LP solution is :" << triplekem.GetValLP() << endl;
		file <<  "The gap is :" << triplekem.GetValIP()/triplekem.GetValLP() << endl;
		if ( triplekem.GetValIP()/triplekem.GetValLP() > max_lp_triple_gap )
		{
			max_lp_triple_gap =  triplekem.GetValIP()/triplekem.GetValLP();
			index_triple = i;
		}	


		OldILPSolver old_ilp(kem_rank);
		old_ilp.SolveILP();

		file << endl;
		file << " Old Formulation " << endl;
		file << "The IP solution is : " << old_ilp.GetValIP() << endl;
		file << "The LP solution is :" << old_ilp.GetValLP() << endl;
		file <<  "The gap is :" << old_ilp.GetValIP()/old_ilp.GetValLP() << endl;
		if ( old_ilp.GetValIP()/old_ilp.GetValLP() > max_lp_old_gap )
		{
			max_lp_old_gap =  old_ilp.GetValIP()/old_ilp.GetValLP();
			index_old_lp = i;
		}
	}
	file << endl << "**************************** The results ********** " << endl;
	file << "The worst LP gap found for Triple Formulation is : " << max_lp_triple_gap << " in instance :" << index_triple << endl;
	file <<"The worst LP gap found for old Formulation is : " << max_lp_old_gap << " in instance :" << index_old_lp << endl;

	file.close();
}


void Test_Triple_Old()

{

	srand(time(NULL));
	Ranking kem_rank(20, 9);
	kem_rank.Generate_Kemeny_Ranking();
	cout << kem_rank << endl;
	kem_rank.PreFunctionsCall();
	//kem_rank.Print_m_Matrix();
	//kem_rank.PrintDiffMatrix();
	TripleFormulKemeny triplekem(kem_rank);
	triplekem.SolveILP();

	//NewILPSolver new_ilp(kem_rank);
	//new_ilp.SolveILP();
	cout << "The next solver results are : " << endl;

	OldILPSolver old_ilp(kem_rank);
	old_ilp.SolveILP();

	//cout << "The next lp splver is : " << endl;
	//OldLPSolver old_lp(kem_rank);
	//old_lp.SolveLPIteratively(); 
	//cout << endl << "The next LP solver, no iterated one is : " << endl;
	//old_lp.SolveLP();
	cout << "*************************************************************"<< endl;
	cout << endl << endl << "The results are : " << endl;
	cout << "The Old Formulation IP Solution is : "<<  old_ilp.GetValIP()<< endl;
	cout << "The Old Formulation LP Solution is : "<<  old_ilp.GetValLP()<< endl;
	cout << "The triple Formulation IP Solution is : "<<  triplekem.GetValIP()<< endl;
	cout << "The triple Formulation LP Solution is : "<<  triplekem.GetValLP()<< endl;
	cout << "*************************************************************"<< endl;

	cout << "This is the end " << endl;


}




void Test_Triple_Old_Input_File()

{


	Ranking kem_rank;
	kem_rank.Read_Rank_From_File_Kemeny();
	cout << kem_rank << endl;
	kem_rank.PreFunctionsCall();
	//kem_rank.Print_m_Matrix();
	//kem_rank.PrintDiffMatrix();
	TripleFormulKemeny triplekem(kem_rank);
	triplekem.SolveILP();

	//NewILPSolver new_ilp(kem_rank);
	//new_ilp.SolveILP();
	cout << "The next solver results are : " << endl;

	OldILPSolver old_ilp(kem_rank);
	old_ilp.SolveILP();

	//cout << "The next lp splver is : " << endl;
	//OldLPSolver old_lp(kem_rank);
	//old_lp.SolveLPIteratively(); 
	//cout << endl << "The next LP solver, no iterated one is : " << endl;
	//old_lp.SolveLP();
	cout << "*************************************************************"<< endl;
	cout << endl << endl << "The results are : " << endl;
	cout << "The Old Formulation IP Solution is : "<<  old_ilp.GetValIP()<< endl;
	cout << "The Old Formulation LP Solution is : "<<  old_ilp.GetValLP()<< endl;
	cout << "The triple Formulation IP Solution is : "<<  triplekem.GetValIP()<< endl;
	cout << "The triple Formulation LP Solution is : "<<  triplekem.GetValLP()<< endl;
	cout << "*************************************************************"<< endl;

	cout << "This is the end " << endl;


}

void Test_AllTripleFormul()
{

	srand(time(NULL));
	Ranking kem_rank(3,4);
	kem_rank.Generate_Kemeny_Ranking();
	cout << kem_rank << endl;
	kem_rank.Print_m_Matrix();
	
	AllTripleFormulKemeny all_triple_solver(kem_rank);
	all_triple_solver.InitialTripleCostCube();
	all_triple_solver.ComputeTripleCost();
	all_triple_solver.PrintTripleCostCube();
	all_triple_solver.SolveILP();



//	TripleFormulKemeny triplekem(kem_rank);
//	triplekem.SolveILP();




	OldILPSolver old_ilp(kem_rank);
	old_ilp.SolveILP();

	cout << "*************************************************************"<< endl;
	cout << endl << endl << "The results are : " << endl;
	cout << "The Old Formulation IP Solution is : "<<  old_ilp.GetValIP()<< endl;
	cout << "The Old Formulation LP Solution is : "<<  old_ilp.GetValLP()<< endl;
	cout << "The all triple Formulation IP Solution is : "<<  all_triple_solver.GetValIP()<< endl;
	cout << "The all triple Formulation LP Solution is : "<<  all_triple_solver.GetValLP()<< endl;
//	cout << "The half triple Formulation IP Solution is : "<<  triplekem.GetValIP()<< endl;
//	cout << "The half triple Formulation LP Solution is : "<<  triplekem.GetValLP()<< endl;
	cout << "*************************************************************"<< endl;
	old_ilp.PrintResultsIP();
	all_triple_solver.PrintResults();

	cout << "This is the end " << endl;

}



void Test_AllTripleFormul(int n)
{
	ofstream file("test_alltriple.txt");
	srand(time(NULL));
	//	cout << kem_rank << endl;
	for (int i = 0; i < n; i++)
	{
		Ranking kem_rank(5,10);
		kem_rank.Generate_Kemeny_Ranking();
		file << "************* instance # " << i << " *****************"<< endl;
		file << kem_rank << endl;
		kem_rank.PreFunctionsCall();


		TripleFormulKemeny triplekem(kem_rank);
		triplekem.SolveILP();


		AllTripleFormulKemeny all_triple_solver(kem_rank);
		all_triple_solver.InitialTripleCostCube();
		all_triple_solver.ComputeTripleCost();
		//	all_triple_solver.PrintTripleCostCube();
		all_triple_solver.SolveILP();


		OldILPSolver old_ilp(kem_rank);
		old_ilp.SolveILP();

		file << "*************************************************************"<< endl;
		file << endl << endl << "The results are : " << endl;
		file << "The Old Formulation IP Solution is : "<<  old_ilp.GetValIP()<< endl;
		file << "The Old Formulation LP Solution is : "<<  old_ilp.GetValLP()<< endl;
		file << "The all triple Formulation IP Solution is : "<<  all_triple_solver.GetValIP()<< endl;
		file << "The all triple Formulation LP Solution is : "<<  all_triple_solver.GetValLP()<< endl;
		file << "The IP ratio is : " << all_triple_solver.GetValIP()/old_ilp.GetValIP() << endl;
		file << "The IP pair/IP triple ratio = "<< all_triple_solver.GetValIP()/old_ilp.GetValIP() << endl;
		file << "The LP ration is : " << all_triple_solver.GetValLP()/old_ilp.GetValLP() << endl;
		file << "The half triple Formulation IP Solution is : "<<  triplekem.GetValIP()<< endl;
		file << "The half triple Formulation LP Solution is : "<<  triplekem.GetValLP()<< endl;
		//	triplekem.Test_Triple_Violation(file);
		file << "*************************************************************"<< endl;

		file << "************************ End of Instance **************************" << endl;
	}

	cout << "This is the end " << endl;

}


/***********************************************/
void Test_AllTripleFormul_Input_File()
{


	Ranking kem_rank;
	kem_rank.Initiate_m_Matrix_From_File();
	//kem_rank.PreFunctionsCall();


//	TripleFormulKemeny triplekem(kem_rank);
//	triplekem.SolveILP();


	//kem_rank.Print_m_Matrix();
	//kem_rank.PrintDiffMatrix();
	AllTripleFormulKemeny all_triple_solver(kem_rank);
	all_triple_solver.InitialTripleCostCube();
	all_triple_solver.ComputeTripleCost();
	//all_triple_solver.PrintTripleCostCube();
	all_triple_solver.SolveILP();


	//NewILPSolver new_ilp(kem_rank);
	//new_ilp.SolveILP();
	cout << "The next solver results are : " << endl;

//	OldILPSolver old_ilp(kem_rank);
//	old_ilp.SolveILP();

	//cout << "The next lp splver is : " << endl;
	//OldLPSolver old_lp(kem_rank);
	//old_lp.SolveLPIteratively(); 
	//cout << endl << "The next LP solver, no iterated one is : " << endl;
	//old_lp.SolveLP();
	cout << "*************************************************************"<< endl;
	cout << endl << endl << "The results are : " << endl;
//	cout << "The Old Formulation IP Solution is : "<<  old_ilp.GetValIP()<< endl;
//	cout << "The Old Formulation LP Solution is : "<<  old_ilp.GetValLP()<< endl;
	cout << "The all triple Formulation IP Solution is : "<<  all_triple_solver.GetValIP()<< endl;
	cout << "The all triple Formulation LP Solution is : "<<  all_triple_solver.GetValLP()<< endl;
//	cout << "The half triple Formulation IP Solution is : "<<  triplekem.GetValIP()<< endl;
//	cout << "The half triple Formulation LP Solution is : "<<  triplekem.GetValLP()<< endl;
	cout << "*************************************************************"<< endl;
	//	triplekem.Test_Triple_Violation();
	cout << "This is the end " << endl;




}
/*
void Test_All_Formulation(int n)
{
	for (int i = 1; i <=n; i++)
	{
		//prepare the index of the  input file
		stringstream ss;
		ss << i;
		string name("../data/kem_instance_");
		name += ss.str();
		name += ".txt";
		const char* n = name.c_str();
		ifstream file("in.txt");

		//creat the ranking isntance based on the input file
		Ranking kem_rank(file);
		kem_rank.Read_Rank_From_File_Kemeny();
		cout << kem_rank << endl;
		kem_rank.PreFunctionsCall();


		file << "************* instance # " << i << " *****************"<< endl;
		file << kem_rank << endl;
		kem_rank.PreFunctionsCall();


		TripleFormulKemeny triplekem(kem_rank);
		triplekem.SolveILP();


		AllTripleFormulKemeny all_triple_solver(kem_rank);
		all_triple_solver.InitialTripleCostCube();
		all_triple_solver.ComputeTripleCost();
		//	all_triple_solver.PrintTripleCostCube();
		all_triple_solver.SolveILP();


		OldILPSolver old_ilp(kem_rank);
		old_ilp.SolveILP();

		file << "*************************************************************"<< endl;
		file << endl << endl << "The results are : " << endl;
		file << "The Old Formulation IP Solution is : "<<  old_ilp.GetValIP()<< endl;
		file << "The Old Formulation LP Solution is : "<<  old_ilp.GetValLP()<< endl;
		file << "The all triple Formulation IP Solution is : "<<  all_triple_solver.GetValIP()<< endl;
		file << "The all triple Formulation LP Solution is : "<<  all_triple_solver.GetValLP()<< endl;
		file << "The IP ratio is : " << all_triple_solver.GetValIP()/old_ilp.GetValIP() << endl;
		file << "The IP pair/IP triple ratio = "<< all_triple_solver.GetValIP()/old_ilp.GetValIP() << endl;
		file << "The LP ration is : " << all_triple_solver.GetValLP()/old_ilp.GetValLP() << endl;
		file << "The half triple Formulation IP Solution is : "<<  triplekem.GetValIP()<< endl;
		file << "The half triple Formulation LP Solution is : "<<  triplekem.GetValLP()<< endl;
		//	triplekem.Test_Triple_Violation(file);
		file << "*************************************************************"<< endl;

		file << "************************ End of Instance **************************" << endl;

		file.close();
	}

	cout << "This is the end " << endl;



}
*/



void Compare_Lps_Run_time_Classic_Model(char* s)
{
	Ranking kem_rank(s);
	kem_rank.Initiate_m_Matrix_From_File();

	OldILPSolver old_ilp(kem_rank,"lp");
	old_ilp.SolveLPIterat();
//	old_ilp.TestLPIterat();
//	old_ilp.SolveLP();

	//OldILPSolver ilp (kem_rank);
	//ilp.SolveILP();

	cout << "*************************************************************"<< endl;
	cout << endl << endl << "The results are : " << endl;
	cout << "The Iterated LP Solution is : "<<  old_ilp.GetValLPIterat()<< endl;
	cout << "The running time of the Iterated LP is : " << old_ilp.GetRTLPIterat() <<" milisecond" << endl;
	cout << "The number of constraints added in Iterated LP is : " << old_ilp.GetnConstraintLPIterat() << endl<<endl;
	cout << "The LP Solution is : "<<  old_ilp.GetValLP()<< endl;
	cout << "The running time of the Iterated LP is : " << old_ilp.GetRTLP() <<" milisecond" << endl;
	cout << "The number of constraints in LP is : " << old_ilp.GetnConstraintLP() << endl<<endl;
//	cout << "The  IP Solution is : "<<  ilp.GetValIP()<< endl;
	cout << "*************************************************************"<< endl;
//	ilp.PrintResults();  
	
	cout << "This is the end " << endl;


}


void Test_Target_Cuts(char* s)
{
	//Ranking kem_rank;
	Ranking kem_rank(s); 
	kem_rank.Initiate_m_Matrix_From_File();
	OldILPSolver old_lp(kem_rank,"lp");
	old_lp.SolveLP();
	//old_lp.CreateInitialSolution();
	//cout << "The LP Solution is : "<<  old_lp.GetValLP()<< endl;
//	old_lp.IterativeMIPHeuristicMIPStart();
   //old_lp.LPRoundingHeuristic();
   //old_lp.IterativeSequenceFixingHeuristic(4);
   //old_lp.IterativeLPVariableFixingHeuristic_2(4);  
	//OldILPSolver ilp(kem_rank);
	//ilp.SolveILP();

	cout << "*************************************************************"<< endl;
	cout << "The LP Solution is : "<<  old_lp.GetValLP()<< endl;
	//cout << "The running time of the Iterated LP is : " << old_lp.GetRTLP() <<" milisecond" << endl;
	//cout << "The number of constraints in LP is : " << old_lp.GetnConstraintLP() << endl<<endl;
	//cout << "The  IP Solution is : "<<  ilp.GetValIP()<< endl;
    //ilp.PrintResults();
	cout << "*************************************************************"<< endl;
	cout << "This is the end " << endl;


}


void Mixed_Integer_Heuristic(char* s)
{
	cout << "start the experiment" << endl;
	//Ranking kem_rank;
	Ranking kem_rank(s); 
	kem_rank.Initiate_m_Matrix_From_File();

	HeurSolver heur(kem_rank);
	//old_lp.SolveLP();
//	old_lp.CreateInitialSolution();
	//cout << "The LP Solution is : "<<  old_lp.GetValLP()<< endl;
//	heur.MIP_Heuristic();
//	heur.KernelSearch();
//	heur.MIP_Heuristic_150();
//	heur.MIP_Heuristic_200();
//	heur.MIP_Heuristic_500();
//	heur.SolveLPDual();
//	heur.SolveLP();
	heur.SolvePrimalDual();
//	heur.SolveLPIterat();
//	heur.SolveILP();
//	heur.LPHeuristic();
   //old_lp.LPRoundingHeuristic();
   //old_lp.IterativeSequenceFixingHeuristic(4);
   //old_lp.IterativeLPVariableFixingHeuristic_2(4);  
	//OldILPSolver ilp(kem_rank);
	//ilp.SolveILP();

	cout << "*************************************************************"<< endl;
	//cout << "The LP Solution is : "<<  old_lp.GetValLP()<< endl;
	//cout << "The running time of the Iterated LP is : " << old_lp.GetRTLP() <<" milisecond" << endl;
	//cout << "The number of constraints in LP is : " << old_lp.GetnConstraintLP() << endl<<endl;
//	cout << "The  IP Solution is : "<<  ilp.GetValIP()<< endl;
    //ilp.PrintResults();
	cout << "*************************************************************"<< endl;
	cout << "This is the end " << endl;


}



void Compare_Lps_Run_time_All_Triple_Model()
/* This function compare the runing time two different LP
 * in all triple formulation, first one is to throw all the constraints
 * in when creating the model, the next one is to add constraints iteratively
 */
{
	srand(time(NULL));
	Ranking kem_rank(10,30);
	kem_rank.Generate_Kemeny_Ranking();
	cout << kem_rank << endl;


	AllTripleFormulKemeny alltrip_lp(kem_rank,"lp");
	alltrip_lp.InitialTripleCostCube();
	alltrip_lp.ComputeTripleCost();
	alltrip_lp.SolveLP();

	AllTripleFormulKemeny alltrip_ip (kem_rank);
	alltrip_ip.InitialTripleCostCube();
	alltrip_ip.ComputeTripleCost();
	alltrip_ip.SolveILP();

	cout << "*************************************************************"<< endl;
	cout << endl << endl << "The results are : " << endl;
	cout << "The Old Formulation LP Solution is : "<<  alltrip_lp.GetValLP()<< endl;
	cout << "The Old Formulation IP Solution is : "<<  alltrip_ip.GetValIP()<< endl;
	cout << "*************************************************************"<< endl;
	cout << "This is the end " << endl;

}

