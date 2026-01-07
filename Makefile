EXE ?= Simpler
CXX ?= clang++
all:
	$(CXX) code/main.cpp -o $(EXE) -O3 -std=c++2a -march=native -pthread -flto -fno-exceptions -fno-rtti -DNDEBUG -ffast-math -funroll-loops

