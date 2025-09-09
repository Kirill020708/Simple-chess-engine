// generates list of moves for current position


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

#ifndef PIECESQTABLE
#define PIECESQTABLE

#include "pieceSquareTable.h"

#endif /* PIECESQTABLE */

const int maxDepth=256,maxListSize=256;

//score: 10 bits for eval, 4 bits for mvv-lva, 

struct MoveListGenerator{
	const int captureShift=10;

	Move moveList[maxDepth][maxListSize];
	int moveListSize[maxDepth];

	inline void generateWhiteMoves(int depth,bool doSort,bool onlyCaptures){
		Board boardCopy=board;
		moveListSize[depth]=0;
		Bitboard pieces=board.whitePieces;
		int currentEvaluation=board.evaluation;
		while(pieces>0){
			int startSquare=pieces.getFirstBitNumberAndExclude();
			Bitboard moves=moveGenerator.moves(startSquare);
			if(onlyCaptures)
				moves&=board.blackPieces;

			while(moves>0){
				int targetSquare=moves.getFirstBitNumberAndExclude();

				int captureCoeff=0;
				if(board.blackPieces.getBit(targetSquare)){
					int attackingPiece=board.occupancyPiece(startSquare);
					int capturedPiece=board.occupancyPiece(targetSquare);
					captureCoeff=pieceSquareTable.materialEval[capturedPiece]-pieceSquareTable.materialEval[attackingPiece];
				}

				if(board.occupancyPiece(startSquare)==PAWN&&targetSquare<8){//white promotion
					Move promotionMoves[4];
					promotionMoves[0]=Move(startSquare,targetSquare,KNIGHT,pieceSquareTable.materialEval[KNIGHT]<<captureShift);
					promotionMoves[1]=Move(startSquare,targetSquare,BISHOP,pieceSquareTable.materialEval[BISHOP]<<captureShift);
					promotionMoves[2]=Move(startSquare,targetSquare,ROOK,pieceSquareTable.materialEval[ROOK]<<captureShift);
					promotionMoves[3]=Move(startSquare,targetSquare,QUEEN,pieceSquareTable.materialEval[QUEEN]<<captureShift);
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
					moveList[depth][moveListSize[depth]++]=Move(startSquare,targetSquare,NOPIECE,
						(captureCoeff<<captureShift)+board.evaluation-currentEvaluation);
				}
				board=boardCopy;
			}
		}
		if(doSort)
			sort(moveList[depth],moveList[depth]+moveListSize[depth]);
	}

	inline void generateBlackMoves(int depth,bool doSort,bool onlyCaptures){
		Board boardCopy=board;
		moveListSize[depth]=0;
		Bitboard pieces=board.blackPieces;
		int currentEvaluation=board.evaluation;
		while(pieces>0){
			int startSquare=pieces.getFirstBitNumberAndExclude();
			Bitboard moves=moveGenerator.moves(startSquare);
			if(onlyCaptures)
				moves&=board.whitePieces;
			
			while(moves>0){
				int targetSquare=moves.getFirstBitNumberAndExclude();

				int captureCoeff=0;
				if(board.blackPieces.getBit(targetSquare)){
					int attackingPiece=board.occupancyPiece(startSquare);
					int capturedPiece=board.occupancyPiece(targetSquare);
					captureCoeff=pieceSquareTable.materialEval[capturedPiece]-pieceSquareTable.materialEval[attackingPiece];
				}

				if(board.occupancyPiece(startSquare)==PAWN&&targetSquare>=56){//black promotion
					Move promotionMoves[4];
					promotionMoves[0]=Move(startSquare,targetSquare,KNIGHT,pieceSquareTable.materialEval[KNIGHT]<<captureShift);
					promotionMoves[1]=Move(startSquare,targetSquare,BISHOP,pieceSquareTable.materialEval[BISHOP]<<captureShift);
					promotionMoves[2]=Move(startSquare,targetSquare,ROOK,pieceSquareTable.materialEval[ROOK]<<captureShift);
					promotionMoves[3]=Move(startSquare,targetSquare,QUEEN,pieceSquareTable.materialEval[QUEEN]<<captureShift);
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
					moveList[depth][moveListSize[depth]++]=Move(startSquare,targetSquare,NOPIECE,
						(captureCoeff<<captureShift)+(-board.evaluation+currentEvaluation));
				}
				board=boardCopy;
			}
		}
		if(doSort)
			sort(moveList[depth],moveList[depth]+moveListSize[depth]);
	}

	inline void generateMoves(int color,int depth,bool doSort,bool onlyCaptures){
		if(color==WHITE)
			generateWhiteMoves(depth,doSort,onlyCaptures);
		if(color==BLACK)
			generateBlackMoves(depth,doSort,onlyCaptures);
	}
};

MoveListGenerator moveListGenerator;