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
	// board.initFromFEN("8/k7/3p4/p2P1p2/P2P1P2/8/8/K7 w - - 0 1");
	// board.makeMove(Move("e2e4"));
	// board.makeMove(Move("a7a6"));
	// cout<<board.evaluation<<'\n';
	// cout<<ttt.tableSize<<'\n';
	searcher.iterativeDeepeningSearch(WHITE,100);
	// perfTest();
}


/*

cd /Users/Apple/Desktop/projects/chessEngv2/Simple-chess-engine/code
c++ main.cpp -o app -std=c++2a -O2

*/