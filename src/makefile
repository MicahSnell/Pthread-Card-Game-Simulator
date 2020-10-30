play: pairWar.o
	g++ -o play pairWar.o -pthread -ansi -pedantic 

pairWar.o: pairWar.cpp
	g++ -c pairWar.cpp -std=c++11

clean:
	@rm -rf pairWar.o

cleanall:
	@rm -rf pairWar.o play
