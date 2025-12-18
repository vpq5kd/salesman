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

Salesman is run by executing the command `./salesman [-n||target value]`

* if the `-n` flag is chosen, the program will not iterate until it hits a target value but will instead exit after one full run. 

Salesman is built by running the command `make` with the makefile in the github. Please ensure you have the appropriate dependencies installed on your machine prior to running. 


In terms of parameter choices, I orginally had manual control over `T0`, the number of melting iterations, and the number of iterations per temperature step. This, however, has changed. My current algorithm chooses `T0` by making a bunch of random swaps and finding the maximum dL = L_new - L_old, then setting `T0` to 100 + (max(dL)). For the melting step, I hard-coded the number of iterations to be `100*ncity`. Further, for the number of iterations per temperature step I changed from a discrete, static, self-selected number to a scalable value that changes with each temperature step ranging from `10*ncity*(T0/T)` <= `100*ncity`, where `T`, is the current temperature step. I also found that scaling `T` by  `T*=alpha`, where `alpha=0.995`, allowed more finite temperature steps and lead to a more optimal solution. This annealing schedule change was based on a recommendation from chatGPT following a prompt regarding techniques in simullated annealing to reliably find more optimal solutions.

**Other Optimizatons & Changes:**

* `deltaDist` was changed in the TwoOpt formula from being calculated by recalculating the distance before and after the reversal, now it only caculates the needed changes and returns, which is a O(N) -> O(1) optimization! 
* deprecatated `simulatedAnnealingCitySwap` entirely due to inneficiency and a lack of being able to achieve better solutions that `simulatedAnnealingTwoOpt`.

*be advised* this program can be halted in the middle of execution by using the usual ctrl ^C command, however, it will wait until the current iteration of annealing is done before terminating.

**cities150** 317298.645290km 49783.180354km 15.09s

**ciites1k** 732177.726605km 95863.791658km 130.02s

**cities2k** 10187617.637342km 283626.488279km 275.74s

