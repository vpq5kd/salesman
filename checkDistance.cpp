// example code to read in a data file of city lat,long coordinates

#include <cstdio>
#include <cstdlib>
#include <cmath>
// simple structure to store city coordinates
// could also use std::pair<double> 
// or define a class

typedef struct {
  double lon, lat;
} COORD;

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
	return totalRouteDistance;
}
int main(int argc, char *argv[]){
  const int NMAX=2500;
  COORD cities[NMAX];

  if (argc<2){
    printf("Please provide a data file path as argument\n");
    return 1;
  }

  int ncity=GetData(argv[1],cities);
  printf("Read %d cities from data file\n",ncity);
  printf("Total distance of provided file: %lf\n", totalDistance(cities, ncity));
  return 0;
}

