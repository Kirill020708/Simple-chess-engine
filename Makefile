EXE ?= Simpler
CXX ?= g++
all:
	$(CXX) code/main.cpp -o $(EXE) -O2 -march=native

