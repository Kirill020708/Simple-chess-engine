#ifndef BITBOARD
#define BITBOARD

#include "bitboard.h"

#endif /* BITBOARD */

struct BoardHelper{
	Bitboard kingMoves[64],knightMoves[64];

	BoardHelper(){
		for(int i=0;i<8;i++)
			for(int j=0;j<8;j++){
				int s=i*8+j;
				for(int i1=0;i1<8;i1++)
					for(int j1=0;j1<8;j1++){
						int s1=i1*8+j1;
						if(s==s1)
							continue;
						if(abs(i1-i)<=1&&abs(j1-j)<=1)
							kingMoves[s]|=(1ull<<s1);
						if(abs(i1-i)*abs(j1-j)==2)
							knightMoves[s]|=(1ull<<s1);
					}
			}
	}

	int getColumnNumber(int square){
		return square&7;
	}

	int getRowNumber(int square){
		return square>>3;
	}

	Bitboard getColumn(int columnNuber){
		return Bitboard(ull(0b1000000010000000100000001000000010000000100000001000000010000000)<<columnNuber);
	}

	Bitboard generateMask(int start,int end){//mask with 1-s from start bit to end bit
		return Bitboard(((1ull<<(end-start+1))-1)<<start);
	}
};

BoardHelper boardHelper;