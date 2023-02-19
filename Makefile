practica2: practica2.o
	g++ -o practica2 practica2.o

practica2.o: practica2.cpp practica2.h
	g++ -c -Wall practica2.cpp
