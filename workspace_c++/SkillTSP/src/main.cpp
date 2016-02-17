#include<iostream>


#include "network.h"
#include "solver.h"
using namespace std;
int main()
{
    Network net;
    net.ReadNetwork();
    Solver s(net);
	s.SolveLP(); 
	cout << "end of program" << endl;
	return 0;

}
