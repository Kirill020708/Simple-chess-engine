// evaluation of the current position on board


#ifndef BOARD
#define BOARD

#include "board.h"

#endif /* BOARD */

#ifndef BOARDHELP
#define BOARDHELP

#include "boardHelper.h"

#endif /* BOARDHELP */

#ifndef PIECESQTABLE
#define PIECESQTABLE

#include "pieceSquareTable.h"

#endif /* PIECESQTABLE */

const int MATE_SCORE=inf/10,DRAW_SCORE=0;
const int KING_DIST_EDGE_SCORE=10; // score for evaluation EDGE_EVAL

struct Evaluator{

	int evaluatePosition(){ // board evaluation with white's perspective
		int evaluation=board.evaluation;
/*
		if(abs(board.evaluation)>=300){ // EDGE_EVAL one side is better -> push opp king to side and keep friend king close
			int whiteKingPos=(board.kings&board.whitePieces).getFirstBitNumber();
			int blackKingPos=(board.kings&board.blackPieces).getFirstBitNumber();

			int whiteRow=boardHelper.getRowNumber(whiteKingPos),whiteColumn=boardHelper.getColumnNumber(whiteKingPos);
			int blackRow=boardHelper.getRowNumber(blackKingPos),blackColumn=boardHelper.getColumnNumber(blackKingPos);

			int kingsDist=abs(whiteRow-blackRow)+abs(whiteColumn-blackColumn); // distance between kings

			if(board.evaluation>=300){ // white is better
				evaluation+=(-kingsDist-boardHelper.distanceToEdge[blackKingPos])*KING_DIST_EDGE_SCORE;
			}else{
				evaluation-=(-kingsDist-boardHelper.distanceToEdge[whiteKingPos])*KING_DIST_EDGE_SCORE;
			}
		}
*/

		return evaluation;
	}

	int evaluatePosition(int color){ // board evaluation witb (color)'s perspective
		return evaluatePosition()*((color==WHITE)?1:-1);
	}

	int evaluateStalledPosition(int color,int depthFromRoot){
		if(moveGenerator.isInCheck(color))
			return -(MATE_SCORE-depthFromRoot);
		return DRAW_SCORE;
	}
};

Evaluator evaluator;