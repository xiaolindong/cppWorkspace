/*
 * NEW_IPCUT_MST.h
 *
 *  Created on: Mar 29, 2012
 *      Author: eia2
 */

#ifndef NEW_IPCUT_MST_H_
#define NEW_IPCUT_MST_H_
#include <vector>
#include <set>
#include <list>
#include <fstream>
#include "variables.h"
#include "node.h"
#include "EdgeManager.h"
#include <map>


using namespace std;

/********************************************************/
void InitialEdgeMatrix(EdgeManager &EdgeMatrix){
	int edgecounter = 1;
	for (int i = 0; i < num_job; i++)
		for (int j = 0; j < i; j++)
		{

			EdgeMatrix.AddEdge(i,j,edgecounter);
			cout << "( "<< i << " , "<< j << " ) "<< " => "<< edgecounter++<< endl;
		}
}


/**************read data from tsp.txt******************/
void readfile(){
	ifstream fin;
	fin.open("/cs/grad3/eia2/workspace/Generator/tsp1.txt");

	fin>>num_job>>num_worker;
	cout<<num_job<<" "<<num_worker<<endl;

	for(int i = 0; i <= num_job; i++){
		for(int j = 0; j <= num_job; j++)
		{
			fin>>cost[i][j];
			cout<<cost[i][j]<<" ";
		}
		cout<<endl;
	}
}

/***********************************************/
/*
ILOMIPCALLBACK2(MyCallback,IloNum,n, int ,m) {
  //cout << "Iteration " << getNiterations() << ": ";
  cout << "yuyuyuyuyuyuyuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu" << endl;

}



/***************** print the graph***************/
void PrintGraph(vector<Node> Graph, ofstream &fout){

	for (int i = 0; i < Graph.size(); i++ ){
			fout << "Node " << Graph.at(i).id << " :" ;
			vector<Node>::iterator p;
			for (p = Graph.at(i).links.begin(); p!= Graph.at(i).links.end(); p++){
				fout << " -> ";
				fout << p->id ;
			}
			fout << endl;

		}

		fout << "-------------------" ;
		fout << endl;
	fout << "****************************************************" <<endl ;
}



/**********************create the input matrix *********************/
/*
void Creat_Input(IloEnv env,BoolVarMatrix &x){
	for(int i = 0; i < num_job; i++)
			x[i] = IloBoolVarArray(env, nEdges);

	for(int i = 0; i < num_job; i++)
				z[i] = IloBoolVarArray(env, num_job);
}


/*********construct object function***************/

IloExpr generate_obj_fun(IloEnv env, EdgeManager &EdgeMatrix, IloBoolVarArray y){
	//cout << "hiiiii";
	IloExpr e(env);
		for(int i = 1; i <= nEdges; i++)
		{
			pair <int,int> edge = EdgeMatrix.GetEdgeByLabel(i);
			cout << "( " << edge.first << "," << edge.second << " ) " << endl;

			e += cost[edge.first][edge.second] * y[i];
		}
		return e;
}

/******************************************************************/
void AddEasyConstraint(IloEnv &env , IloModel &model, IloBoolVarArray y)
{


	IloExpr e(env);
		for (int i = 1; i <= nEdges; i++)
			e += y[i];
		model.add( e >= (num_job -1));
		//model.add(y[0] == 0);
}


/*******************print solution ******************/
void Print_Solution(IloEnv env, IloCplex cplex,IloBoolVarArray y){
	IloNumArray vals(env);
		env.out() << "Solution status = " << cplex. getStatus () << endl;
		env.out() << "Solution value = " << cplex. getObjValue () << endl;

		// Printing X
		cplex.getValues(vals,y);
		cout << " y = " << vals << endl;
		cout<<"##########################" <<endl;

}

/*******************************************************************************/

void initial(vector<Node> &Graph){
	Graph.resize(num_job);
	vector<Node>::iterator p;
	int i = 0;
	for (p = Graph.begin(); p != Graph.end(); p++){
		p->id = i ;
		i++;
	}

}
/*******************************************************************************/
void initBool(vector<Node> &Graph){
	vector<Node>::iterator p;
	for (p = Graph.begin(); p != Graph.end(); p++){
		p->flag = false ;
	}

}

/*********constructing the connectivity graph for each driver***************/

