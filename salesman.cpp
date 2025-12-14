// example code to read in a data file of city lat,long coordinates

#include <string> 
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <random>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <csignal>
using namespace std;
// simple structure to store city coordinates
// could also use std::pair<double> 
// or define a class

typedef struct {
  double lon, lat;
} COORD;

//bool that defines when to exit
bool killSwitch = false; 

//random number generator device
mt19937 gen(random_device{}());
int randInt(int a, int b){
	uniform_int_distribution <> d(a,b);
	return d(gen);
}


double randDouble(double a, double b){
	uniform_real_distribution <> d(a,b);
	return d(gen);
}

//signal handler for interrupt exit
void signalHandler(int sig){
	printf("\nProgram will exit momentarily with the best solution found so far!\n");
	killSwitch = true;
}

// fill the array of city locations
int GetData(char* fname, COORD *cities){
  FILE* fp=fopen(fname,"r");
  const int bufsiz=1000;
  char line[bufsiz+1];
  int ncity=0;
  while(1){
    fgets(line,bufsiz,fp);
    if (line[0]=='#') continue;  // skip comments
    if (feof(fp)) break;
    // we only scan for two numbers at start of each line
    sscanf(line,"%lf %lf",&cities[ncity].lon,&cities[ncity].lat);    
    ncity++;
  }
  fclose(fp);
  return ncity;
}


//computes great circle distance between two points  using the haversine formula
double computeDistance(double lat1, double lon1, double lat2, double lon2){
	double R = 6371.0;
	double phi1 = lat1 * M_PI/180.0;
	double phi2 = lat2 * M_PI/180.0;
	double lam1 = lon1 * M_PI/180.0;
	double lam2 = lon2 * M_PI/180.0;

	double dphi = phi2 - phi1;
	double dlam = lam2 - lam1;

	double a = (sin(dphi/2) * sin(dphi/2)) + (cos(phi1)*cos(phi2)*sin(dlam/2)*sin(dlam/2));
	double d = 2*R*asin(sqrt(a));
	
	return d;
}


//function that computes total distance of an array of cities
double totalDistance(const COORD cities[], int ncities){
	double totalRouteDistance = 0.0;
	for (int i = 0; i<ncities-1; i++){
		double lat1 = cities[i].lat;
		double lon1 = cities[i].lon;
		double lat2 = cities[i+1].lat;
		double lon2 = cities[i+1].lon;
		double localDistance = computeDistance(lat1, lon1, lat2, lon2);
		totalRouteDistance += localDistance;
	}

	double latStart = cities[0].lat;
	double lonStart = cities[0].lon;
	double latEnd = cities[ncities-1].lat;
	double lonEnd = cities[ncities-1].lon;
	double retDistance = computeDistance(latStart, lonStart, latEnd, lonEnd);
	totalRouteDistance+= retDistance;

	return totalRouteDistance ;
}

//function to swap two cities (used for melting)
void randCitySwap(COORD cities [], int ncity){
	int randCity1 = randInt(0,ncity-1);
	int randCity2 = randInt(0,ncity-1);
	while (randCity2 == randCity1){
		randCity2 = randInt(0, ncity-1);
	}
	swap(cities[randCity1], cities[randCity2]);
}

//melt function
void melt(COORD cities [], int ncity, double T0, double numIterations){

	double oldDist = totalDistance(cities, ncity);
	double newDist = 0.0; 
	for (int i = 0; i<numIterations; i++){
		
		int randCity1 = randInt(0,ncity-1);
		int randCity2 = randInt(0,ncity-1);
		while (randCity2 == randCity1){
			randCity2 = randInt(0, ncity-1);
		}
		swap(cities[randCity1], cities[randCity2]);

		newDist = totalDistance(cities, ncity);

		double deltaDist = newDist - oldDist;
		
		if (deltaDist < 0){
			oldDist = newDist;
			continue;
		}
		else {
			double p = exp(-deltaDist/T0);
			double r = randDouble(0.0,1.0);

			if (r < p){
				oldDist = newDist;
				continue;
			}
			swap(cities[randCity1], cities[randCity2]);
		
		}

	}
	printf("Finished melting\n");
}

//smulated annealing logic using the method of swapping two random cities.
void simulatedAnnealingCitySwap(COORD cities [], int ncity, double T0, double numIterations){
	printf("Running city swap formula\n");
	double T = T0;
	double oldDist = totalDistance(cities, ncity);
	double newDist = 0.0;
	while(T>0){
		for (int i = 0; i < numIterations; i++){
			int randCity1 = randInt(0,ncity-1);
			int randCity2 = randInt(0,ncity-1);
			while (randCity2 == randCity1){
				randCity2 = randInt(0, ncity-1);
			}
			swap(cities[randCity1], cities[randCity2]);

			newDist = totalDistance(cities, ncity);

			double deltaDist = newDist - oldDist;
			
			if (deltaDist < 0){
				oldDist = newDist;
				continue;
			}
			else {
				double p = exp(-deltaDist/T);
				double r = randDouble(0.0,1.0);

				if (r < p){
					oldDist = newDist;
					continue;
				}
				swap(cities[randCity1], cities[randCity2]);
			
			}
		}
		T-= 0.1;	
	}
}

