/*
 * functions.h
 *
 *  Created on: Mar 23, 2012
 *      Author: eia2
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

using namespace std;



/*********check a particular skill is in the worker's set*********************/


bool checkskill(int workerid, int jobskill)
{

	if(jobskill >= skilllow[workerid] && jobskill <= skillhigh[workerid])
		return true;
	else
		return false;
}

/**************read data from tsp.txt******************/
void readfile(){
	ifstream fin;
	fin.open("/cs/grad3/eia2/workspace/Generator/tsp1 (copy).txt");

	fin>>num_job>>num_worker;
	cout<<num_job<<" "<<num_worker<<endl;

	for(int i = 1; i <= num_job; i++){
		fin>>jobid[i];
		fin>>job_skill[i];
		cout<<jobid[i]<<" "<<job_skill[i]<<endl;
	}
	cout<<endl;

	for(int i = 0; i < num_worker; i++){
		fin>>skilllow[i]>>skillhigh[i];
		cout<<skilllow[i]<<" "<<skillhigh[i]<<endl;
	}

	for(int i = 0; i <= num_job; i++){
		for(int j = 0; j <= num_job; j++)
		{
			fin>>cost[i][j];
			cout<<cost[i][j]<<" ";
		}
		cout<<endl;
	}
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
/************************* priting only Y **********************************/
void Print_Solution(IloEnv env, IloCplex cplex,ThreeDMatrix y){
	IloNumArray vals(env);
		env.out() << "Solution status = " << cplex. getStatus () << endl;
		env.out() << "Solution value = " << cplex. getObjValue () << endl;


		//Printing Y
		for(int k = 0; k < num_worker; k++){
				cout<<"Worker #"<<k+1<<": "<<endl;
				for(int i = 0; i < num_job; i++){
					cout<<"y["<<i<<"]= ";
					for(int j = 0; j < num_job; j++)
							env.out() <<cplex.getValue(y[k][i][j])<<" ";
					cout<<endl;
				}
				cout<<endl;
			}


		cout<<"##########################" <<endl;

}
/******************************* copy Y ****************************/
vector<vector<vector<bool> > > Copy_Y(IloCplex cplex, ThreeDMatrix y){
	vector<vector<vector<bool> > > y_copy;
	y_copy.resize(num_worker);
	for (int k = 0; k < num_worker; k++)
		y_copy.at(k).resize(num_job);
	for (int k = 0; k < num_worker; k++)
		for (int i = 0; i < num_job; i++)
			for (int j = 0; j < num_job; j++)
				if (cplex.getValue(y[k][i][j]))
					y_copy.at(k).at(i).push_back(true);
				else
					y_copy.at(k).at(i).push_back(false);
	return y_copy;
}



/************************* adding cycle constraints************/
void AddConnectivityConstarint(IloEnv &env, IloModel &model,IloCplex &cplex, set<int> s,vector<vector<Node> > Graph,ThreeDMatrix y, int driver){

	//cout<< "This is addConnectivity function"<<endl;
	set<int>::iterator p;
	IloExpr e(env);
	for (p = s.begin(); p != s.end(); p++){
		int i = (*p);
		for (int j = 1; j < num_job; j++){
			if (j != i){
				int index = Find_In_Vector(Graph.at(driver), j);
				if (index !=-1)
					if (s.count(j) == 0){
						if (checkskill(driver, job_skill[j]))
							e += y[driver][i][j] ;
					}
			}
		}
	}
	model.add( e >= 1);
	//cout <<" This is the end of addConnecticity" << endl;
}

/****** finding a component in the graph of a driver *********/
set<int> Find_Component(vector<vector<Node> > Graph, int k){

	list<Node> s;      // works as an stack
	set<int> iSolated_Comp;   //keep the component
	if (!Graph.at(k).empty()){
		s.push_front(Graph.at(k).front());
		while (!s.empty()){
			Node temp = s.front();
			s.pop_front();
			iSolated_Comp.insert(temp.id); // insert the ID to component

			/*** insert the links of ID to the stack ****/
			vector<Node> ::iterator p;
			for (p = temp.links.begin(); p != temp.links.end(); p++){
				int index = Find_In_Vector(Graph.at(k),p->id);
				if (index != -1)
					s.push_front(Graph.at(k).at(index));
				else{
					cout << "error in finding the index of specific ID" << endl;
					break;
				}
			}
			/**** end of inserting links of ID to stack***/
		}
	}
	return iSolated_Comp;
}

/*********************check a boolean array to be all true**************/
bool CheckBoolArray(bool a[], int size){
	for (int i = 0; i < size; i++)
		if(!a[i])
			return false;
	return true;
}

/*********************** find a node which is not connected to the other component***/
int FindVertexNonHited(bool vertex_hit[], int graph_nodeSize){
	int i = 0;
	while( i < graph_nodeSize){
		if (!vertex_hit[i])
			return i ;
		i++;
	}
	return -1;
}

/****** finding all of the component of driver K in the graph *********/
vector<set<int> > FindAllComponent(vector<vector<Node> > Graph, int k){
	vector<set<int> > all_comp;
	if (!Graph.at(k).empty()){
		int graph_nodeSize = Graph.at(k).size();
		bool vertex_hit[graph_nodeSize];
		for (int i = 0; i < graph_nodeSize; i++)
			vertex_hit[i] = false;
		while (!CheckBoolArray(vertex_hit,graph_nodeSize)){
			list<Node> s;      // works as an stack

			set<int> isolated_comp;   //keep the component
			int indexx = FindVertexNonHited(vertex_hit, graph_nodeSize);
			if (indexx == -1)
				cout << "error in finding a node" << endl;
			else
			{
				vertex_hit[indexx] = true;
				s.push_front(Graph.at(k).at(indexx));
				while (!s.empty()){
					Node temp = s.front();
					s.pop_front();
					isolated_comp.insert(temp.id); // insert the ID to component

					/*** insert the links of ID to the stack ****/
					vector<Node> ::iterator p;
					for (p = temp.links.begin(); p != temp.links.end(); p++){
						int index = Find_In_Vector(Graph.at(k),p->id);
						if (index != -1){
							if (!vertex_hit[index]){
								s.push_front(Graph.at(k).at(index));
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
	}
		return all_comp;
}



/*********************check dirivers tree connectivity**************/
bool Check_Connectivity(bool tree_connectivity[]){
	for (int i = 0; i < num_worker; i++)
		if(!tree_connectivity[i])
			return false;
	return true;
}

/***************** print the graph***************/
void PrintGraph(vector<vector<Node> > Graph, ofstream &fout){

	for (int k = 0; k < num_worker; k++){
		fout<< "Driver " << k << " graph is :" << endl;
		for (int i = 0; i < Graph.at(k).size(); i++ ){
			fout << "Node " << Graph.at(k).at(i).id << " :" ;
			vector<Node>::iterator p;
			for (p = Graph.at(k).at(i).links.begin(); p!= Graph.at(k).at(i).links.end(); p++){
				fout << " -> ";
				fout << p->id ;
			}
			fout << endl;

		}

		fout << "-------------------" ;
		fout << endl;
	}
	fout << "****************************************************" <<endl ;
}



/*********Updating the connectivity graph for each driver***************/

void Update_Graph(IloCplex & cplex, vector< vector<Node> > &Graph, BoolVarMatrix x,ThreeDMatrix y){

	for (int k = 0; k < num_worker; k++)
		for (int i= 1; i < num_job; i++)
			if (cplex.getValue(x[i][k])){
				// find the index of the node with (id = i)
				int index = Find_In_Vector(Graph.at(k),i); // check if index != -1
				/*
				vector<Node>::iterator p;
				vector<Node> temp = Graph.at(k);
				int index = 0;
				for (p = temp.begin(); p != temp.end(); p++){
					if (p->id != i)
						index++;
				}*/
				// add to the neighbor of node with (id = i) all of it adjacents nodes
				for (int j = 1; j < num_job; j++){
					if ((cplex.getValue(y[k][i][j])) || (cplex.getValue(y[k][j][i])))
						Graph.at(k).at(index).AddNeighbor(j);
					}
			}
}


/*********constructing the connectivity graph for each driver***************/

vector<vector<Node> > Drivers_Connectivity_Graph_Construct (IloCplex & cplex, BoolVarMatrix x,ThreeDMatrix y){
	vector<vector<Node> > Graph;
	Graph.resize (num_worker);
	 for (int k = 0; k < num_worker; k++){
					for (int i = 1; i < num_job; i++)
						if (cplex.getValue(x[i][k])){
							Node newNode (i);
							Graph.at(k).push_back(newNode);
						}
	 }
	 Update_Graph(cplex, Graph, x, y);
	 return Graph;

  }

/**********************create the input matrix *********************/
void Creat_Input(IloEnv env,BoolVarMatrix &x,ThreeDMatrix &y,BoolVarMatrix &z){
	for(int i = 0; i < num_job; i++){
			x[i] = IloBoolVarArray(env, num_worker);
		}

		// create the y matrix
		for(int k = 0; k < num_worker; k++){
				y[k] = BoolVarMatrix(env, num_job);
				for ( int i = 0; i < num_job; i++){
							y[k][i] = IloBoolVarArray(env, num_job);
						}
			}
		//create matrix z
		 for (int i =0 ; i < num_job ; i++){
			 z[i] = IloBoolVarArray(env, num_job);
		 }

}

/*********construct object function***************/
IloExpr generate_obj_fun(IloEnv env,BoolVarMatrix z ){

	IloExpr e(env);
		for(int i = 0; i < num_job; i++)
			for(int j = 0; j < i; j++)
				//if (i != j)
					e += cost[i][j] * z[i][j];

	return e;
}

/************************ adding easy constraints*****************/
void AddEasyConstraint(IloEnv &env , IloModel &model, BoolVarMatrix x, ThreeDMatrix y, BoolVarMatrix z){
	// initial most of the x_uj=0 regarding to the skill requirement of job u and and skill of work j
		for (int i = 1; i < num_job; i++)
			for (int k=0; k < num_worker; k++)
				if(!checkskill(k,job_skill[i]))
					model.add(x[i][k] == 0);


		// initial most of the y_ek=0 regarding to the skill requirement of job e = (i,j) and and skill of work k
		for (int k = 0 ; k < num_worker; k++)
			for (int i = 1; i < num_job; i++)
				if (!checkskill(k,job_skill[i]))
					for(int j = 1; j < num_job; j++)
						model.add(y[k][i][j] == 0);

		// y[k][i][i]=0
		for (int k = 0; k < num_worker; k++)
			for (int i = 0; i < num_job; i++)
				model.add(y[k][i][i] == 0);

		// sum_{j \in s(u)}x_{uj}=1
		for( int i = 1 ; i < num_job; i++){
				IloExpr e(env);
				for(int k = 0; k < num_worker; k++){
					if (checkskill(k, job_skill[i]))
						e += x[i][k];
					else
						model.add(x[i][k] == 0);
				}
				model.add(e == 1);
			}

		//y_[k][i][j] \le x_[i][k] & x_[j][k]
/*
		for (int k = 0; k < num_worker; k++)
			for (int i = 1; i < num_job; i++)
				for (int j = 1; j < num_job; j++){
					model.add(y[k][i][j] <= x[i][k]);
					model.add(y[k][i][j] <= x[j][k]);
					model.add(y[k][j][i] <= x[i][k]);
					model.add(y[k][j][i] <= x[j][k]);
				}
*/



		// y_{ej} <= x_{uj} + x_{vj} / 2

		for (int k =0; k < num_worker; k++)
			for (int i = 0; i < num_job; i++)
				for (int j = 0; j < num_job; j++){
					model.add(y[k][i][j] <= (x[i][k] + x[j][k]) / 2 );
				}

		// y_[]k[i][j]= y[k][j][i]
/*
			for (int k =0; k < num_worker; k++)
					for (int i = 0; i < num_job; i++)
						for (int j = 0; j < num_job; j++){
							model.add(y[k][i][j]- y[k][j][i] == 0);
						}*/

		// z=sum_{ j \in worker}y_{ej}
		for (int i = 0; i < num_job; i++)
			for (int j = 0; j < num_job; j++){
				IloExpr e(env);
				for (int k = 0; k < num_worker; k++){
					e += y[k][i][j] ;
					//e += y[k][j][i];
					//e =+ y[k][j][i]*y[k][i][j];
				}
				model.add(z[i][j] == e );
			}
/*

		for (int i = 0; i < num_job; i++)
			for (int j = 0; j < i; j++)
				for (int k = 0; k < num_worker; k++){
					model.add(y[k][i][j] <= z[i][j]);
					model.add(y[k][j][i] <= z[i][j]);
					model.add(y[k][i][j] <= z[j][i]);
					model.add(y[k][j][i] <= z[j][i]);
				}
*/



		// z[i][j]= z[j][i]

		for (int i = 0; i < num_job; i++)
					for (int j = 0; j < num_job; j++)
						model.add(z[i][j]- z[j][i] == 0);
}


/*******************print solution ******************/
void Print_Solution(IloEnv env, IloCplex &cplex,BoolVarMatrix x,ThreeDMatrix y,BoolVarMatrix z){
	IloNumArray vals(env);
		env.out() << "Solution status = " << cplex. getStatus () << endl;
		env.out() << "Solution value = " << cplex. getObjValue () << endl;

		// Printing X
		for (int i = 1; i< num_job; i++){
					cplex.getValues(vals,x[i]);
					cout << "x[" << i <<"]=" << vals << endl;
		}
		cout << endl;

		// Printing Z
		for (int i = 0; i< num_job; i++){
					cplex.getValues(vals,z[i]);
					cout << "z[" << i << "]=" <<  vals<< endl;
		}
		cout << endl;

		//Printing Y
		for(int k = 0; k < num_worker; k++){
				cout<<"Worker #"<<k+1<<": "<<endl;
				for(int i = 0; i < num_job; i++){
					cout<<"y["<<i<<"]= ";
					for(int j = 0; j < num_job; j++)
							env.out() <<cplex.getValue(y[k][i][j])<<" ";
					cout<<endl;
				}
				cout<<endl;
			}


		cout<<"##########################" <<endl;

}
/**************************Add cardinality constraints**********************************/
void addCardinalityConstraint(IloEnv &env,IloModel &model,set<int> isolated_comp, ThreeDMatrix y, int driverId){
	set<int>::iterator p,q;
	IloExpr e(env);
	for (p = isolated_comp.begin(); p != isolated_comp.end(); p++)
		for (q = isolated_comp.begin(); q != isolated_comp.end(); q++)
			if(p != q)
			e += y[driverId][(*p)][(*q)];
	int comp_size = isolated_comp.size();
	model.add(e <= (--comp_size));

}

/*************** add depot connectivity constraints**************************/
void AddDepotConnectivityConstraints(IloEnv &env, IloModel &model, vector<vector<Node> > Graph, ThreeDMatrix y){
	IloExpr e(env);
	for (int k = 0; k < num_worker; k++){
		vector<Node>::iterator p;
		for (p = Graph.at(k).begin(); p != Graph.at(k).end(); p++)
			e += y[k][0][p->id];
		model.add(e == 1);
	}
}


/**************************add all Complicated constraints******************************/

void AddComplicatedConstraints (IloEnv &env, IloModel model, IloCplex cplex, BoolVarMatrix x, ThreeDMatrix y, BoolVarMatrix z){
	vector<vector<Node> > Graph = Drivers_Connectivity_Graph_Construct (cplex, x, y);
		ofstream result_graph;
		result_graph.open("graph.txt");
		PrintGraph(Graph,result_graph);
		vector<vector<vector<bool> > > y_copy = Copy_Y(cplex, y);
		bool tree_connectivity[num_worker];
		for (int  i = 0; i < num_worker; i ++)
			tree_connectivity[i] = false;
		while(!Check_Connectivity(tree_connectivity)){
			for (int k = 0; k < num_worker; k++){
				if (Graph.at(k).empty())
					tree_connectivity[k] = true;
				else{
				vector<set<int> > driverKComponents = FindAllComponent(Graph, k);   //keep the component
				vector<set<int> > ::iterator p;
				for (p = driverKComponents.begin(); p != driverKComponents.end(); p++){
					set<int> isolated_comp = *p;
						//cout << "compsize ="<< isolated_comp.size() << endl;
						//cout << "graph k ="<< Graph.at(k).size()<< endl;

						if (isolated_comp.size() == Graph.at(k).size()){
							//cout << " the tree of driver " << k << "is connected" << endl;
							tree_connectivity[k] = true;
						}
						else{
							tree_connectivity[k] = false;
							//addCardinalityConstraint(env,model,isolated_comp,y, k);
							AddConnectivityConstarint(env,model,cplex,isolated_comp,Graph,y, k); //generate constarint for this isolated component
							//cout << "hi" << endl;
						}
						//isolated_comp.clear();
					}
				}
			}
			//AddDepotConnectivityConstraints(env, model, Graph, y);
			if (!Check_Connectivity(tree_connectivity)){
				cplex.setParam(IloCplex::EpGap,.1);
				if ( !cplex.solve() ) {
					env.error() << "Failed to optimize LP." << endl;
					throw (-1);
				}
			}
			cout<<"####################solved"<<endl;
			Graph = Drivers_Connectivity_Graph_Construct (cplex, x, y);
			// print the solution
			PrintGraph(Graph,result_graph);
			y_copy = Copy_Y(cplex, y);
			Print_Solution(env,cplex,x,y,z);
			//Check_Connectivity(tree_connectivity);

		}
}


#endif /* FUNCTIONS_H_ */
