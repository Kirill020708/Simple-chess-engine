// functions for different board data


#pragma once

#ifndef BITBOARD
#define BITBOARD

#include "bitboard.h"

#endif /* BITBOARD */

struct BoardHelper{
	Bitboard kingMoves[64],knightMoves[64];

	Bitboard pawnMoves[2][64],pawnCaptures[2][64],pawnCaptureLeft[2][64],pawnCaptureRight[2][64];

	Bitboard neighborColumns[64],columns[64];

	Bitboard possiblePawnDefendersWhite[64],possiblePawnDefendersBlack[64]; // squares for checking for passed pawn
	Bitboard possibleOutpostDefendersWhite[64],possibleOutpostDefendersBlack[64]; // squares for checking for outpost

	Bitboard updatedSquares[64];

	Bitboard columnUp[64],columnDown[64];

	int distanceToEdge[64];

	inline int getColumnNumber(int square){
		return square&7;
	}

	inline int getRowNumber(int square){
		return square>>3;
	}

	inline Bitboard getRow(int rowNumber){
		return Bitboard(ull(0b11111111)<<((7-rowNumber)<<3));
	}

	inline Bitboard getColumn(int columnNuber){
		return Bitboard(ull(0b0000000100000001000000010000000100000001000000010000000100000001)<<columnNuber);
	}

	inline Bitboard generateMask(int start,int end){//mask with 1-s from start bit to end bit
		return Bitboard(((1ull<<(end-start+1))-1)<<start);
	}

	inline int distanceColumn(Bitboard column,int color){ // distance for the first piece in this column from color's perspective
		if(column==0)
			return 7;
		if(color==WHITE)
			return 7-getRowNumber(column.getLastBitNumber());
		else
			return getRowNumber(column.getFirstBitNumber());
	}

	BoardHelper(){
		for(int i=0;i<8;i++)
			for(int j=0;j<8;j++){
				int s=i*8+j;

				columns[s]=getColumn(j);


				neighborColumns[s]=0;
				if(j)
					neighborColumns[s]|=getColumn(j-1);
				if(j<7)
					neighborColumns[s]|=getColumn(j+1);


				possiblePawnDefendersWhite[s]=0;
				for(ll i1=i-1;i1>=0;i1--){
					int s1=i1*8+j;
					columnUp[s]|=(1ull<<s1);
					possiblePawnDefendersWhite[s]|=(1ull<<s1);
					if(j)
						possiblePawnDefendersWhite[s]|=(1ull<<(s1-1));
					if(j+1<8)
						possiblePawnDefendersWhite[s]|=(1ull<<(s1+1));
				}
				possiblePawnDefendersBlack[s]=0;
				for(ll i1=i+1;i1<8;i1++){
					int s1=i1*8+j;
					columnDown[s]|=(1ull<<s1);
					possiblePawnDefendersBlack[s]|=(1ull<<s1);
					if(j)
						possiblePawnDefendersBlack[s]|=(1ull<<(s1-1));
					if(j+1<8)
						possiblePawnDefendersBlack[s]|=(1ull<<(s1+1));
				}

				possibleOutpostDefendersWhite[s]=possiblePawnDefendersWhite[s]^columnUp[s];
				possibleOutpostDefendersBlack[s]=possiblePawnDefendersBlack[s]^columnDown[s];


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

				if(i>=1){
					pawnMoves[WHITE][s]|=(1ull<<(s-8));
					if(i==6)
						pawnMoves[WHITE][s]|=(1ull<<(s-16));
				}

				if(i<=6){
					pawnMoves[BLACK][s]|=(1ull<<(s+8));
					if(i==1)
						pawnMoves[BLACK][s]|=(1ull<<(s+16));
				}


				if(i>=1){
					if(j<=6){
						pawnCaptures[WHITE][s]|=(1ull<<(s-7));
						pawnCaptureRight[WHITE][s]=(1ull<<(s-7));
					}
					if(j>=1){
						pawnCaptures[WHITE][s]|=(1ull<<(s-9));
						pawnCaptureLeft[WHITE][s]=(1ull<<(s-9));
					}
				}

				if(i<=6){
					if(j>=1){
						pawnCaptures[BLACK][s]|=(1ull<<(s+7));
						pawnCaptureLeft[BLACK][s]=(1ull<<(s+7));
					}
					if(j<=6){
						pawnCaptures[BLACK][s]|=(1ull<<(s+9));
						pawnCaptureRight[BLACK][s]=(1ull<<(s+9));
					}
				}

				updatedSquares[s]=kingMoves[s]|knightMoves[s];
				for(ll i1=0;i1<8;i1++)
					for(ll j1=0;j1<8;j1++){
						ll s1=i1*8+j1;
						if(i1==i||j1==j||(abs(i-i1)==1&&abs(j-j1)==1))
							updatedSquares[s]|=(1ull<<s1);
					}


				distanceToEdge[s]=min(i,7-i)+min(j,7-j);
			}
	}
};

BoardHelper boardHelper;