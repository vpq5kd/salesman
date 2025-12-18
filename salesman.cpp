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
#include <time.h>
#include "TCanvas.h"
#include "TGraph.h"
#include "TApplication.h"
#include "TAxis.h"

using namespace std;
// simple structure to store city coordinates
// could also use std::pair<double> 
// or define a class

typedef struct {
  double lon, lat;
} COORD;

//bool that defines when to exit
bool killSwitch = false; 
bool reachedEnd = false; 

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
	if(reachedEnd) {exit(0);}

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


//computes distance using haversine formula provided in project description
double computeDistance(double lat1, double lon1, double lat2, double lon2){
	double R = 6371.0;
	double deltaLat = (lat2 - lat1)*M_PI/180.0;
	double deltaLon = (lon2 - lon1)*M_PI/180.0;
	double a = (sin(deltaLat/2)*sin(deltaLat/2)) + (cos(lat1*M_PI/180.0)*cos(lat2*M_PI/180.0)*(sin(deltaLon/2)*sin(deltaLon/2)));
	double c = 2*atan2(sqrt(a),sqrt(1-a));

	return R*c;
}

//function defined after bulk of code was written. Used to make new functions easier to write. 
double computeDistanceCities(const COORD city1, const COORD city2){
	return computeDistance(city1.lat, city1.lon, city2.lat, city2.lon);
}

//function that allows you to avoid recalculating totaldistance in TwoOpt method
double deltaDistTwoOpt(const COORD cities [], int ncity, int randCity1, int randCity2){
	COORD cityA = cities[randCity1];
	COORD cityB = cities[randCity1+1];
	COORD cityC = cities[randCity2];
	COORD cityD = cities[(randCity2 + 1) % ncity];
	
	double oldDist = computeDistanceCities(cityA, cityB) + computeDistanceCities(cityC, cityD);
	double newDist = computeDistanceCities(cityA, cityC) + computeDistanceCities(cityB, cityD);

	double deltaDist = newDist-oldDist;

	return deltaDist;
	
}
//function that allows you to avoid recalculating totaldistance in the city swap method

