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

Salesman is run by executing the command `./salesman [target value] [{optional} TwoOpt]`

Salesman is built by running the command `make` with the makefile in the github. Please ensure you have the appropriate dependencies installed on your machine prior to running. 

If the `TwoOpt` argument is not used the program will default to a less optimal method that swaps one city at a time. Originally chosing the city swap method as my perturbation method, I came across the two opt method through a variety of google searches and youtube videos and found that it seemed to outperform my city swap method based on significant testing. I will qualify that, however, I noticed very little difference between the TwoOpt case at the low `ncity` values of 23 and 150. 

In terms of parameter choices, I orginally had manual control over `T0`, the number of melting iterations, and the number of iterations per temperature step. This, however, has changed. My current algorithm chooses `T0` by making a bunch of random swaps and finding the maximum dL = L_new - L_old, then setting `T0` to 100 + (max(dL)). For the melting step, I hard-coded the number of iterations to be `100*ncity`. Further, for the number of iterations per temperature step I changed from a discrete, static, self-selected number to a scalable value that changes with each temperature step ranging from `10*ncity*(T0/T)` <= `100*ncity`, where `T`, is the current temperature step. I also found that scaling `T` by  `T*=alpha`, where `alpha=0.995`, allowed more finite temperature steps and lead to a more optimal solution. This annealing schedule change was based on a recommendation from chatGPT following a prompt regarding techniques in SA to reliably find more optimal solutions.

**Other Optimizatons:**

* `deltaDist` was changed in the TwoOpt formula from being calculated by recalculating the distance before and after the reversal, now it only caculates the needed changes and returns, which is a O(N) -> O(1) optimization!  - this change has not yet been made in the city swap method so that one still hangs for a while.

*be advised* this program can be halted in the middle of execution by using the usual ctrl ^C command, however, it will wait until the current iteration of annealing is done before terminating.

**cities150** 317298.645290km 48577.332461km 11.82s

**ciites1k** 732177.726605km 96226.304264km 15609.72s

**cities2k** 10187617.637342km 795546.519951km 1069.79s

