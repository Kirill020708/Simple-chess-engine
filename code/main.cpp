#ifndef BBB
#define BBB

#include "searcher.h"

#endif /* BBB */

string squareToString(int square){
	return string(1,char((square&7)+'a'))+string(1,char(7-(square>>3)+'1'));
}

void printDesk01(ull x){
	for(ll i=0;i<64;i++){
		cout<<((x&(1ull<<i))>0);
		if(i%8==7)
			cout<<'\n';
	}
}

int main(){
	// board.initFromFEN("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
	// board.makeMove(Move("e2e4"));
	// board.makeMove(Move("a7a6"));
	// cout<<board.evaluation<<'\n';
	searcher.iterativeDeepeningSearch(WHITE,100);
	// perfTest();
}


/*

cd /Users/Apple/Desktop/projects/chessEngv2/Simple-chess-engine/code
c++ main.cpp -o app -std=c++2a -O2

*/