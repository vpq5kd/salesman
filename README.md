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

Salesman is run by executing the command `./sales [-n||target value]`

* if the `-n` flag is chosen, the program will not iterate until it hits a target value but will instead exit after one full run.
* **Note:** with high `ncity` values the code hangs for a few seconds directly after execution. At this time I believe that this has something to do with the compiler loading information into memory, but I'm not entirely sure. 

Salesman is built by running the command `make` with the makefile in the github. Please ensure you have the appropriate dependencies installed on your machine prior to running. 


In terms of parameter choices, I orginally had manual control over `T0`, the number of melting iterations, and the number of iterations per temperature step. This, however, has changed. My current algorithm chooses `T0` by making a bunch of random swaps and finding the maximum dL = L_new - L_old, then setting `T0` to 100 + (max(dL)). For the melting step, I hard-coded the number of iterations to be `100*ncity`. Further, for the number of iterations per temperature step I changed from a discrete, static, self-selected number to a scalable value that changes with each temperature step ranging from `10*ncity*(T0/T)` <= `100*ncity`, where `T`, is the current temperature step. I also found that scaling `T` by  `T*=alpha`, where `alpha=0.995`, allowed more finite temperature steps and lead to a more optimal solution. This annealing schedule change was based on a recommendation from chatGPT following a prompt regarding techniques in simullated annealing to reliably find more optimal solutions.

**Other Optimizatons & Changes:**

* `deltaDist` was changed in the TwoOpt formula from being calculated by recalculating the distance before and after the reversal to only calculating the specific distances needed to find the delta. This edit resulted in a O(N) -> O(1) optimization! 
* Deprecatated `simulatedAnnealingCitySwap` entirely due to inneficiency and a lack of being able to achieve better solutions that `simulatedAnnealingTwoOpt`.

*be advised* this program can be halted in the middle of execution by using the usual ctrl ^C command, however, it will wait until the current iteration of annealing is done before terminating.

**Final Map:**

The final map is created using `routeplot.py ` with the minor edit of the textbox addition. In order to streamline the process of adding the distances to the plot, I made the distance a "city" in my .dat file and processed it seperately in routeplot.py. As such, if you are attempting to run one of my `*_optimal.dat` files in your own plotting framework, either remove the first line of said file (it will have a "km" as its first two characters), or be sure to error handle approrpriately. 

**cities150** 317299.0km 49374.6km 14.93s

**ciites1k** 732178.0km 96068.9km 131.94s

**cities2k** 10187600.0km 286906.0km 285.61s

--------------------------------------------
**Disclaimer:** chatGPT was used in the development of this program in a similar manner to the traditional Google search or Stack Overflow query. That is, of course, for edits and assistance with syntax, coding convetions, c/cpp standard functions, etc. Except where otherwise noted, it was not used for the development of original thoughts or algorithmic code. All of my code can be explained thoroughly by me if necessary. 
