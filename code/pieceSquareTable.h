// evaluation for piece value and position

#pragma once


#ifndef BOARDHELP
#define BOARDHELP

#include "boardHelper.h"

#endif /* BOARDHELP */

struct PieceSquareTable{
	int materialEval[7]={0,100,300,300,500,1000,1000000000};

	int pawns[64]={
			0,   0,   0,   0,   0,   0,   0,   0,
			50,  50,  50,  50,  50,  50,  50,  50,
			10,  10,  20,  30,  30,  20,  10,  10,
			 5,   5,  10,  25,  25,  10,   5,   5,
			 0,   0,   0,  20,  20,   0,   0,   0,
			 5,  -5, -10,   0,   0, -10,  -5,   5,
			 5,  10,  10, -20, -20,  10,  10,   5,
			 0,   0,   0,   0,   0,   0,   0,   0
		 };
	int pawnsEnd[64]={
			0,   0,   0,   0,   0,   0,   0,   0,
			80,  80,  80,  80,  80,  80,  80,  80,
			50,  50,  50,  50,  50,  50,  50,  50,
			30,  30,  30,  30,  30,  30,  30,  30,
			20,  20,  20,  20,  20,  20,  20,  20,
			15,  15,  15,  15,  15,  15,  15,  15,
			10,  10,  10,  10,  10,  10,  10,  10,
			 0,   0,   0,   0,   0,   0,   0,   0
		 };
	int rooks[64]={
			0,  0,  0,  0,  0,  0,  0,  0,
			5, 10, 10, 10, 10, 10, 10,  5,
			-5,  0,  0,  0,  0,  0,  0, -5,
			-5,  0,  0,  0,  0,  0,  0, -5,
			-5,  0,  0,  0,  0,  0,  0, -5,
			-5,  0,  0,  0,  0,  0,  0, -5,
			-5,  0,  0,  0,  0,  0,  0, -5,
			0,  0,  0,  5,  5,  0,  0,  0
		};
	int knights[64] = {
			-50,-40,-30,-30,-30,-30,-40,-50,
			-40,-20,  0,  0,  0,  0,-20,-40,
			-30,  0, 10, 15, 15, 10,  0,-30,
			-30,  5, 15, 20, 20, 15,  5,-30,
			-30,  0, 15, 20, 20, 15,  0,-30,
			-30,  5, 10, 15, 15, 10,  5,-30,
			-40,-20,  0,  5,  5,  0,-20,-40,
			-50,-40,-30,-30,-30,-30,-40,-50,
		};
	int bishops[64] =  {
			-20,-10,-10,-10,-10,-10,-10,-20,
			-10,  0,  0,  0,  0,  0,  0,-10,
			-10,  0,  5, 10, 10,  5,  0,-10,
			-10,  5,  5, 10, 10,  5,  5,-10,
			-10,  0, 10, 10, 10, 10,  0,-10,
			-10, 10, 10, 10, 10, 10, 10,-10,
			-10,  5,  0,  0,  0,  0,  5,-10,
			-20,-10,-10,-10,-10,-10,-10,-20,
		};
	int queens[64] =  {
			-20,-10,-10, -5, -5,-10,-10,-20,
			-10,  0,  0,  0,  0,  0,  0,-10,
			-10,  0,  5,  5,  5,  5,  0,-10,
			-5,   0,  5,  5,  5,  5,  0, -5,
			0,	0,  5,  5,  5,  5,  0, -5,
			-10,  5,  5,  5,  5,  5,  0,-10,
			-10,  0,  5,  0,  0,  0,  0,-10,
			-20,-10,-10, -5, -5,-10,-10,-20
		};
	int kings[64]= {
			-80, -70, -70, -70, -70, -70, -70, -80, 
			-60, -60, -60, -60, -60, -60, -60, -60, 
			-40, -50, -50, -60, -60, -50, -50, -40, 
			-30, -40, -40, -50, -50, -40, -40, -30, 
			-20, -30, -30, -40, -40, -30, -30, -20, 
			-10, -20, -20, -20, -20, -20, -20, -10, 
			20,  20,  -5,  -5,  -5,  -5,  20,  20, 
			20,  30,  10,   0,   0,  10,  30,  20
		};
	int kingsEnd[64] = 
		{
			-20, -10, -10, -10, -10, -10, -10, -20,
			-5,   0,   5,   5,   5,   5,   0,  -5,
			-10, -5,   20,  30,  30,  20,  -5, -10,
			-15, -10,  35,  45,  45,  35, -10, -15,
			-20, -15,  30,  40,  40,  30, -15, -20,
			-25, -20,  20,  25,  25,  20, -20, -25,
			-30, -25,   0,   0,   0,   0, -25, -30,
			-50, -30, -30, -30, -30, -30, -30, -50
		};

	inline int getPieceMaterialEval(int piece){
		return materialEval[piece];
	}

	// pawns and king: total_eval=(eval*(number_of_pieces)+eval_endgame*(32-number_of_pieces))/32

	inline int getPiecePositionEval(int piece,int square,int color,float endgameWeight){
		if(color==BLACK)
			square=((7-(square>>3))<<3)+(square&7); // mirror the square
		if(piece==PAWN)
			return int(pawns[square]*(1-endgameWeight)+pawnsEnd[square]*endgameWeight);
		if(piece==KNIGHT)
			return knights[square];
		if(piece==BISHOP)
			return bishops[square];
		if(piece==ROOK)
			return rooks[square];
		if(piece==QUEEN)
			return queens[square];
		if(piece==KING)
			return int(kings[square]*(1-endgameWeight)+kingsEnd[square]*endgameWeight);
		return 0;
	}

	inline int getPieceEval(int piece,int square,int color,float endgameWeight){
		return (getPieceMaterialEval(piece)+getPiecePositionEval(piece,square,color,endgameWeight))*((color==WHITE)?1:-1);
	}
};

PieceSquareTable pieceSquareTable;