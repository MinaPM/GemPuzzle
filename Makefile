all: compile link run

compile:
	g++ -c main.cpp -I".\src\include"

link:
	g++ main.o -o gem_puzzle -L".\src\lib" -lsfml-graphics -lsfml-audio -lsfml-window  -lsfml-system

run:
	.\gem_puzzle