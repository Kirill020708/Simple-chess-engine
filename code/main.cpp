#ifndef BBB
#define BBB

#include "moveGeneration.h"

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
	board.initFromFEN("r3kbnr/pp2pppp/2np4/q1p2b2/2B1P3/N1P2N2/PP1P1PPP/R1BQK2R w KQkq - 3 6");
	ll smw=0,smb=0;
	for(ll i=0;i<64;i++){
		ll x=__builtin_popcountll(moveGenerator.moves(i));
		if(board.whitePieces.getBit(i))
			smw+=x;
		else
			smb+=x;
		if(x){
			cout<<squareToString(i)<<' '<<x<<'\n';
			// cout<<moveGenerator.moves(i)<<'\n';
			// printDesk01(moveGenerator.moves(i));
			// cout<<'\n';
		}

	}
	cout<<smw<<' '<<smb<<'\n';
}


/*

cd /Users/Apple/Desktop/projects/chessEngv2/Simple-chess-engine/code
c++ main.cpp -o app -std=c++2a -O2

*/