# Makefile for stats
stats : stats.cpp dataIO.cpp progress.cpp
	g++ -std=c++11 stats.cpp dataIO.cpp progress.cpp -o stats 

clean :
	rm *.o