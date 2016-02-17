/*
 *
 *
 *experiments.cpp
 *
 *
 */

#include "experiments.h"
/*

void Test_ILP(int n)
{	
	double max_lp_gap =-1;
	int index_lp = -1;	
	ofstream file("ILP.txt");
	srand(time(0));
	for (int i = 0; i < n ; i++ )
	{
		file << "**** Instance #" << i << " ********" << endl;
		election e(9,6,3);
		e.eFile() << "********** Instance " << i << " ***********" << endl;	
		e.Generate_Rank_General_Election();
		file << "Ranking input is :" << endl << e << endl;

		ILPSolver ilpsolver(e);
		ilpsolver.SolveILP();

		file << endl;
		file << "The IP solution is : " << e.GetValIP() << endl;
		file << "The LP solution is : " << e. GetValLP() << endl;
		file << "The Integrality Gap for Regular LP is " << e.GetValIP()/e.GetValLP() << endl;
		if (e.GetValIP()/e.GetValLP() > max_lp_gap)
		{
			max_lp_gap = e.GetValIP()/e.GetValLP();
			index_lp = i;
		}

		e.ResultFileClose();
	}
	file << endl << "********* The overal results ******** " << endl;
	file << "The worst LP gap found is : " << max_lp_gap << " and it occurs at instance : " << index_lp << endl;
	file.close();

}





void Test_Single_Peak_3_Weights()

{
	srand(time(NULL));
	election e (6,4,2,true);
	e.Generate_Rank_SinglePeaked_3_Weights(); 
	cout << e << endl;
	DynamicSolver dynsolver(e);
	dynsolver.LexicoSortOnVoters();
	dynsolver.FillTable();
	cout << "dynamic table is : " << endl;
	cout << dynsolver ;
	cout << endl;
	//	dynsolver.PrintOptPath();
	//	dynsolver.ExtractPrintDynamicSolution();
	cout << "hiii" << endl; 
	cout << "optimal dynamic solution is : " << dynsolver.OptimalDynamic()<<  endl;
	dynsolver.PrintOptPath();
	ILPSolver ilp(e);
	ilp.SolveILP();
	ilp.PrintResult();
}


void Test_Single_Peakedness()
{

	srand(time(NULL));
	election e(16,10,4,true);
	e.Generate_Rank_SinglePeaked_Election();
	cout << e << endl;
	DynamicSolver dynsolver(e);
	dynsolver.LexicoSortOnVoters();
	dynsolver.FillTable();
	cout << "dynamic table is : " << endl;
	cout << dynsolver ;
	cout << endl;
	//dynsolver.PrintOptPath();
	//dynsolver.ExtractPrintDynamicSolution();
	cout << "optimal dynamic solution is : " << dynsolver.OptimalDynamic()<<  endl;
	ILPSolver ilp(e);
	ilp.SolveILP();
	//ilp.PrintResult();
	//cout << e << endl;
	cout << "this is the end "<< endl;

}



void Test_Single_Peakedness(int n)
{

	double max_lp_gap, max_dynam_gap = -1;
	int index_lp, index_dynam = -1;
	ofstream file("gap__dynamic.txt");
	srand(time(NULL));
	for (int i = 0; i < n; i++)
	{
		file << "******************* instance # "<< i << " *************" << endl; 
		election e(9,6,3,true);
		e.Generate_Rank_SinglePeaked_Election();
		file <<"Ranking input is : " << endl << e << endl;

		ILPSolver ilp(e);
		ilp.SolveILP();

		file << endl;
		file << "The IP solution is :" << e.GetValIP() << endl;
		file << "The LP solution is " << e.GetValLP() << endl;
		file << "The integrality gap for regular LP is :" << e.GetValIP()/e.GetValLP() << endl;
		if (e.GetValIP()/e.GetValLP() > max_lp_gap)
		{
			max_lp_gap = e.GetValIP()/e.GetValLP();
			index_lp = i;
		}


		DynamicSolver dynsolver(e);
		dynsolver.LexicoSortOnVoters();
		dynsolver.FillTable();
		//cout << "dynamic table is : " << endl;
		//cout << dynsolver ;
		//cout << endl;
		//dynsolver.PrintOptPath();
		//dynsolver.ExtractPrintDynamicSolution();

		file << endl;
		file << "optimal dynamic solution is : " << dynsolver.OptimalDynamic()<<  endl;
		file << "The gap for dynamic is : " << dynsolver.OptimalDynamic()/e.GetValIP() << endl;
		if (dynsolver.OptimalDynamic()/e.GetValIP() > max_dynam_gap && e.GetValIP() != 0)
		{
			max_dynam_gap = dynsolver.OptimalDynamic()/e.GetValIP();
			index_dynam = i;
		}



	}
	file << endl << "********* The overal results ******** " << endl;
	file << "The worst LP gap found is : " << max_lp_gap << " and it occurs at instance : " << index_lp << endl;
	file << "The worst dynamic gap found is : " << max_dynam_gap << " and it occurs at instance : " << index_dynam<< endl;
	file.close();
}




void Test_Single_Peakedness_Input_File()
{

	srand(time(NULL));
	election e(true);
	e.Read_Rank_From_File_General_Election();
	cout<<"Ranking input is : " << endl << e << endl;

	ILPSolver ilp(e);
	ilp.SolveILP();

	DynamicSolver dynsolver(e);
	dynsolver.LexicoSortOnVoters();
	dynsolver.FillTable();
	cout << "optimal dynamic solution is : " << dynsolver.OptimalDynamic()<<  endl;


	//cout << "dynamic table is : " << endl;
	cout << dynsolver ;
	//cout << endl;
	dynsolver.PrintOptPath();
	dynsolver.ExtractPrintDynamicSolution();
	cout << "This is the end!" << endl;
}

*/

