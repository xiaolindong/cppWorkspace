/*
 * all_triple_formul_solver.cpp
 *
 *  Created on: April 14, 2014
 *      Author: eia2
 */


#include "all_triple_formul_solver.h"



inline int indicate(int a) { return (a > 0) ? 1 : 0;} 

void AllTripleFormulKemeny::PrintResults()
{
    cout << "The y values in IP are :" << endl;
    IloNumArray val_y_ip(env);
    for (int i = 0; i < R.Candidates();i++)
    {
        for (int j = 0; j < R.Candidates(); j++)
        {	
            cout << "y["<< i <<"]["<< j << "]= " ;
            ip_cplex.getValues(val_y_ip,y[i][j]);
            cout << val_y_ip << " "<< endl ;						
        }
        cout << endl;
    }
}



void AllTripleFormulKemeny::InitialTripleCostCube()
{
    cout << endl;
    for (int i = 0; i < R.Candidates(); i++ )
        for (int j = 0; j < R.Candidates(); j++)
            for (int k = 0; k < R.Candidates(); k++)
                if (i != j && i != k && j != k)
                {
                    triple_cost_cube[i][j][k] = 0;
                    //cout << i << "," << j << "," << k << "-> " << triple_cost_cube[i][j][k] << endl;
                }	

}




void AllTripleFormulKemeny::PrintTripleCostCube()
{
    cout << "Print triple cost "<<endl;
    for (int i = 0; i < R.Candidates(); i++ )
        for (int j = 0; j < R.Candidates(); j++)
            for (int k = 0; k < R.Candidates(); k++)
                cout << i << "," << j << "," << k << "-> " << triple_cost_cube[i][j][k] << endl;


}







void AllTripleFormulKemeny::ComputeTripleCost()
{
    for (int i = 0; i < R.Candidates(); i++ )
        for (int j = 0; j < R.Candidates(); j++)
            for (int k = 0; k < R.Candidates(); k++)
                if (i != j && i != k && j != k)
                {
                    triple_cost_cube[i][j][k] =  (R.Get_m_Array()[i][j] + R.Get_m_Array()[i][k]+ R.Get_m_Array()[k][j])/(R.Candidates()-2);
                }
}


/*



   void AllTripleFormulKemeny::ComputeTripleCost()
   {
   for (int v= 0; v < R.Voters(); v++)
   for (int i = 0; i < R.Candidates(); i++ )
   for (int j = 0; j < R.Candidates(); j++)
   for (int k = 0; k < R.Candidates(); k++)
   if (i != j && i != k && j != k)
   {
// triple_cost_cube[i][j][k] =  (R.Get_m_Array()[i][j] + R.Get_m_Array()[i][k]+ R.Get_m_Array()[k][j])/(R.Candidates()-2);
int r_i = R.FindRankinProfile(v,i);
//	cout << "r_i is :" << r_i << endl;
int r_j = R.FindRankinProfile(v,j);
//	cout << "r_j is :" << r_j << endl;
int r_k = R.FindRankinProfile(v,k);
//	cout << "r_k is :" << r_k << endl;
triple_cost_cube[i][j][k] += indicate(r_i-r_j) + indicate( r_j-r_k) + indicate(r_i-r_k);
//if (r_i > r_j || r_i > r_k || r_j > r_k)
//	{
//		triple_cost_cube[i][j][k]++;
//		cout << "triple " << i << ","<<j <<"," << k << " " << triple_cost_cube[i][j][k] << endl; 
//	}
}


}


*/