vector<Node>  Graph_Construction (IloCplex cplex, EdgeManager EdgeMatrix, IloBoolVarArray y){
	vector<Node> Graph;
	initial(Graph);
	 for (int i = 1; i <= nEdges; i++)
	 {
			if (cplex.getValue(y[i]))
			{
				pair<int, int> edge =EdgeMatrix.GetEdgeByLabel(i);
				Graph.at(edge.first).AddNeighbor(edge.second);
				Graph.at(edge.second).AddNeighbor(edge.first);
			}

	 }
	 //Update_Graph(cplex, Graph, x);
	 return Graph;

  }

/************************* adding cycle constraints************/

void AddConnectivityConstarint(IloEnv &env , IloModel &model, IloCplex &cplex, set<int> s,EdgeManager EdgeMatrix, IloBoolVarArray y){

/*
	set<int>::iterator p1;
	IloExpr e1(env);
	for (p1 = s.begin(); p1 != s.end(); p1++)
		for (int j = 0; j < num_job; j++)
		if (s.count(j) == 0)
			e1 += y[EdgeMatrix.GetLabel ((*p1),j)];
	model.add(e1 >= 1);*/

	//cout<< "This is addConnectivity function"<<endl;

	set<int>::iterator p,q;
	int comp_size= s.size();
	int array[comp_size];
	IloExpr e(env);
	int k = 0;
	for (p = s.begin(); p != s.end(); p++)
		array[k++] = (*p);

	for (int i = 0 ; i < comp_size - 1; i ++)
		for (int j = i + 1; j < comp_size; j++)
		{
			int label = EdgeMatrix.GetLabel(array[i],array[j]);
			e += y[label];
		}
	model.add(e <= (comp_size - 1));
	//cplex.addLazyConstraint(e <= (comp_size - 1));
}

/************Searching for the index of an ID in the vector******************/
int Find_In_Vector(vector<Node> v, int id){
	vector<Node>::iterator p = v.begin();
	int index = -1;
	int i = 0;
	while (p != v.end()){
		if (p->id == id){
			index = i;
			break;
		}
		else
			i++;
		p++;
	}
	return index;
}

/*************************************************************/
set<int> Find_Component(vector<Node> Graph){
	cout << "hi";
	initBool(Graph);
	list<Node> s;      // works as an stack
	set<int> iSolated_Comp;   //keep the component
	s.push_front(Graph.front());
	Graph.front().flag = true;
	cout << "hi2";
		while (!s.empty()){
			Node temp = s.front();
			s.pop_front();
			iSolated_Comp.insert(temp.id); // insert the ID to component

			/*** insert the links of ID to the stack ****/
			vector<Node> ::iterator p;
			for (p = temp.links.begin(); p != temp.links.end(); p++){
				int index = Find_In_Vector(Graph,p->id);
				if (index != -1){
					if (!Graph.at(index).flag){
						s.push_front(Graph.at(index));
						Graph.at(index).flag = true;
					}
				}
				else{
					cout << "error in finding the index of specific ID" << endl;
					break;
				}
			}
			/**** end of inserting links of ID to stack***/
		}

	return iSolated_Comp;
}
/***********************************************************/
int FindVertexNonHited(bool vertex_hit[], int size){
	int i = 0;
	while( i < size){
		if (!vertex_hit[i])
			return i ;
		i++;
	}
	return -1;
}

/*********************check a boolean array to be all true**************/
bool CheckBoolArray(bool a[], int size){
	for (int i = 0; i < size; i++)
		if(!a[i])
			return false;
	return true;
}

/****** finding all of the component of driver K in the graph *********/
vector<set<int> > FindAllComponent(vector<Node> Graph){

		vector<set<int> > all_comp;
		int nJobs = Graph.size();
		bool vertex_hit[nJobs];
		for (int i = 0; i < nJobs; i++)
			vertex_hit[i] = false;
		while (!CheckBoolArray(vertex_hit,nJobs)){
			list<Node> s;      // works as an stack

			set<int> isolated_comp;   //keep the component
			int indexx = FindVertexNonHited(vertex_hit, nJobs);
			if (indexx == -1)
				cout << "error in finding a node" << endl;
			else
			{
				vertex_hit[indexx] = true;
				s.push_front(Graph.at(indexx));
				while (!s.empty()){
					Node temp = s.front();
					s.pop_front();
					isolated_comp.insert(temp.id); // insert the ID to component

					/*** insert the links of ID to the stack ****/
					vector<Node> ::iterator p;
					for (p = temp.links.begin(); p != temp.links.end(); p++){
						int index = p->id;
						if (index != -1){
							if (!vertex_hit[index]){
								s.push_front(Graph.at(index));
								vertex_hit[index] = true;
							}
						}
						else{
							cout << "error in finding the index of specific ID" << endl;
							break;
						}
					}
					/**** end of inserting links of ID to stack***/
				}

			}
			all_comp.push_back(isolated_comp);
		}

		return all_comp;
}