void TestHeuristic(char* s)
{
	//srand(time(NULL));
	election e(s);
	e.Read_Rank_From_File_General_Election();
	Heuristic heur(e);
	heur.SolveIP();
	heur.LocalSearch();
	cout << endl << " This is the end!!"<< endl;

}






void Test_Column_Generation(char* s)
{
	srand(time(NULL));
	election e(s);
	e.Read_Rank_From_File_General_Election();
	ILPSolver ilpsolver(e);
	ilpsolver.SolveILP();


	UFLColGenSolver solver(e,10);
	solver.InitSubSets();
	solver.SolveLP();
	cout << solver ;



	cout << endl << "****** The solution results ************" << endl << endl;
	cout << "The IP solutio is : " << e.GetValIP() << endl;
	cout << "The Regular LP solution is : " << e.GetValLP() << endl;
	cout << "The Colgen LP solution is : " << e.GetValColgen() << endl;
	cout << "Regular LP Gap = " << e.GetValIP()/e.GetValLP() << endl;
	cout << "Colgen LP Gap = " << e.GetValIP()/e.GetValColgen() << endl;
	cout << endl << "******* End of Results******************" << endl << endl;
	cout << endl << " This is the end!!"<< endl;


	/*
	   ColGenSolver solver(e,1);
	   solver.InitSubSets();
	   solver.SolveLP();
	   cout << solver ;
	   */

	cout << endl << " This is the end!!"<< endl;

}
/*

void Compare_Gap_Colgen_LP(int n)
{	
	double max_lp_gap, max_colgen_gap = -1;
	int index_lp, index_colgen = -1;	
	ofstream file("ColgenGap.txt");
	srand(time(0));
	for (int i = 0; i < n ; i++ )
	{
		file << "**** Instance #" << i << " ********" << endl;
		election e(24,12,6);
		e.eFile() << "********** Instance " << i << " ***********" << endl;	
		e.Generate_Rank_General_Election();
		file << "Ranking input is :" << endl << e << endl;

		ILPSolver ilpsolver(e);
		ilpsolver.SolveILP();

		file << endl;
		file << "The IP solution is : " << e.GetValIP() << endl;
		file << "The LP solution is : " << e. GetValLP() << endl;
		file << "The Integrality Gap for Regular LP is " << e.GetValIP()/e.GetValLP() << endl;
		if (e.GetValIP()/e.GetValLP() > max_lp_gap)
		{
			max_lp_gap = e.GetValIP()/e.GetValLP();
			index_lp = i;
		}


		UFLColGenSolver solver(e,6);
		solver.InitSubSets();
		double sol = solver.SolveLP();

		file  << "The Colgen Lp sol  is : " << sol << endl;
		file << "The integrality Gap for Colgen is :" << e.GetValIP()/sol << endl;
		file  << endl << "***** End Of Instance ****!!"<< endl;
*/
		//old Column Gneration
		/*ColGenSolver solver(e,6);
		  solver.InitSubSets();
		  double sol = solver.SolveLP();

		  file  << "The Colgen Lp sol  is : " << sol << endl;
		  file << "The integrality Gap for Colgen is :" << e.GetValIP()/sol << endl;
		  file  << endl << "***** End Of Instance ****!!"<< endl;

		  if (e.GetValIP()/sol > max_colgen_gap)
		  {
		  max_colgen_gap = e.GetValIP()/sol;
		  index_colgen = i;
		  }
		  e.ResultFileClose();*/
