// evaluation of the current position on board

#pragma once


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

const int NONE_SCORE=inf-10; // score which never occures
const int MATE_SCORE=inf/10,DRAW_SCORE=0;
const int KING_DIST_EDGE_SCORE=10; // score for evaluation EDGE_EVAL

// Transposition table

#pragma once


#ifndef DECLARS
#define DECLARS

#include "declars.h"

#endif /* DECLARS */

struct EvalTableEntry{
	ull key;
	int evaluation=NO_EVAL;

	EvalTableEntry(){}

	EvalTableEntry(ull key_,int evaluation_){
		key=key_;
		evaluation=evaluation_;
	}
};


struct EvaluationTranspositionTable{
	const ll tableSize=ll(memoryUsageMB)*1024*1024/sizeof(EvalTableEntry);
	EvalTableEntry table[ll(memoryUsageMB)*1024*1024/sizeof(EvalTableEntry)];

	inline void write(ull key,int evaluation){
		int index=key%tableSize;
		table[index]={key,evaluation};
	}

	inline int get(ull key){
		int index=key%tableSize;
		if(table[index].key!=key)
			return NO_EVAL;
		return table[index].evaluation;
	}
};

EvaluationTranspositionTable evaluationTranspositionTable;



struct Evaluator{

	int mobilityScoreMg[7] = {0, 0, 1, 2, 1, 0, -1};
	int mobilityScoreEg[7] = {0, 0, 2, 3, 2, 1, 1};  // v2
	inline int evaluateMobility(){
		int evaluation=0;

		float endgameWeight=board.endgameWeight();

		Bitboard pieces=board.whitePieces&(~board.pawns);
		while(pieces>0){
			int square=pieces.getFirstBitNumberAndExclude();
			int pieceType=board.occupancyPiece(square);
			int mobility=moveGenerator.moves(square).popcnt();
			evaluation+=mobility*(mobilityScoreMg[pieceType]*(1-endgameWeight)+mobilityScoreEg[pieceType]*endgameWeight);
		}

		pieces=board.blackPieces&(~board.pawns);
		while(pieces>0){
			int square=pieces.getFirstBitNumberAndExclude();
			int pieceType=board.occupancyPiece(square);
			int mobility=moveGenerator.moves(square).popcnt();
			evaluation-=mobility*(mobilityScoreMg[pieceType]*(1-endgameWeight)+mobilityScoreEg[pieceType]*endgameWeight);
		}

		return evaluation;
	}

	inline int evaluatePawnIslands(){
		const int pawnIslandPenalty=15;
		int prevW=0,prevB=0;
		int evaluation=0;
		for(int col=0;col<8;col++){
			Bitboard column=boardHelper.getColumn(col);
			int colW=(column&board.pawns&board.whitePieces)>0;
			int colB=(column&board.pawns&board.blackPieces)>0;
			if(!colW&&prevW)
				evaluation-=pawnIslandPenalty;
			prevW=colW;

			if(!colB&&prevB)
				evaluation+=pawnIslandPenalty;
			prevB=colB;
		}
		if(prevW)
			evaluation-=pawnIslandPenalty;

		if(prevB)
			evaluation+=pawnIslandPenalty;

		return evaluation;
	}

	inline int evaluateDoubledPawns(){
		const int doubledPawnPenaltyMg=20,doubledPawnPenaltyEg=7;

		float endgameWeight=board.endgameWeight();

		int numberOfDoubled=0;

		for(int col=0;col<8;col++){
			Bitboard column=boardHelper.getColumn(col);
			int colW=(column&board.pawns&board.whitePieces).popcnt();
			int colB=(column&board.pawns&board.blackPieces).popcnt();

			if(colW>1)
				numberOfDoubled+=colW-1;
			if(colB>1)
				numberOfDoubled-=(colB-1);
		}
		return -(numberOfDoubled*(doubledPawnPenaltyMg*(1-endgameWeight)+doubledPawnPenaltyEg*endgameWeight));
	}

	const int passedPawnScore[8] = {0,5,10,25,45,80,140,0};
	inline int evaluatePassedPawns(){
		// const int isolatedPawnScore=-10;
		// const int blockedPawnScore=0;
		// const int defendedPawnScore=0;

		float endgameWeight=board.endgameWeight();

		int evaluation=0;

		int passedEvaluation=0;

		Bitboard pawns=board.pawns&board.whitePieces;
		Bitboard friendPawns=board.pawns&board.whitePieces;
		Bitboard opponentPawns=board.pawns&board.blackPieces;

		while(pawns){
			int square=pawns.getFirstBitNumberAndExclude();
			int col=boardHelper.getColumnNumber(square),row=7-boardHelper.getRowNumber(square);
			Bitboard column=boardHelper.columns[square],nbColumns=boardHelper.neighborColumns[square];

			if((boardHelper.possiblePawnDefendersWhite[square]&opponentPawns)==0)
				passedEvaluation+=passedPawnScore[row];

		}


		pawns=board.pawns&board.blackPieces;
		friendPawns=board.pawns&board.blackPieces;
		opponentPawns=board.pawns&board.whitePieces;

		while(pawns){
			int square=pawns.getFirstBitNumberAndExclude();
			int col=boardHelper.getColumnNumber(square),row=boardHelper.getRowNumber(square);
			Bitboard column=boardHelper.columns[square],nbColumns=boardHelper.neighborColumns[square];

			if((boardHelper.possiblePawnDefendersBlack[square]&opponentPawns)==0)
				passedEvaluation-=passedPawnScore[row];

		}

		passedEvaluation=passedEvaluation*(0.7*(1-endgameWeight)+1.3*endgameWeight);
		evaluation+=passedEvaluation;

		return evaluation;


		// while(pawns){
		// 	int square=pawns.getFirstBitNumberAndExclude();
		// 	int color=board.occupancy(square);
		// 	int oppositeColor=(color==WHITE)?BLACK:WHITE;

		// 	Bitboard friendPawns=board.pawns&((color==WHITE)?board.whitePieces:board.blackPieces);

		// 	int pawnScore=0;

		// 	if((boardHelper.neighborColumns[square]&friendPawns)==0)
		// 		pawnScore+=isolatedPawnScore;

		// 	if((boardHelper.getColumn(square&7)&friendPawns)!=(1ull<<square))
		// 		pawnScore+=doubledPawnScore;

		// 	int nextSquare=(color==WHITE)?square-8:square+8; // square, where pawn is moving

		// 	if(board.pawns.getBit(nextSquare))
		// 		pawnScore+=blockedPawnScore;

		// 	if((boardHelper.pawnCaptures[oppositeColor][square]&friendPawns)>0)
		// 		pawnScore+=defendedPawnScore;

		// 	pawnsEval+=(color==WHITE)?pawnScore:-pawnScore;
		// }
	}

