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

//score: 10 bits for eval, 6 bits for mvv-lva, 

struct MoveListGenerator{
	const int captureShift=10,hashMoveShift=16;

	Move moveList[maxDepth][maxListSize];
	int moveListSize[maxDepth];

	Move hashMove;

	inline void generateMoves(int color,int depth,bool doSort,bool onlyCaptures){
		Board boardCopy=board;
		moveListSize[depth]=0;

		Bitboard friendPieces,opponentPieces;


		if(color==WHITE){
			friendPieces=board.whitePieces;
			opponentPieces=board.blackPieces;
		}else{
			friendPieces=board.blackPieces;
			opponentPieces=board.whitePieces;
		}


		Bitboard pieces=friendPieces;
		int currentEvaluation=board.evaluation;

		while(pieces>0){
			int startSquare=pieces.getFirstBitNumberAndExclude();
			Bitboard moves=moveGenerator.moves(startSquare);
			if(onlyCaptures){
				moves&=opponentPieces;
			}

			while(moves>0){
				int targetSquare=moves.getFirstBitNumberAndExclude();

				int captureCoeff=0;
				if(opponentPieces.getBit(targetSquare)){
					int attackingPiece=board.occupancyPiece(startSquare);
					int capturedPiece=board.occupancyPiece(targetSquare);
					captureCoeff=(capturedPiece-attackingPiece)+10;
				}

				if(board.occupancyPiece(startSquare)==PAWN&&
					((color==WHITE&&targetSquare<8)||(color==BLACK&&targetSquare>=56))){// promotion
					Move promotionMoves[4];
					promotionMoves[0]=Move(startSquare,targetSquare,KNIGHT,pieceSquareTable.materialEval[KNIGHT]<<captureShift);
					promotionMoves[1]=Move(startSquare,targetSquare,BISHOP,pieceSquareTable.materialEval[BISHOP]<<captureShift);
					promotionMoves[2]=Move(startSquare,targetSquare,ROOK,pieceSquareTable.materialEval[ROOK]<<captureShift);
					promotionMoves[3]=Move(startSquare,targetSquare,QUEEN,pieceSquareTable.materialEval[QUEEN]<<captureShift);
					for(int i=0;i<4;i++){
						board.makeMove(promotionMoves[i]);
						if(moveGenerator.isInCheck(color)){
							board=boardCopy;
							continue;
						}
						if(promotionMoves[i]==hashMove)
							promotionMoves[i].score+=(1<<hashMoveShift);
						moveList[depth][moveListSize[depth]++]=promotionMoves[i];
					}
				}else{
					board.makeMove(Move(startSquare,targetSquare,NOPIECE));
					if(moveGenerator.isInCheck(color)){
						board=boardCopy;
						continue;
					}
					int multForColor=1; // if color is black, we must get board.evaluation diff with negating, because board.evaluation is from white perspective
					if(color==BLACK)
						multForColor=-1;
					Move move=Move(startSquare,targetSquare,NOPIECE,
						(captureCoeff<<captureShift)+(board.evaluation)*multForColor);
					if(move==hashMove)
						move.score+=(1<<hashMoveShift);
					moveList[depth][moveListSize[depth]++]=move;
				}
				board=boardCopy;
			}
		}
		if(doSort)
			sort(moveList[depth],moveList[depth]+moveListSize[depth]);
	}


	inline void generateMovesForPerft(int color,int depth){ // optimized gen without scores
		Board boardCopy=board;
		moveListSize[depth]=0;

		Bitboard friendPieces,opponentPieces;


		if(color==WHITE){
			friendPieces=board.whitePieces;
			opponentPieces=board.blackPieces;
		}else{
			friendPieces=board.blackPieces;
			opponentPieces=board.whitePieces;
		}


		Bitboard pieces=friendPieces;

		while(pieces>0){
			int startSquare=pieces.getFirstBitNumberAndExclude();
			Bitboard moves=moveGenerator.moves(startSquare);

			while(moves>0){
				int targetSquare=moves.getFirstBitNumberAndExclude();

				if(board.occupancyPiece(startSquare)==PAWN&&
					((color==WHITE&&targetSquare<8)||(color==BLACK&&targetSquare>=56))){// promotion
					Move promotionMoves[4];
					promotionMoves[0]=Move(startSquare,targetSquare,KNIGHT);
					promotionMoves[1]=Move(startSquare,targetSquare,BISHOP);
					promotionMoves[2]=Move(startSquare,targetSquare,ROOK);
					promotionMoves[3]=Move(startSquare,targetSquare,QUEEN);
					for(int i=0;i<4;i++){
						board.makeMove(promotionMoves[i]);
						if(moveGenerator.isInCheck(color)){
							board=boardCopy;
							continue;
						}
						moveList[depth][moveListSize[depth]++]=promotionMoves[i];
					}
				}else{
					board.makeMove(Move(startSquare,targetSquare,NOPIECE));
					if(moveGenerator.isInCheck(color)){
						board=boardCopy;
						continue;
					}
					moveList[depth][moveListSize[depth]++]=Move(startSquare,targetSquare,NOPIECE);
				}
				board=boardCopy;
			}
		}
	}




	bool isStalled(int color){
		Board boardCopy=board;
		Bitboard friendPieces;


		if(color==WHITE)
			friendPieces=board.whitePieces;
		else
			friendPieces=board.blackPieces;


		Bitboard pieces=friendPieces;

		while(pieces>0){
			int startSquare=pieces.getFirstBitNumberAndExclude();
			Bitboard moves=moveGenerator.moves(startSquare);

			while(moves>0){
				int targetSquare=moves.getFirstBitNumberAndExclude();

				if(board.occupancyPiece(startSquare)==PAWN&&
					((color==WHITE&&targetSquare<8)||(color==BLACK&&targetSquare>=56))){// promotion
					Move promotionMoves[4];
					promotionMoves[0]=Move(startSquare,targetSquare,KNIGHT,pieceSquareTable.materialEval[KNIGHT]<<captureShift);
					promotionMoves[1]=Move(startSquare,targetSquare,BISHOP,pieceSquareTable.materialEval[BISHOP]<<captureShift);
					promotionMoves[2]=Move(startSquare,targetSquare,ROOK,pieceSquareTable.materialEval[ROOK]<<captureShift);
					promotionMoves[3]=Move(startSquare,targetSquare,QUEEN,pieceSquareTable.materialEval[QUEEN]<<captureShift);
					for(int i=0;i<4;i++){
						board.makeMove(promotionMoves[i]);
						if(moveGenerator.isInCheck(color)){
							board=boardCopy;
							continue;
						}
						board=boardCopy;
						return false;
					}
				}else{
					board.makeMove(Move(startSquare,targetSquare,NOPIECE));
					if(moveGenerator.isInCheck(color)){
						board=boardCopy;
						continue;
					}
					board=boardCopy;
					return false;
				}
			}
		}
		return true;
	}
};

MoveListGenerator moveListGenerator;