default: all

all:
	# "building city data reader example"
	g++ -O -Wall -o datareader datareader.cpp
	g++ -O -Wall -o salesman salesman.cpp

clean:
	rm -f datareader *~ *png
