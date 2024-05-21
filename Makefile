all: compile link run

compile:
	g++ -c ./app/main.cpp -o ./app/main.o -I".\src\include"

link:
	g++ ./app/main.o -o ./app/gem_puzzle -L".\src\lib" -lsfml-graphics -lsfml-audio -lsfml-window  -lsfml-system

run:
	.\app\gem_puzzle