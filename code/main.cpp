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
	ll sm=0;
	for(ll i=30;i<64;i++){
		ll x=__builtin_popcountll(moveGenerator.moves(i));
		sm+=x;
		if(x){
			cout<<squareToString(i)<<' '<<x<<'\n';
			// cout<<moveGenerator.moves(i)<<'\n';
			// printDesk01(moveGenerator.moves(i));
			// cout<<'\n';
		}

	}
	cout<<sm;
}


/*

cd /Users/Apple/Desktop/projects/chessEngv2/Simple-chess-engine/code
c++ main.cpp -o app -std=c++2a -O2

*/