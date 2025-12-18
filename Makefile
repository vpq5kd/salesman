default: all

CPPFLAGS = -O -Wall $(shell root-config --cflags)
LDLIBS = $(shell root-config --libs)

all: salesman checkDistance datareader

salesman: salesman.cpp
	g++ $(CPPFLAGS) -o salesman salesman.cpp $(LDLIBS)
datareader: datareader.cpp
	g++ -O -Wall -o datareader datareader.cpp
checkDistance: checkDistance.cpp
	g++ -O -Wall -o checkDistance checkDistance.cpp
clean:
	rm -f datareader *png *pdf *_optimal.dat salesman checkDistance
