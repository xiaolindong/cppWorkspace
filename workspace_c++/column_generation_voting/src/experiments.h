/*
 *
 *
 *
 *experiments.h
 *
 *
 */

#include "master_header.h"
#include "election_class.h"
#include "colgen.h"
#include "ilp_solver.h"
#include "dynamic_solver.h"
#include "ufl_colgen.h"
#include "cfl_colgen.h"
#include "heuristic.h"

void Test_Single_peak_3_Weights();
void Test_Single_Peakedness();
void Test_Single_Peakedness(int n);
void Test_Single_Peakedness_Input_File();

void Test_Column_Generation(char* s);
void Test_Column_Generation();
void Compare_Gap_Colgen_LP(int n);

void Test_ILP(int n);

void Test_CFL_ColGen(int n);
void Test_CFL_ColGen();
void Test_CFL_ColGen_Input_File();

void TestHeuristic(char* s);
