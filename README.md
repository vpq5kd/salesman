Starter code and data for traveling salesman problem


Files in this directory:

* datareader.cpp : example code to read in the data files (use Makefile)
* datareader.py  : example code to read in the data files
* cities23.dat : list of coordinates for 23 cities in North America
* cities150.dat : 150 cities in North America
* cities1k.dat : 1207 cities in North America
* cities2k.dat : 2063 cities around the world
* routeplot.py : code to plot the globe and salesman's path<br>
usage:<br>
python routeplot.py cities.dat [cities2.dat] -r [="NA"],"World"'<br>
NA = North America, World = Mercator projection of the whole earth
* earth.C : (just for fun) plotting the globe in ROOT

***SALESMAN SOLUTIONS AND WRITEUP***

Salesman is run by doing the command ./salesman [T0] [Number of melting iterations] [number of iterations per temperature step] [target value] [{optional} TwoOpt]

Salesman is built by running the command "make" with the makefile in the github. Please ensure you have the appropriate dependencies installed on your machine prior to running. 

If the twoopt argument is not used the program will default to a less optimal method that swaps one city at a time. Originally chosing the city swap method organically, I came across the two opt method through a variety of google searches and youtube videos and found that it seemed to outperform my city swap method based on significant testing. To chose parameters I made some initial guesses and then tweaked them until I felt that the Distance Vs. Temperature graphs took the appropriate shape.
Cities150 used a T0 of 5000, 2000 melting iterations, 100 iterations per temp, and an arbitralily large target value so as to make the program only iterate once. The other two used the same parameters except a T0 of 8000 and 4000 melting iterations. 

*be advised* this program can be halted in the middle of execution by using the usual ctrl ^C command, however, it will wait until the current iteration of annealing is done before terminating.

**cities150** 317298.645290km 50640.727832km 40.95s

**ciites1k** 732177.726605km 221109.568843km 558.06s

**cities2k** 10187617.637342km 795546.519951km 1069.79s