/*

   void AllTripleFormulKemeny::ComputeTripleCost()
   {
   for (int i = 0; i < R.Candidates(); i++ )
   for (int j = 0; j < R.Candidates(); j++)
   for (int k = 0; k < R.Candidates(); k++)
   if (i != j && i != k && j != k)
   {
   int temp =0;
   for (int v= 0; v < R.Voters(); v++)
   {
//triple_cost_cube[i][j][k] = R.Voters()-(R.Get_m_Array()[i][j] + R.Get_m_Array()[i][k]+ R.Get_m_Array()[k][j])/(R.Candidates()-2);
int r_i = R.FindRankinProfile(v,i);
//	cout << "r_i is :" << r_i << endl;
int r_j = R.FindRankinProfile(v,j);
//	cout << "r_j is :" << r_j << endl;
int r_k = R.FindRankinProfile(v,k);
//	cout << "r_k is :" << r_k << endl;
//triple_cost_cube[i][j][k] += (indicate(r_i-r_j) + indicate( r_j-r_k) + indicate(r_i-r_k));
//if (r_i > r_j || r_i > r_k || r_j > r_k)
// {
// triple_cost_cube[i][j][k]++;
// cout << "triple " << i << ","<<j <<"," << k << " " << triple_cost_cube[i][j][k] << endl; 
// }
if ((indicate(r_i-r_j) + indicate( r_j-r_k) + indicate(r_i-r_k) != 0))
temp++;
}
triple_cost_cube[i][j][k] = temp;

}	
}

*/

IloExpr AllTripleFormulKemeny::CreateObjFunction()
    /**************************************************/
    /* This Function generates the objective function */
    /**************************************************/
{

    IloExpr e(env);
    for (int i = 0; i < R.Candidates(); i++ )
        for (int j = 0; j < R.Candidates(); j++)
            for (int k = 0; k < R.Candidates(); k++)
                e += triple_cost_cube[i][j][k]*y[i][j][k];
    return e;
}


void AllTripleFormulKemeny::AddConstraints()
    /****************************************************/
    /* This function add constarints to the formulation */
    /****************************************************/
{


    for(int i = 0; i < R.Candidates(); i++)
        for(int j =0; j < R.Candidates(); j++)
            for (int k = 0 ; k < R.Candidates(); k++)
                if (i == j || i == k || j == k)
                {
                    ip_model.add(y[i][j][k] == 0);
                    // lp_model.add(y[i][j][k] == 0);
                    //	cout << endl << i <<","<< j << "," << k << endl; 
                }
    /*
       cout << endl << "********" << endl;
       for (int i = 0; i < R.Candidates(); i++)
       for (int j = 0; j < R.Candidates(); j++)
       for (int k = 0; k < R.Candidates(); k++)
       if ( i!=j && i!=k && j!=k)
       {
       cout << i << "," << j << "," << k << " & " 
       << i << "," << k << "," << j << " & " 
       << k << "," << i << "," << j << " & "
       << j << "," << i << "," << k << " & "
       << j << "," << k << "," << i << " & "
       << k << "," << j << "," << i << " & " 
       << endl;

       ip_model.add(y[i][j][k] + y[i][k][j] + y[j][i][k] + y[j][k][i] + y[k][i][j] + y[k][j][i] == 1);
       lp_model.add(y[i][j][k] + y[i][k][j] + y[j][i][k] + y[j][k][i] + y[k][i][j] + y[k][j][i] == 1);

       }
       */

    for (int i = 0; i < R.Candidates(); i++)
        for (int j = 0; j < R.Candidates(); j++)	
            if (i != j)
            {
                for (int p = 0; p < R.Candidates(); p++)
                    for (int q = 0; q < R.Candidates(); q++)
                        if ( i!=p && i!=q && j!=p && j!=q)
                        {
                            //                            cout << "i is " << i << " j is " << j << " p is " << p << " q is " << q << endl << endl;
                            ip_model.add(y[p][i][j] + y[i][p][j] + y[i][j][p] + y[q][j][i] + y[j][q][i] + y[j][i][q] == 1);
                            // lp_model.add(y[p][i][j] + y[i][p][j] + y[i][j][p] + y[q][j][i] + y[j][q][i] + y[j][i][q] == 1);
                        }
            }

    /*

       void AllTripleFormulKemeny::AddTripleConstraints()
       {
       for (int i = 0; i < R.Candidates(); i++)
       for (int j = 0; j < R.Candidates(); j++)
       for (int k = 0; k < R.Candidates(); k++)
       if ( i!=j && i!=k && j!=k)
       {
       ip_model.add(y[i][j][k] - x[i][j] <= 0);
       ip_model.add(y[i][j][k] - x[j][k] <= 0);
       ip_model.add(y[i][j][k] - x[i][k] <= 0);

       lp_model.add(y[i][j][k] - x[i][j] <= 0);
       lp_model.add(y[i][j][k] - x[j][k] <= 0);
       lp_model.add(y[i][j][k] - x[i][k] <= 0);
       }
       for (int i = 0; i < R.Candidates()-2; i++)
       for (int j = i+1; j < R.Candidates()-1; j++)
       for (int k = j+1; k < R.Candidates(); k++)
       if ( i!=j && i!=k && j!=k)
       {
       ip_model.add(y[i][j][k] + y[i][k][j] + y[j][i][k] + y[j][k][i] + y[k][i][j] + y[k][j][i] == 1);
       lp_model.add(y[i][j][k] + y[i][k][j] + y[j][i][k] + y[j][k][i] + y[k][i][j] + y[k][j][i] == 1);

       }

       for (int i = 0; i < R.Candidates(); i++)
       for (int j = 0; j < R.Candidates(); j++)
       if (i !=j)
       for (int k = 0; k < R.Candidates(); k++)	
       if (k!=j)
       {
       ip_model.add(y[k][i][j] + y[i][k][j] + y[i][j][k] - x[i][j] == 0);
       lp_model.add(y[k][i][j] + y[i][k][j] + y[i][j][k] - x[i][j] == 0);

       }
       */	
}







