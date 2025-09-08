#ifndef MOVE
#define MOVE

#include "move.h"

#endif /* MOVE */

#ifndef MAGIC
#define MAGIC

#include "magicNumbers.h"

#endif /* MAGIC */

#ifndef MOVEGEN
#define MOVEGEN

#include "moveGeneration.h"

#endif /* MOVEGEN */

#ifndef BOARD
#define BOARD

#include "board.h"

#endif /* BOARD */

const int maxDepth=256,maxListSize=256;
struct MoveListGenerator{
	Move moveList[maxDepth][maxListSize];
	int moveListSize[maxDepth];

	inline void generateWhiteMoves(int depth){
		Board boardCopy=board;
		moveListSize[depth]=0;
		Bitboard pieces=board.whitePieces;
		while(pieces>0){
			int startSquare=pieces.getFirstBitNumberAndExclude();
			Bitboard moves=moveGenerator.moves(startSquare);
			while(moves>0){
				int targetSquare=moves.getFirstBitNumberAndExclude();
				if(board.occupancyPiece(startSquare)==PAWN&&targetSquare<8){//white promotion
					Move promotionMoves[4];
					promotionMoves[0]=Move(startSquare,targetSquare,KNIGHT);
					promotionMoves[1]=Move(startSquare,targetSquare,BISHOP);
					promotionMoves[2]=Move(startSquare,targetSquare,ROOK);
					promotionMoves[3]=Move(startSquare,targetSquare,QUEEN);
					for(int i=0;i<4;i++){
						board.makeMove(promotionMoves[i]);
						if(moveGenerator.isWhiteInCheck()){
							board=boardCopy;
							continue;
						}
						moveList[depth][moveListSize[depth]++]=promotionMoves[i];
					}
				}else{
					board.makeMove(Move(startSquare,targetSquare,NOPIECE));
					if(moveGenerator.isWhiteInCheck()){
						board=boardCopy;
						continue;
					}
					moveList[depth][moveListSize[depth]++]=Move(startSquare,targetSquare,NOPIECE);
				}
				board=boardCopy;
			}
		}
	}

	inline void generateBlackMoves(int depth){
		Board boardCopy=board;
		moveListSize[depth]=0;
		Bitboard pieces=board.blackPieces;
		while(pieces>0){
			int startSquare=pieces.getFirstBitNumberAndExclude();
			Bitboard moves=moveGenerator.moves(startSquare);
			while(moves>0){
				int targetSquare=moves.getFirstBitNumberAndExclude();
				if(board.occupancyPiece(startSquare)==PAWN&&targetSquare>=56){//black promotion
					Move promotionMoves[4];
					promotionMoves[0]=Move(startSquare,targetSquare,KNIGHT);
					promotionMoves[1]=Move(startSquare,targetSquare,BISHOP);
					promotionMoves[2]=Move(startSquare,targetSquare,ROOK);
					promotionMoves[3]=Move(startSquare,targetSquare,QUEEN);
					for(int i=0;i<4;i++){
						board.makeMove(promotionMoves[i]);
						if(moveGenerator.isBlackInCheck()){
							board=boardCopy;
							continue;
						}
						moveList[depth][moveListSize[depth]++]=promotionMoves[i];
					}
				}else{
					board.makeMove(Move(startSquare,targetSquare,NOPIECE));
					if(moveGenerator.isBlackInCheck()){
						board=boardCopy;
						continue;
					}
					moveList[depth][moveListSize[depth]++]=Move(startSquare,targetSquare,NOPIECE);
				}
				board=boardCopy;
			}
		}
	}

	inline void generateMoves(int color,int depth){
		if(color==WHITE)
			generateWhiteMoves(depth);
		if(color==BLACK)
			generateBlackMoves(depth);
	}
};

MoveListGenerator moveListGenerator;