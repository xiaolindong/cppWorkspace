/*
 *experiments.h
 *
 *
 *
 */


#include"ranking.h"
#include "new_ilp_solver.h"
#include "old_ilp_solver.h"
#include "old_lp_solver.h"
#include "triple_solver.h"
#include "all_triple_formul_solver.h"
#include "heuristic_solver.h"

void Compare_Gap_Triple_Old(int n);
void Test_Triple_Old();
void Test_Triple_Old_Input_File();
void Test_AllTripleFormul();
void Test_AllTripleFormul(int );
void Test_AllTripleFormul_Input_File();
void Test_All_Formulation(int n);
void Compare_Lps_Run_time_Classic_Model(char* s);
void Compare_Lps_Run_time_All_Triple_Model();
void Test_Target_Cuts(char* s);
void Mixed_Integer_Heuristic(char* s );