	int pawnDistancePenalty[8]={0,0,20,50,80,110,120,150};
	inline int evaluateKingShield(){
		int mainColumnEvaluationW=0,nearColumnEvaluationW=0;
		int mainColumnEvaluationB=0,nearColumnEvaluationB=0;

		float endgameWeight=board.endgameWeight();

		int whiteKingPos=(board.kings&board.whitePieces).getFirstBitNumber();
		int col=boardHelper.getColumnNumber(whiteKingPos);
		Bitboard mainColumn=boardHelper.getColumn(col);

		int dist=boardHelper.distanceColumn(mainColumn&board.whitePieces&board.pawns,WHITE);
		mainColumnEvaluationW-=pawnDistancePenalty[dist];

		if(col>0){
			dist=boardHelper.distanceColumn(boardHelper.getColumn(col-1)&board.whitePieces&board.pawns,WHITE);
			nearColumnEvaluationW-=pawnDistancePenalty[dist];
		}
		if(col<7){
			dist=boardHelper.distanceColumn(boardHelper.getColumn(col+1)&board.whitePieces&board.pawns,WHITE);
			nearColumnEvaluationW-=pawnDistancePenalty[dist];
		}
		if(whiteKingPos==58 || whiteKingPos==50){ // c1, c2
			dist=boardHelper.distanceColumn(boardHelper.getColumn(col-2)&board.whitePieces&board.pawns,WHITE);
			nearColumnEvaluationW-=pawnDistancePenalty[dist];
		}

		if(col==3||col==4){ // d or e column
			mainColumnEvaluationW=0;
			nearColumnEvaluationW=0;
		}



		int blackKingPos=(board.kings&board.blackPieces).getFirstBitNumber();
		col=boardHelper.getColumnNumber(blackKingPos);
		mainColumn=boardHelper.getColumn(col);

		dist=boardHelper.distanceColumn(mainColumn&board.blackPieces&board.pawns,BLACK);
		mainColumnEvaluationB+=pawnDistancePenalty[dist];

		if(col>0){
			dist=boardHelper.distanceColumn(boardHelper.getColumn(col-1)&board.blackPieces&board.pawns,BLACK);
			nearColumnEvaluationB+=pawnDistancePenalty[dist];
		}
		if(col<7){
			dist=boardHelper.distanceColumn(boardHelper.getColumn(col+1)&board.blackPieces&board.pawns,BLACK);
			nearColumnEvaluationB+=pawnDistancePenalty[dist];
		}
		if(blackKingPos==2 || blackKingPos==10){ // c8, c7
			dist=boardHelper.distanceColumn(boardHelper.getColumn(col-2)&board.blackPieces&board.pawns,BLACK);
			nearColumnEvaluationB+=pawnDistancePenalty[dist];
		}

		if(col==3||col==4){ // d or e column
			mainColumnEvaluationB=0;
			nearColumnEvaluationB=0;
		}

		int evaluation=mainColumnEvaluationW+mainColumnEvaluationB;

		evaluation+=(nearColumnEvaluationW+nearColumnEvaluationB)*0.6;

		evaluation*=(1-endgameWeight);

		return evaluation;
	}

	inline int evaluatePosition(){ // board evaluation with white's perspective
		ull key=board.getZobristKey();
		int evaluation=evaluationTranspositionTable.get(key);
		if(evaluation!=NO_EVAL)
			return evaluation;

		evaluation=board.evaluation;

		// int numberOfPieces=board.numberOfPieces();
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
		evaluation+=evaluateMobility();

		evaluation+=evaluatePassedPawns();
		evaluation+=evaluatePawnIslands();
		// evaluation+=evaluateDoubledPawns();


		// evaluation+=evaluateKingShield();

		// const int attackSquareScore=1;
		// evaluation+=(moveGenerator.numOfSquaresAttackedByWhite()-moveGenerator.numOfSquaresAttackedByWhite())*attackSquareScore;
		evaluationTranspositionTable.write(key,evaluation);
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