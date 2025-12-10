// example code to read in a data file of city lat,long coordinates

#include <cstdio>
#include <cstdlib>
#include <cmath>

using namespace std;
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


//computes great circle distance between two points  using the haversine formula
double computeDistance(double lat1, double lon1, double lat2, doulbe lon2){
	double R = 6371.0
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

int main(int argc, char *argv[]){
  const int NMAX=2500;
  COORD cities[NMAX];

  if (argc<2){
    printf("Please provide a data file path as argument\n");
    return 1;
  }

  int ncity=GetData(argv[1],cities);
  printf("Read %d cities from data file\n",ncity);
  printf("Longitude  Latitude\n");
  for (int i=0; i<ncity; i++)
    printf("%lf %lf\n",	cities[i].lon,cities[i].lat); 
  return 0;
}

