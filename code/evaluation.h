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

struct Evaluator{
	const int mateScore=inf/10;

	int evaluatePosition(){
		return board.evaluation;
	}

	int evaluatePosition(int color){
		return board.evaluation*((color==WHITE)?1:-1);
	}

	int evaluateStalledPosition(int color,int depthFromRoot){
		if(moveGenerator.isInCheck(color))
			return -(mateScore-depthFromRoot);
		return 0;
	}
};

Evaluator evaluator;