//============================================================================
// Name        : NEW_MSP_IPCUT.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : This program find the minimum spanning tree of a graph
// the input is nJobs and nWorker = 1
//============================================================================


#include <iostream>
#include <ilcplex/ilocplex.h>
#include "variables.h"
#include "node.h"
#include "NEW_IPCUT_MST.h"
#include <vector>
#include <set>
#include <list>
#include <map>
#include <stack>
#include <ctime>
#include <math.h>
#include "EdgeManager.h"
using namespace std;



int main() {
	readfile();
	num_job++;

	time_t start,end;
	cout<< "nEdges = " << nEdges << " nJobs = " << num_job << endl;
	EdgeManager EdgeMatrix;
	InitialEdgeMatrix(EdgeMatrix);
		cout<<"##################construction"<<endl;
		time(&start);
		IloEnv env;
		try {

			// Model and variables defining
			IloModel model(env);
			IloBoolVarArray y(env,nEdges+1); //
			BoolVarMatrix x(env,num_job); // for every jobs (rows) and workers (columns)

			// create the x,y,z matrix
			//Creat_Input(env,x);

			//generate the obj_function
			IloExpr obj_fun=generate_obj_fun(env,EdgeMatrix,y);
			model.add(IloMinimize(env, obj_fun));

			/*add new constraints here*/
			AddEasyConstraint(env , model, y);
			//AddEasyConstraint(env , model, x, y, z,z_bar);



			// solve the model
			cout<<"#######################start"<<endl;
			IloCplex cplex(model);
			//cplex.use(MyCallback(env,4,5));
			//cplex.setParam(IloCplex::RootAlg, IloCplex::Network);
			//cplex.setVectors();
			if ( !cplex.solve() ) {
				env.error() << "Failed to optimize LP." << endl;
				throw (-1);
			}
			cout<<"####################solved"<<endl;
			// print the solution
			//Print_Solution(env,cplex,y);


			//adding cycle elimination and cardinality constraints
			AddComplicatedConstraints(env, model, cplex, EdgeMatrix ,y);
			if ( !cplex.solve() ) {
						env.error() << "Failed to optimize LP." << endl;
						throw (-1);
					}
			cout << "Number of one in y's matrix is : " << CountNumberOfOne(cplex,y) << endl;
			//Print_Solution(env,cplex,y);
			time(&end);
			cout << "The runnung time in seconds is : " << difftime(end,start) << " seconds" << endl;
			env.out() << "Solution value = " << cplex. getObjValue () << endl;


		} catch ( IloException & e) {
			cerr << "Concert exception caught: " << e << endl;
		} catch (...) {
			cerr << "Unknown exception caught" << endl;
		}

		env.end();
		cout << "end" << endl;
		return 0;

	}