double deltaDistCitySwap(const COORD cities [], int ncity, int randCity1, int randCity2){
	if (randCity1 > randCity2){
		swap(randCity1, randCity2);
	}
	COORD city1 = cities[randCity1];
	COORD city1a = cities[(randCity1-1 + ncity)%ncity];
	COORD city1b = cities[(randCity1+1)];
	COORD city2 = cities[randCity2];	
	COORD city2a = cities[randCity2 -1];
	COORD city2b = cities[(randCity2 + 1) % ncity];
	
	double oldDist = computeDistanceCities(city1a, city1) + computeDistanceCities(city1, city1b) + computeDistanceCities(city2a, city2) + computeDistanceCities(city2, city2b);
	double newDist = computeDistanceCities(city1a, city2) + computeDistanceCities(city2, city1b) + computeDistanceCities(city2a, city1) + computeDistanceCities(city1, city2b);

	double deltaDist = newDist-oldDist;

	return deltaDist;
	
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

//melt function
void melt(COORD cities [], int ncity, double T0, double iterationsPerTemperature){

	double oldDist = totalDistance(cities, ncity);
	double newDist = 0.0; 
	for (int i = 0; i<iterationsPerTemperature; i++){
		
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
void simulatedAnnealingCitySwap(COORD cities [], double temperatureArray [], double distanceArray [], double *dataPoints,  int ncity, double T0, double iterationsPerTemperature){
	printf("Running city swap formula\n");
	double T = T0;
	double oldDist = totalDistance(cities, ncity);
	//double newDist = 0.0;

	int arrayIterator = 0;

	double Tmin = 1e-6;
	double alpha = 0.995; 
	while(T>Tmin){
		int iters = iterationsPerTemperature * (T0/T);
		if (iters > 100*ncity){
			iters = 100*ncity;
		}
		for (int i = 0; i < iters; i++){
			int randCity1 = randInt(0,ncity-1);
			int randCity2 = randInt(0,ncity-1);
			while (randCity2 == randCity1){
				randCity2 = randInt(0, ncity-1);
			}

			//newDist = totalDistance(cities, ncity);

			double deltaDist = deltaDistCitySwap(cities, ncity, randCity1, randCity2); 
			
			if (deltaDist < 0){
				oldDist += deltaDist;
				swap(cities[randCity1], cities[randCity2]);
				continue;
			}
			else {
				double p = exp(-deltaDist/T);
				double r = randDouble(0.0,1.0);

				if (r < p){
					oldDist += deltaDist;
					swap(cities[randCity1], cities[randCity2]);
					continue;
				}
			
			}
		}
		temperatureArray[arrayIterator] = T;
		distanceArray[arrayIterator] = oldDist;
		arrayIterator += 1;
		T*=alpha;	
	}
	*dataPoints = arrayIterator;
}

//simulated annealing logic using the traditional two-opt method at random indeces.
void simulatedAnnealingTwoOpt(COORD cities [], double temperatureArray [], double distanceArray[], double * dataPoints,  int ncity, double T0, double iterationsPerTemperature){
	printf("Running twoopt formula\n"); 
	double T = T0;
	double oldDist = totalDistance(cities, ncity);
	//double newDist = 0.0;

	int distanceArrayIterator = 0;
	int temperatureArrayIterator = 0;

 	double tMin = 1e-6;
	double alpha = .995;	
	while(T>tMin){
		int iters = iterationsPerTemperature * (T0/T);
		if (iters > 100*ncity){
			iters = 100*ncity;
		}
		for (int i = 0; i < iters; i++){
			int randCity1 = randInt(1,ncity-3);
			int randCity2 = randInt(randCity1 + 2,ncity-1);

			double deltaDist = deltaDistTwoOpt(cities, ncity, randCity1, randCity2);
			
			if (deltaDist < 0){

				reverse(cities + randCity1 + 1, cities + randCity2 + 1);
				oldDist += deltaDist;
				continue;
			}

			else {
				double p = exp(-deltaDist/T);
				double r = randDouble(0.0,1.0);

				if (r < p){

					reverse(cities + randCity1 + 1, cities + randCity2 + 1);
					oldDist += deltaDist;
					continue;
				}
			}
		}
		distanceArray[distanceArrayIterator] = oldDist;
		temperatureArray[temperatureArrayIterator] = T;
		distanceArrayIterator += 1; 
		temperatureArrayIterator +=1;
		T*=alpha; 	
	}
	*dataPoints = temperatureArrayIterator;
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

//function to calculate T0
double calculateT0(COORD cities [],int ncity){

	COORD citiesCopy[3000];
	copy(cities, cities +ncity, citiesCopy); 
	double deltaDistMax = 0.0;
	double newDist = 0.0;
       	double oldDist = totalDistance(cities,ncity);	
	for (int i = 0; i<100*ncity; i++){
		int randCity1 = randInt(0,ncity-1);
		int randCity2 = randInt(0,ncity-1);
		while (randCity2 == randCity1){
			randCity2 = randInt(0, ncity-1);
		}
		swap(citiesCopy[randCity1], citiesCopy[randCity2]);

		newDist = totalDistance(citiesCopy, ncity);

		double deltaDist = newDist - oldDist;
		if (deltaDist > deltaDistMax){
			deltaDistMax = deltaDist;
		}	
		oldDist = newDist; 
	}
	return 100 +deltaDistMax;
}


int main(int argc, char *argv[]){

  clock_t tStart = clock();
  const int NMAX=2500;
  COORD cities[NMAX];
  


  signal(SIGINT, signalHandler);

  if (argc<2){
    printf("You did not provide the appropriate arguments. Usage is as follows: [files.dat] [target value] [(optional} TwoOpt]]\n");
    return 1;
  }

  double targetDistance = atoi(argv[2]); 
  const char * algorithmType;
  double dataPoints; 
  
  string citySwapVariableName = "City Swap";

  if (argc > 3) {algorithmType = argv[3];} else {algorithmType = citySwapVariableName.c_str();}
  


  string fileName = argv[1]; 
  int ncity=GetData(argv[1],cities);
 
  double meltingIterations = 100*ncity;
  double T0 = calculateT0(cities, ncity);
  double iterationsPerTemperature = 10*ncity;   
  printf("\nYou are running the %s algorithm with the following paramers:\nT0 = %d\nTarget distance = %d\n", algorithmType,(int) T0,(int) targetDistance);

  double * temperatureArray = (double *) malloc(T0*1000*sizeof(double));
  double * distanceArray = (double *) malloc(T0*1000*sizeof(double));
  
  printf("Read %d cities from data file\n",ncity);
  printf("Distance of initially provided path: %lf\n", totalDistance(cities, ncity)); 


  melt(cities, ncity, T0, meltingIterations);

  double bestDistance = 0.0;
  COORD bestCity[ncity]; 

  if (strcmp("TwoOpt", algorithmType)==0){
	simulatedAnnealingTwoOpt(cities, temperatureArray, distanceArray, &dataPoints, ncity, T0, iterationsPerTemperature);
	bestDistance = totalDistance(cities, ncity);
	copy(cities, cities + ncity, bestCity);
	while ((totalDistance(cities, ncity) > targetDistance)&& !killSwitch){
		printf("Target distance of %lf not reached, currecnt best distance = %lf\n", targetDistance, bestDistance);

		melt(cities, ncity, T0, meltingIterations);
		simulatedAnnealingTwoOpt(cities, temperatureArray, distanceArray, &dataPoints, ncity, T0, iterationsPerTemperature);

		double testDistance = totalDistance(cities, ncity);
		printf("Most recent calculated distance %lf\n", testDistance);
		if (testDistance  < bestDistance){
			bestDistance = testDistance;
			copy(cities, cities + ncity, bestCity);
		}	
	}	
  }

  else{ 
	simulatedAnnealingCitySwap(cities,temperatureArray, distanceArray, &dataPoints, ncity, T0, iterationsPerTemperature);
	bestDistance = totalDistance(cities, ncity);
	copy(cities, cities + ncity, bestCity);
	while ((totalDistance(cities, ncity) > targetDistance)&& !killSwitch){
		printf("Target distance of %lf not reached, currecnt best distance = %lf\n", targetDistance, bestDistance);
		
		melt(cities, ncity, T0, meltingIterations);
		simulatedAnnealingCitySwap(cities, temperatureArray, distanceArray, &dataPoints, ncity, T0, iterationsPerTemperature);
		
		double testDistance = totalDistance(cities, ncity);
		printf("Most recent calculated distance %lf\n", testDistance);
		if (testDistance  < bestDistance){
			bestDistance = testDistance;
			copy(cities, cities + ncity, bestCity);
		}	
	}	

  }

  printf("Final best distance found: %lf\n", bestDistance);
  printf("Time taken for solution execution: %.2fs\n", (double) (clock() - tStart)/CLOCKS_PER_SEC);

  string dataFileName = "cities" + to_string(ncity) + "_optimal.dat";  
  string distanceVsTimeFileName = "an" + to_string(ncity) + ".png";
  writeRoute(dataFileName.c_str(), bestCity, ncity);

  reachedEnd = true; 
  TApplication app("app", &argc, argv);
  TCanvas *c = new TCanvas("c", "Statistics Canvas", 1000, 800);
  TGraph *g = new TGraph(dataPoints, temperatureArray, distanceArray);

  //c->SetLeftMargin(0.20);
  //c->SetRightMargin(0.20);
  //c->SetBottomMargin(0.20);
  //c->SetTopMargin(0.20);

  string graphTitle = string("Distance Vs. Temperature of ") +  fileName; 
  g->SetTitle(graphTitle.c_str());
  g->GetYaxis()->SetTitle("Distance (km)");
  g->GetYaxis()->CenterTitle();
  g->GetYaxis()->SetTitleOffset(1.4);

  g->GetXaxis()->SetTitle("Temperature");
  g->GetXaxis()->CenterTitle();
  g->GetXaxis()->SetTitleOffset(1.4);

  g->GetXaxis()->SetMaxDigits(3);
  g->Draw("AP");

  c->SetLogx();
  c->Update();
  c->SaveAs(distanceVsTimeFileName.c_str());

  app.Run();
  free(temperatureArray);
  free(distanceArray);
  return 0;
}

