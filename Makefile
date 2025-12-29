EXE ?= Simpler
CXX ?= g++
all:
	$(CXX) code/main.cpp -o $(EXE) -O3 -std=c++2a -march=native -pthread