/*	}
	file << endl << "********* The overal results ******** " << endl;
	file << "The worst LP gap found is : " << max_lp_gap << " and it occurs at instance : " << index_lp << endl;
	file << "The worst colgen gap found is : " << max_colgen_gap << " and it occurs at instance : " << index_colgen << endl; 
	file.close();

}

*/
/*

void Test_Column_Generation()
{
	srand(time(NULL));
	election e(24,12,6);
	e.Generate_Rank_General_Election();
	cout  << e ;

	ILPSolver ilpsolver(e);
	ilpsolver.SolveILP();

	//ColGenSolver solver(e,1);

	UFLColGenSolver solver(e,4);
	solver.InitSubSets();
	cout << solver;
	double sol = solver.SolveLP();
	cout << "The subsets are :" << endl;


	cout << endl << "****** The solution results ************" << endl << endl;
	cout << "The IP solutio is : " << e.GetValIP() << endl;
	cout << "The Regular LP solution is : " << e.GetValLP() << endl;
	cout << "The Colgen LP solution is : " << e.GetValColgen() << endl;
	cout << "Regular LP Gap = " << e.GetValIP()/e.GetValLP() << endl;
	cout << "Colgen LP Gap = " << e.GetValIP()/e.GetValColgen() << endl;
	cout << endl << "******* End of Results******************" << endl << endl;
	cout << endl << " This is the end!!"<< endl;

}





void Test_CFL_ColGen_Input_File()
{
	srand(time(NULL));
	election e(true);
	e.Read_Rank_From_File_General_Election();
	ILPSolver ilpsolver(e);
	ilpsolver.SolveILP();


	CFLColGenSolver solver(e,6);
	solver.InitSubSets();
	cout << solver << endl;
	solver.SolveLP();
	cout << solver ;
*/
	/*	ColGenSolver solver(e,1);
		solver.InitSubSets();
		solver.SolveLP();
		cout << solver ;*/
/*

	cout << endl << " This is the end!!"<< endl;

}




void Test_CFL_ColGen()
{
	srand(time(NULL));
	election e(24,12,6,true);
	e.Generate_Rank_General_Election();
	cout  << e ;

	ILPSolver ilpsolver(e);
	ilpsolver.SolveILP();

	//ColGenSolver solver(e,1);

	CFLColGenSolver solver(e,4);
	solver.InitSubSets();
	cout << solver;
	double sol = solver.SolveLP();
	cout << "The subsets are :" << endl;


	cout << endl << "****** The solution results ************" << endl << endl;
	cout << "The IP solutio is : " << e.GetValIP() << endl;
	cout << "The Regular LP solution is : " << e.GetValLP() << endl;
	cout << "The Colgen LP solution is : " << e.GetValColgen() << endl;
	cout << "Regular LP Gap = " << e.GetValIP()/e.GetValLP() << endl;
	cout << "Colgen LP Gap = " << e.GetValIP()/e.GetValColgen() << endl;
	cout << endl << "******* End of Results******************" << endl << endl;
	cout << endl << " This is the end!!"<< endl;

}




void Test_CFL_ColGen(int n)
{	
	double max_lp_gap, max_colgen_gap = -1;
	int index_lp, index_colgen = -1;	
	ofstream file("ColgenGap.txt");
	srand(time(0));
	for (int i = 0; i < n ; i++ )
	{
		file << "**** Instance #" << i << " ********" << endl;
		election e(7,3,2,true);
		e.eFile() << "********** Instance " << i << " ***********" << endl;	
		e.Generate_Rank_General_Election();
		file << "Ranking input is :" << endl << e << endl;

		ILPSolver ilpsolver(e);
		ilpsolver.SolveILP();

		file << endl;
		file << "The IP solution is : " << e.GetValIP() << endl;
		file << "The LP solution is : " << e. GetValLP() << endl;
		file << "The Integrality Gap for Regular LP is " << e.GetValIP()/e.GetValLP() << endl;
		if (e.GetValIP()/e.GetValLP() > max_lp_gap)
		{
			max_lp_gap = e.GetValIP()/e.GetValLP();
			index_lp = i;
		}


		CFLColGenSolver solver(e,3);
		solver.InitSubSets();
		double sol = solver.SolveLP();

		file  << "The Colgen Lp sol  is : " << sol << endl;
		file << "The integrality Gap for Colgen is :" << e.GetValIP()/sol << endl;
		file  << endl << "***** End Of Instance ****!!"<< endl;
*/
		//old Column Gneration
		/*ColGenSolver solver(e,6);
		  solver.InitSubSets();
		  double sol = solver.SolveLP();

		  file  << "The Colgen Lp sol  is : " << sol << endl;
		  file << "The integrality Gap for Colgen is :" << e.GetValIP()/sol << endl;
		  file  << endl << "***** End Of Instance ****!!"<< endl;

		  if (e.GetValIP()/sol > max_colgen_gap)
		  {
		  max_colgen_gap = e.GetValIP()/sol;
		  index_colgen = i;
		  }
		  e.ResultFileClose();*/
/*	}
	file << endl << "********* The overal results ******** " << endl;
	file << "The worst LP gap found is : " << max_lp_gap << " and it occurs at instance : " << index_lp << endl;
	file << "The worst colgen gap found is : " << max_colgen_gap << " and it occurs at instance : " << index_colgen << endl; 
	file.close();

}

void Test_Branch_Price()
{
	vector<int> winner(3);
	double cur_best_sol = 10000000;
	election e(7,3,2,true);
	e.eFile() << "********** Instance " << i << " ***********" << endl;	
	e.Generate_Rank_General_Election();
	file << "Ranking input is :" << endl << e << endl;

	// call branch and price , the recursive function 
	// branchprice function 
		// check if the current solution is better tham the best one
		// compute the lower bound, lp, colgenlp, ...
		// bound if you can after lower bound computing
		// prepare the branching
		//branch on with calling branchprice
		//branch again

	//print out the results


}
*/