void AllTripleFormulKemeny::SolveILP()
{
    /**********************************************************/
    /* This function compute the optimal solution using CPLEX */
    /* 		   Minimizing Proportional Representaion          */
    /**********************************************************/
    try
    {

        IloExpr obj_fun = CreateObjFunction(); //generate the obj_function
        ip_model.add(IloMinimize(env, obj_fun));

        AddConstraints();/*add new constraints here*/
        ip_cplex.setOut(env.getNullStream());

        time_t start, end;
        time(&start);

        // solve the model	
        cout<<"#######################start"<<endl;
        if ( !ip_cplex.solve() ) {
            env.error() << "Failed to optimize LP." << endl;
            throw (-1);
        }
        cout<<"####################solved"<<endl;

        env.out() << "All Triple formulation IP Solution status = " << ip_cplex.getStatus() << endl;
        env.out() << "All Triple formulation IP Solution value = " << ip_cplex.getObjValue() << endl;
        val_ip = ip_cplex.getObjValue();

        time(&end);
        rt_ip = difftime(end, start);

        PrintResults();
        //cplex.setParam(IloCplex::RootAlg, IloCplex::Primal);
        //cplex.setParam(IloCplex::NodeAlg, IloCplex::Primal);
    }
    catch ( IloException & e)
    {
        cerr << "Concert exception caught: " << e << endl;
    } catch (...)
    {
        cerr << "Unknown exception caught" << endl;
    }



}





IloExpr AllTripleFormulKemeny::Lp_CreateObjFunction()
    /**************************************************/
    /* This Function generates the objective function */
    /**************************************************/
{

    IloExpr e(env);
    for (int i = 0; i < R.Candidates(); i++ )
        for (int j = 0; j < R.Candidates(); j++)
            for (int k = 0; k < R.Candidates(); k++)
                e += triple_cost_cube[i][j][k]*y_lp[i][j][k];
    return e;
}


