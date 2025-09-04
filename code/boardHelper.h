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
};