/**********************************************/
void print_vector (vector<set<int> > aComp, ofstream &result_graph){
	vector<set<int> >::iterator p;
	int i = 0 ;
	for (p = aComp.begin(); p != aComp.end(); p++){
		result_graph << "The aComp size is : " << aComp.size() << endl;
		result_graph << "The " << i << "component is : ";
		set<int> s = (*p);
		result_graph << "The " << i << "the component size is : " << s.size() << endl;
		set<int>::iterator q;
		for (q = s.begin(); q!= s.end(); q++){
			result_graph << (*q) << "," ;
		}
		result_graph << endl;
		i++;
	}
	result_graph << "********************************************************************************************" << endl;
}

/**********************************************************************/
int CountNumberOfOne(IloCplex cplex,IloBoolVarArray y)
{
	int counter = 0;
	for (int i = 0; i <= nEdges; i++)
		if (cplex.getValue(y[i]))
			counter++;
	return counter;
}

/******************************************************************************************/
void ComputeObjectiveFunctionValue(vector<Node> Graph){
			double ObjValue = 0;
			vector<Node>::iterator p ;
			for (p = Graph.begin(); p != Graph.end(); p++){
				vector<Node>::iterator q;
				for (q = p->links.begin(); q != p->links.end(); q++)
					ObjValue += cost[p->id][q->id];
			}
			cout<< endl << "The objective function value for the PRIM tree is :" << ObjValue/2 << endl;
	}


/***********************************************/

/**************************add all Complicated constraints******************************/

void AddComplicatedConstraints (IloEnv &env, IloModel &model, IloCplex cplex, EdgeManager EdgeMatrix,IloBoolVarArray y){

	static int i = 0;

	vector<Node> Graph = Graph_Construction (cplex,EdgeMatrix, y);
	ofstream result_graph;
	result_graph.open("graph.txt");
   //	PrintGraph(Graph,result_graph);
	bool treeconnectivity = false;

	while ((!treeconnectivity)){
		vector<set<int> > aComp = FindAllComponent(Graph);

		result_graph << "i = " << i << " and the total number of component is : " << aComp.size() << endl;
					PrintGraph(Graph,result_graph);
					result_graph << endl;
					print_vector(aComp, result_graph);
		/*if (aComp.size() == 1)
			treeconnectivity = true;
		else{
			result_graph << "i = " << i << " and the total number of component is : " << aComp.size() << endl;
			PrintGraph(Graph,result_graph);
			result_graph << endl;
			print_vector(aComp, result_graph);

			for (int i = 0; i < aComp.size() - 1; i++)
				for (int j = 0 ; j < aComp.size(); j++){
					set <int> s = aComp.at(i);
					set <int> w = aComp.at(j);
					AddConnectivityConstarint(env,model,cplex,s,w,x); //generate constarint for this isolated component
				}
		}*/

		vector<set<int> >::iterator p;
		for (p = aComp.begin(); p != aComp.end(); p++){
			set<int> s = *p;
			if (s.size() == Graph.size()){
				if (CountNumberOfOne(cplex,y) == (num_job - 1))
				{
					treeconnectivity = true;
					cout << "Number of one in y's matrix is : " << CountNumberOfOne(cplex,y) << endl;
					AddConnectivityConstarint(env,model,cplex,s,EdgeMatrix,y); //generate constarint for this isolated component
					if ( !cplex.solve() ) {
						env.error() << "Failed to optimize LP." << endl;
						throw (-1);
					}
					cout << "Number of one in y's matrix is : " << CountNumberOfOne(cplex,y) << endl;
				}
			}

			else
				AddConnectivityConstarint(env,model,cplex,s,EdgeMatrix,y); //generate constarint for this isolated component
		}
		//if (!treeconnectivity){
		//cplex.setParam(IloCplex::EpGap,.01);
			if ( !cplex.solve() ) {
				env.error() << "Failed to optimize LP." << endl;
				throw (-1);
			}
			cout <<"hi" << i++ << endl;
			Graph = Graph_Construction(cplex,EdgeMatrix, y);
			PrintGraph(Graph,result_graph);
		//}
	}
	ComputeObjectiveFunctionValue(Graph);
	cout << "Number of one in y's matrix is : " << CountNumberOfOne(cplex,y) << endl;
	// print the solution

}


#endif /* NEW_IPCUT_MST_H_ */
