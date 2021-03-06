all: main

main: main.o Environnement.o Individu.o Simulation.o
	g++ main.o Environnement.o Individu.o Simulation.o -o main -s -O4

main.o : main.cpp Environnement.h Individu.h Simulation.h
	g++ -c 	main.cpp -o main.o -std=c++11 -s -O4

Environnement.o : Environnement.cpp Environnement.h
	g++ -c Environnement.cpp -o Environnement.o -std=c++11 -s -O4

Individu.o: Individu.cpp Individu.h
	g++ -c Individu.cpp -o Individu.o -std=c++11 -s -O4

Simulation.o: Simulation.cpp Simulation.h
	g++ -c Simulation.cpp -o Simulation.o -std=c++11 -s -O4

clean :
	rm *.o
