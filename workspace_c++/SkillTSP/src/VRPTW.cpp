//============================================================================
// Name        : VRPTW.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
/************************
*TIME WINDOW
*************************/

#include <iostream>
#include <fstream>
#include <ilcplex/ilocplex.h>

using namespace std;

typedef IloArray < IloBoolVarArray > BoolVarMatrix ;
typedef IloArray < BoolVarMatrix > ThreeDMatrix;
typedef IloArray < IloNumVarArray > NumVarMatrix ;
typedef IloArray < NumVarMatrix > ThreeDNumMatrix;
typedef IloArray < IloNumVarArray > TwoDMatrix;

const int MAX_JOB = 1000;
const int MAX_WORKER = 100;

static int num_job;
static int num_worker;
static int job_demand[MAX_JOB]; //start from index 1;
static double job_release[MAX_JOB]; //start from index 0;
static double job_deadline[MAX_JOB];//start from index 0;
static int job_service[MAX_JOB];
static double cost[MAX_JOB][MAX_JOB]; //store cij.


/**************read data from tsp.txt*******************/
void readfile(){
	ifstream fin;
	fin.open("/Users/ehsaniranmanesh/Documents/workspace_ccc/VRPTW_GENERATOR/tsp.txt");

	fin>>num_job>>num_worker;
	cout<<num_job<<" "<<num_worker<<endl;

	for(int i = 0; i <= num_job; i++){
		fin>>job_demand[i];
		cout<<job_demand[i]<<" ";
	}
	cout<<endl;

	for(int i = 0; i <= num_job; i++){
		fin>>job_release[i];
		cout<<job_release[i]<<" ";
	}
	cout<<endl;

	for(int i = 0; i <= num_job; i++){
		fin>>job_deadline[i];
		cout<<job_deadline[i]<<" ";
	}
	cout<<endl;

	for(int i = 0; i <= num_job; i++){
		fin>>job_service[i];
		cout<<job_service[i]<<" ";
	}
	cout<<endl;

	for(int i = 0; i <= num_job; i++){
		for(int j = 0; j <= num_job; j++)
		{
			fin>>cost[i][j];
			cout<<cost[i][j]<<" ";
		}
		cout<<endl;
	}

}

/*********construct object function***************/
IloExpr generate_obj_fun(IloEnv env, ThreeDMatrix x){

	IloExpr e(env);
	for( int k = 0; k < num_worker; k++){
		for(int i = 0; i <= num_job; i++){
			for(int j = 0; j <= num_job; j++){
				if(i != j)
					e += cost[i][j] * x[k][i][j];
			}
		}
	}

	return e;
}

/****************************main()***********************************/
int main()
{
	readfile();
	ofstream fout;
	fout.open("tsp.txt");

	/*****************COMPUTING OPTIMAL SOLUTION WITH CPLEX**************************************/
	cout<<"##################construction"<<endl;
	IloEnv env;
	try {
	IloModel model(env);
	ThreeDNumMatrix x(env, num_worker);
	for(int k = 0; k < num_worker; k++){
		x[k] = NumVarMatrix(env, num_job+1);
		for ( int i = 0; i < num_job + 1; i++){
		  x[k][i] = IloNumVarArray(env, num_job + 1, ILOFLOAT, 0,1);
		}
	}

	IloExpr obj_fun=generate_obj_fun(env,x);
	model.add(IloMinimize(env, obj_fun));


	//constraint 3.2
	for(int i = 1; i <= num_job; i++)
	{
		IloExpr constraint(env);
		for(int k = 0; k < num_worker; k++){
			for(int j = 0; j <= num_job; j++)
				if(i != j)
					constraint += x[k][i][j];
		}
		model.add(constraint == 1);
	}

	//constraint 3.3
	for(int k = 0; k < num_worker; k++){
		IloExpr constraint(env);
		for(int i = 1; i <=num_job; i++){
			for(int j = 0; j <= num_job; j++)
				if(i != j)
					constraint += job_demand[i]*x[k][i][j];
		}
		model.add(constraint <= 2000);
	}

	//constraint 3.4
	for(int k = 0; k < num_worker; k++){
		IloExpr constraint(env);
		for(int j = 1; j <= num_job; j++){
			constraint += x[k][0][j];
		}
		model.add(constraint == 1);
	}
	//constraint 3.5
	for(int k = 0; k < num_worker; k++){
		for(int i = 1; i <= num_job; i++){
			IloExpr constraint(env);
			for(int j = 0; j <= num_job; j++){
				if(i != j)
					constraint = constraint + x[k][i][j] - x[k][j][i];
			}
			model.add(constraint == 0);
		}
	}

	//constraint 3.6
	for(int k = 0; k < num_worker; k++){
		IloExpr constraint(env);
		for(int i = 1; i <= num_job; i++)
			constraint += x[k][i][0];
		model.add(constraint == 1);
	}

	cout<<"#######################start"<<endl;
	IloCplex cplex(model);
	if ( !cplex.solve() ) {
		env.error() << "Failed to optimize LP." << endl;
		throw (-1);
	}
	cout<<"####################solved"<<endl;

	IloNumArray vals(env);
	env.out() << "Solution status = " << cplex. getStatus () << endl;
	env.out() << "Solution value = " << cplex. getObjValue () << endl;
	cout<<"##########################" <<endl;

	bool xx[num_worker][num_job][num_job];

	for (int k = 0; k < num_worker; k++)
		for (int i = 0; i <= num_job; i++)
			for ( int j = 0; j <= num_job; j++)
				xx[k][i][j]=false;

	for(int k = 0; k < num_worker; k++){
		cout<<"Worker #"<<k+1<<": "<<endl;
		for(int i = 0; i <= num_job; i++){
			cout<<"x["<<i<<"]= ";
			for(int j = 0; j <= num_job; j++){
				if(j == i)
				cout<<"a"<<" ";
				else{
					env.out() <<cplex.getValue(x[k][i][j])<<" ";
					//bool temp =cplex.getValue(x[k][i][j]);
					//<< temp << " ";
					//xx[k][i][j]= (bool)cplex.getValue(x[k][i][j]);

					double temp =cplex.getValue(x[k][i][j]);
					if (temp!=1)
						xx[k][i][j]=false;
					else
						xx[k][i][j]=true;
				}
			}
			cout<<endl;
		}
		cout<<endl;
	}
	/************************************/


		for (int k = 0; k < num_worker; k++){
			fout << "Woker #" << k << endl;

				for (int i = 0; i <= num_job; i++){
					for ( int j = 0; j <= num_job; j++)
						fout << xx[k][i][j]  << " ";
					fout << endl;
				}
		}
		fout.close();
	/**********************************/

	} catch ( IloException & e) {
		cerr << "Concert exception caught: " << e << endl;
	} catch (...) {
		cerr << "Unknown exception caught" << endl;
		}

	env.end();

	/*****************END OF COMPUTING OPTIMAL SOLUTION WITH CPLEX*******************************/
	return 0;

}