void AllTripleFormulKemeny::Lp_AddConstraints()
    /****************************************************/
    /* This function add constarints to the formulation */
    /****************************************************/
{


    for(int i = 0; i < R.Candidates(); i++)
        for(int j =0; j < R.Candidates(); j++)
            for (int k = 0 ; k < R.Candidates(); k++)
                if (i == j || i == k || j == k)
                {
                    lp_model.add(y_lp[i][j][k] == 0);
                    //	cout << endl << i <<","<< j << "," << k << endl; 
                    /*               }

                                     cout << endl << "********" << endl;
                                     for (int i = 0; i < R.Candidates(); i++)
                                     for (int j = 0; j < R.Candidates(); j++)
                                     for (int k = 0; k < R.Candidates(); k++)
                                     if ( i!=j && i!=k && j!=k)
                                     {
                                     cout << i << "," << j << "," << k << " & " 
                                     << i << "," << k << "," << j << " & " 
                                     << k << "," << i << "," << j << " & "
                                     << j << "," << i << "," << k << " & "
                                     << j << "," << k << "," << i << " & "
                                     << k << "," << j << "," << i << " & " 
                                     << endl;

                                     lp_model.add(y_lp[i][j][k] + y_lp[i][k][j] + y_lp[j][i][k] + y_lp[j][k][i] + y_lp[k][i][j] + y_lp[k][j][i] == 1);

                                     }
                                     */
}
for (int i = 0; i < R.Candidates(); i++)
    for (int j = 0; j < R.Candidates(); j++)	
if (i != j)
{
    for (int p = 0; p < R.Candidates(); p++)
        for (int q = 0; q < R.Candidates(); q++)
            if ( i!=p && i!=q && j!=p && j!=q)
            {
                //ip_model.add(y[p][i][j] + y[i][p][j] + y[i][j][p] + y[q][j][i] + y[j][q][i] + y[j][i][q] == 1);
                lp_model.add(y_lp[p][i][j] + y_lp[i][p][j] + y_lp[i][j][p] + y_lp[q][j][i] + y_lp[j][q][i] + y_lp[j][i][q] == 1);
            }
}

}
/*

   void AllTripleFormulKemeny::AddTripleConstraints()
   {
   for (int i = 0; i < R.Candidates(); i++)
   for (int j = 0; j < R.Candidates(); j++)
   for (int k = 0; k < R.Candidates(); k++)
   if ( i!=j && i!=k && j!=k)
   {
   ip_model.add(y[i][j][k] - x[i][j] <= 0);
   ip_model.add(y[i][j][k] - x[j][k] <= 0);
   ip_model.add(y[i][j][k] - x[i][k] <= 0);

   lp_model.add(y[i][j][k] - x[i][j] <= 0);
   lp_model.add(y[i][j][k] - x[j][k] <= 0);
   lp_model.add(y[i][j][k] - x[i][k] <= 0);
   }
   for (int i = 0; i < R.Candidates()-2; i++)
   for (int j = i+1; j < R.Candidates()-1; j++)
   for (int k = j+1; k < R.Candidates(); k++)
   if ( i!=j && i!=k && j!=k)
   {
   ip_model.add(y[i][j][k] + y[i][k][j] + y[j][i][k] + y[j][k][i] + y[k][i][j] + y[k][j][i] == 1);
   lp_model.add(y[i][j][k] + y[i][k][j] + y[j][i][k] + y[j][k][i] + y[k][i][j] + y[k][j][i] == 1);

   }

   for (int i = 0; i < R.Candidates(); i++)
   for (int j = 0; j < R.Candidates(); j++)
   if (i !=j)
   for (int k = 0; k < R.Candidates(); k++)	
   if (k!=j)
   {
   ip_model.add(y[k][i][j] + y[i][k][j] + y[i][j][k] - x[i][j] == 0);
   lp_model.add(y[k][i][j] + y[i][k][j] + y[i][j][k] - x[i][j] == 0);

   }
   */	


