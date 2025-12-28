EXE ?= Simpler
CXX ?= g++
all:
	$(CXX) code/main.cpp -o $(EXE) -O3 -march=native

