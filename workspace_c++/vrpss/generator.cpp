#include <iostream>
#include <fstream>
#include <cstdlib>
#include <math.h>

using namespace std;


static double tmp[1000][1000];
static int tmpskill[100][100];
static int skilllow[1000];
static int skillhigh[1000];
static int locationx[1000];
static int locationy[1000];

void SkillGenerator()
{
	ofstream fout;
	fout.open("tsp1.txt");
	//srand ( time(NULL) );
	int num_job = 6;
	int num_worker = 3;

	fout<<num_job<<" "<<num_worker<<endl;



	for(int i = 0; i < num_worker; i++){
		int low = rand()%4+1;
		int high = rand()%6 + 5;
		while(high <= low)
			high = rand()%10 + 1;

		skilllow[i] = low;
		skillhigh[i] = high;
	}

	for(int i = 0; i < num_job; i++){
		int skill = rand()%10 + 1;
		while(true){
			int j;
			for(j = 0; j < num_worker; j++){
				if(skill >= skilllow[j] && skill <= skillhigh[j])
					break;
			}
			if(j == num_worker)
				skill = rand()%10 + 1;
			else{
				fout<<i+1<<" "<<skill<<endl;
				break;
			}
		}
	}
	for(int i = 0; i < num_worker; i++)
		fout<<skilllow[i]<<" "<<skillhigh[i]<<endl;

	/*for(int i = 0; i < 2; i++){
		fout<<"1"<<" "<<"10"<<endl;
	}*/

	for(int i = 1; i <= num_job; i++){
		locationx[i] = rand()%100 + 1;
		locationy[i] = rand()%100 + 1;
	}

	for(int i = 0; i <= num_job; i++){
		for(int j = i + 1; j <= num_job; j++){
			double distance = pow(pow(locationx[i] - locationx[j], 2) + pow(locationy[i] - locationy[j],2), 0.5);
			tmp[i][j] = distance;
			tmp[j][i] = distance;
		}
	}

	for(int i = 0; i <= num_job; i++){
		for(int j = 0; j <=num_job; j++){
			if(i == j)
				fout<<"0"<<" ";
			else{
				fout<<tmp[i][j]<<" ";
			}
		}
		fout<<endl;
	}
}


void KMSTGenerator(){
	ofstream fout;
	fout.open("tsp1.txt");
	srand ( time(NULL) );
	int num_job =10;
	int num_worker = 4;

	fout<<num_job<<" "<<num_worker<<endl;


	for(int i = 1; i <= num_job; i++){
		locationx[i] = rand()%100 + 1;
		locationy[i] = rand()%100 + 1;
	}

	for(int i = 0; i <= num_job; i++){
		for(int j = i + 1; j <= num_job; j++){
			double distance = pow(pow(locationx[i] - locationx[j], 2) + pow(locationy[i] - locationy[j],2), 0.5);
			tmp[i][j] = distance;
			tmp[j][i] = distance;
		}
	}

	for(int i = 0; i <= num_job; i++){
		for(int j = 0; j <=num_job; j++){
			if(i == j)
				fout<<"0"<<" ";
			else{
				fout<<tmp[i][j]<<" ";
			}
		}
		fout<<endl;
	}
}


int main(int argc, char* argv[])
{
	int seed;
	srand(100);
	SkillGenerator();
	//KMSTGenerator();
	return 0;
}