void AllTripleFormulKemeny::SolveLP()
{
    try
    {

        IloExpr obj_fun = Lp_CreateObjFunction(); //generate the obj_function
        lp_model.add(IloMinimize(env, obj_fun));

        Lp_AddConstraints();   //add new constraints here
        lp_cplex.setOut(env.getNullStream());

        time_t start, end;
        time(&start);
        clock_t s,e;
        s = clock();
        // solve the model	
        cout<<"#######################start"<<endl;
        if ( !lp_cplex.solve() ) {
            env.error() << "Failed to optimize LP." << endl;
            throw (-1);
        }
        cout<<"####################solved"<<endl;
        e = clock();
        rt_lp = diffclock(e,s);
        env.out() << "All Triple formulation IP Solution status = " << lp_cplex.getStatus() << endl;
        env.out() << "All Triple formulation IP Solution value = " << lp_cplex.getObjValue() << endl;
        val_lp = lp_cplex.getObjValue();

        time(&end);
        rt_lp = difftime(end, start);
	lp_cplex.clearModel();

        //	PrintResults();
        //cplex.setParam(IloCplex::RootAlg, IloCplex::Primal);
        //cplex.setParam(IloCplex::NodeAlg, IloCplex::Primal);
    }
    catch ( IloException & e)
    {
        cerr << "Concert exception caught: " << e << endl;
    } catch (...)
    {
        cerr << "Unknown exception caught" << endl;
    }
}

/*

   double AllTripleFormulKemeny::Manual_Obj_Computation(IloCplex cplex)
   {
   double obj = 0; 

   for (int i = 0 ; i < R.Candidates(); i++)
   for (int j = 0; j < R.Candidates(); j++)
   obj += cplex.getValue(x[i][j]*R.Get_m_Array()[j][i]);

   return obj;
   }

*/


void AllTripleFormulKemeny::Lp_Iterat_AddConstraints()
    /****************************************************/
    /* This function add constarints to the formulation */
    /****************************************************/
{


    for(int i = 0; i < R.Candidates(); i++)
        for(int j =0; j < R.Candidates(); j++)
            for (int k = 0 ; k < R.Candidates(); k++)
                if (i == j || i == k || j == k)
                    lp_model.add(y_lp[i][j][k] == 0);
}

int AllTripleFormulKemeny::Exist_Cycle()
{
    int n;
    return n;
}

void AllTripleFormulKemeny::SolveLPIterat()
{
    try
    {

        IloExpr obj_fun = Lp_CreateObjFunction(); //generate the obj_function
        lp_model.add(IloMinimize(env, obj_fun));

        Lp_Iterat_AddConstraints();   //add new constraints here
        lp_cplex_iterat.setOut(env.getNullStream());

        //time_t start, end;
        //time(&start);
        clock_t s,e;
        s = clock();

        // solve the model	
        cout<<"#######################start"<<endl;
        if ( !lp_cplex.solve() ) {
            env.error() << "Failed to optimize LP." << endl;
            throw (-1);
        }
        while (Exist_Cycle() != 0)
        {		
            if ( !lp_cplex_iterat.solve() ) 
            {
                env.error() << "Failed to optimize LP." << endl;
                throw (-1);
            }
        }
        if ( !lp_cplex_iterat.solve() ) 
        {
            env.error() << "Failed to optimize LP." << endl;
            throw (-1);
        }
        cout<<"####################solved"<<endl;
        e = clock();
        rt_lp_iterat = diffclock(e,s);
        //env.out() << "All Triple formulation IP Solution status = " << lp_cplex.getStatus() << endl;
        //env.out() << "All Triple formulation IP Solution value = " << lp_cplex.getObjValue() << endl;
        val_lp = lp_cplex.getObjValue();
	    lp_cplex_iterat.clearModel();

        //time(&end);
        // rt_lp = difftime(end, start);

        //	PrintResults();
        //cplex.setParam(IloCplex::RootAlg, IloCplex::Primal);
        //cplex.setParam(IloCplex::NodeAlg, IloCplex::Primal);
    }
    catch ( IloException & e)
    {
        cerr << "Concert exception caught: " << e << endl;
    } catch (...)
    {
        cerr << "Unknown exception caught" << endl;
    }
}
