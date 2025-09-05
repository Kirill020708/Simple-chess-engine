#ifndef MOVE
#define MOVE

#include "move.h"

#endif /* MOVE */

#ifndef BITBOARD
#define BITBOARD

#include "bitboard.h"

#endif /* BITBOARD */

#ifndef BOARDHELP
#define BOARDHELP

#include "boardHelper.h"

#endif /* BOARDHELP */

const int EMPTY=-1,WHITE=0,BLACK=1,ERROR=2;

struct Board{
	Bitboard whitePieces,blackPieces;
	Bitboard pawns,knights,bishops,rooks,queens,kings;

	bool castlingWhiteQueensideBroke,castlingWhiteKingsideBroke,castlingBlackQueensideBroke,castlingBlackKingsideBroke;

	int enPassantColumn;

	Board(){
		whitePieces=boardHelper.generateMask(48,63);
		blackPieces=boardHelper.generateMask(0,15);
		pawns=boardHelper.generateMask(8,15)|boardHelper.generateMask(48,55);
		knights=(1ull<<1)|(1ull<<6)|(1ull<<57)|(1ull<<62);
		bishops=(1ull<<2)|(1ull<<5)|(1ull<<58)|(1ull<<61);
		rooks=(1ull<<0)|(1ull<<7)|(1ull<<56)|(1ull<<63);
		queens=(1ull<<3)|(1ull<<59);
		kings=(1ull<<4)|(1ull<<60);
		castlingWhiteQueensideBroke=castlingWhiteKingsideBroke=castlingBlackQueensideBroke=castlingBlackKingsideBroke=0;
		enPassantColumn=-1;
	}

	int occupancy(int square){
		if(square<0||square>63)
			return ERROR;
		if(whitePieces.getBit(square))
			return WHITE;
		if(blackPieces.getBit(square))
			return BLACK;
		return EMPTY;
	}

	void initFromFEN(string fen){
		vector<string>tokens=splitStr(fen," ");
		string position=tokens[0],currentColor=tokens[1],castlingAvailability=tokens[2],enPassantSquare=tokens[3];
		whitePieces=blackPieces=pawns=knights=bishops=rooks=queens=kings=0;
		castlingWhiteQueensideBroke=castlingWhiteKingsideBroke=castlingBlackQueensideBroke=castlingBlackKingsideBroke=1;
		enPassantColumn=-1;

		int currentSquare=0;
		for(auto piece:position){
			if(piece=='/')
				continue;
			if(piece>='0'&&piece<='9'){
				currentSquare+=piece-'0';
				continue;
			}
			ull currentSquareBit=(1ull<<currentSquare);
			if(piece>='A'&&piece<='Z'){
				whitePieces|=currentSquareBit;
				piece+='a'-'A';
			}else
				blackPieces|=currentSquareBit;
			if(piece=='p')
				pawns|=currentSquareBit;
			if(piece=='n')
				knights|=currentSquareBit;
			if(piece=='b')
				bishops|=currentSquareBit;
			if(piece=='r')
				rooks|=currentSquareBit;
			if(piece=='q')
				queens|=currentSquareBit;
			if(piece=='k')
				kings|=currentSquareBit;
			currentSquare++;
		}

		for(auto cst:castlingAvailability){
			if(cst=='Q')
				castlingWhiteQueensideBroke=0;
			if(cst=='K')
				castlingWhiteKingsideBroke=0;
			if(cst=='q')
				castlingBlackQueensideBroke=0;
			if(cst=='k')
				castlingBlackKingsideBroke=0;
		}

		if(enPassantSquare!="-")
			enPassantColumn=enPassantSquare[0]-'a';
	}
};

Board board;