//simulated annealing logic using the traditional two-opt method at random indeces.
void simulatedAnnealingTwoOpt(COORD cities [], int ncity, double T0, double numIterations){
	printf("Running twoopt formula\n"); 
	double T = T0;
	double oldDist = totalDistance(cities, ncity);
	double newDist = 0.0;
	while(T>0){
		for (int i = 0; i < numIterations; i++){
			int randCity1 = randInt(1,ncity-3);
			int randCity2 = randInt(randCity1 + 2,ncity-1);
			reverse(cities + randCity1 + 1, cities + randCity2 + 1);
			newDist = totalDistance(cities, ncity);

			double deltaDist = newDist - oldDist;
			
			if (deltaDist < 0){
				oldDist = newDist;
				continue;
			}
			else {
				double p = exp(-deltaDist/T);
				double r = randDouble(0.0,1.0);

				if (r < p){
					oldDist = newDist;
					continue;
				}
				reverse(cities + randCity1 + 1, cities + randCity2 + 1);	
			}
		}
		T-= 0.1;	
	}
}

//saves the route to an output file 
void writeRoute(const char * filename, const COORD cities [], int ncity){
	ofstream out(filename);
	for (int i = 0; i<ncity; i++){
		out << cities[i].lon << " " << cities[i].lat << "\n";
	}
	out.close();
	printf("Saved final route to %s\n", filename);
}

int main(int argc, char *argv[]){
  const int NMAX=2500;
  COORD cities[NMAX];

  signal(SIGINT, signalHandler);

  if (argc<5){
    printf("You did not provide the appropriate arguments. Usage is as follows: [files.dat] [T0] [# Internal Iterations] [target value] [(optional} TwoOpt]]\n");
    return 1;
  }

  double T0 = atoi(argv[2]); //second argument value is the initial temperature
  double numIterations = atoi(argv[3]); //third argument value is the number of iterations per temperature step.
  double targetDistance = atoi(argv[4]); //fourth argument value is upper, non-inclusive, threshold for distance
  const char * algorithmType;

  string citySwapVariableName = "City Swap";

  if (argc > 5) {algorithmType = argv[5];} else {algorithmType = citySwapVariableName.c_str();}
  

  printf("You are running the %s algorithm with the following paramers:\nT0 = %d\nNumber of iterations per temperature = %d\nTarget distance = %d\n", algorithmType,(int) T0, (int) numIterations,(int) targetDistance);

  int ncity=GetData(argv[1],cities);
  printf("Read %d cities from data file\n",ncity);
  printf("Distance of initially provided path: %lf\n", totalDistance(cities, ncity)); 


  melt(cities, ncity, T0, 1000);
  printf("Distance of path after melting: %lf\n", totalDistance(cities, ncity));

  double bestDistance = 0.0;
  COORD bestCity[ncity]; 
  if (argc > 5 && strcmp("TwoOpt", argv[5])==0){
	simulatedAnnealingTwoOpt(cities, ncity, T0, numIterations);
	bestDistance = totalDistance(cities, ncity);
	copy(cities, cities + ncity, bestCity);
	while ((totalDistance(cities, ncity) > targetDistance)&& !killSwitch){
		printf("Target distance of %lf not reached, currecnt best distance = %lf\n", targetDistance, bestDistance);

		melt(cities, ncity, T0, 1000);
		simulatedAnnealingTwoOpt(cities, ncity, T0, numIterations);

		double testDistance = totalDistance(cities, ncity);
		printf("Most recent calculated distance %lf\n", testDistance);
		if (testDistance  < bestDistance){
			bestDistance = testDistance;
			copy(cities, cities + ncity, bestCity);
		}	
	}	
  }

  else{ 
	simulatedAnnealingCitySwap(cities, ncity, T0, numIterations);
	bestDistance = totalDistance(cities, ncity);
	copy(cities, cities + ncity, bestCity);
	while ((totalDistance(cities, ncity) > targetDistance)&& !killSwitch){
		printf("Target distance of %lf not reached, currecnt best distance = %lf\n", targetDistance, bestDistance);
		
		melt(cities, ncity, T0, 1000);
		simulatedAnnealingCitySwap(cities, ncity, T0, numIterations);
		
		double testDistance = totalDistance(cities, ncity);
		printf("Most recent calculated distance %lf\n", testDistance);
		if (testDistance  < bestDistance){
			bestDistance = testDistance;
			copy(cities, cities + ncity, bestCity);
		}	
	}	

  }

  printf("Final best distance found: %lf\n", bestDistance);

  string filename = "cities" + to_string(ncity) + "_optimal.dat";  
  writeRoute(filename.c_str(), bestCity, ncity);

  return 0;
}

