// history heruistics helper

#pragma once

#ifndef DECLARS
#define DECLARS

#include "declars.h"

#endif /* DECLARS */

#ifndef MOVE
#define MOVE

#include "move.h"

#endif /* MOVE */

struct HistoryHelper {
    int historyScore[2][64][64][2][2];
    int pieceSquareHistory[2][8][64][2][2];
    int counterHistory[2][8][64][8][64];
    
    const int pwnSz = (1 << 10) - 1;
    int pawnHistory[1 << 10][2][8][64];

    int captHistoryScore[2][8][64][8];
    int maxHistoryScore = 511;

    Bitboard whiteAttacks, blackAttacks;

    void clear() {
    	memset(historyScore, 0, sizeof(historyScore));
    }

    inline void update(Board &board, int color, Move move, int score) {
        if (score < -maxHistoryScore)
            score = -maxHistoryScore;
        if (score > maxHistoryScore)
            score = maxHistoryScore; // clamp

	    int st = move.getStartSquare();
	    int tr = move.getTargetSquare();
	    int movedPiece = board.occupancyPiece(st);

        if (board.occupancy(tr) == EMPTY) {

	        int stTh, trTh;
	        if (color == WHITE) {
	        	stTh = (blackAttacks & (1ull << st)) > 0;
	        	trTh = (blackAttacks & (1ull << tr)) > 0;
	        } else {
	        	stTh = (whiteAttacks & (1ull << st)) > 0;
	        	trTh = (whiteAttacks & (1ull << tr)) > 0;
	        }

	        historyScore[color][st][tr][stTh][trTh] +=
	            score - historyScore[color][st][tr][stTh][trTh] * abs(score) / maxHistoryScore;

	        pieceSquareHistory[color][movedPiece][tr][stTh][trTh] +=
	            score - pieceSquareHistory[color][movedPiece][tr][stTh][trTh] * abs(score) / maxHistoryScore;

	        counterHistory[color][board.lastPs][board.lastSq][movedPiece][tr] +=
	            score - counterHistory[color][board.lastPs][board.lastSq][movedPiece][tr] * abs(score) / maxHistoryScore;

	        int index = board.zobristKeyPawn & pwnSz;
	        pawnHistory[index][color][movedPiece][tr] +=
	            score - pawnHistory[index][color][movedPiece][tr] * abs(score) / maxHistoryScore;

	    } else {
	    	int capturedPiece = board.occupancyPiece(tr);

	        captHistoryScore[color][movedPiece][tr][capturedPiece] +=
	            score - captHistoryScore[color][movedPiece][tr][capturedPiece] * abs(score) / maxHistoryScore;
	    }
    }

    inline int getScore(Board &board, int color, Move move) {

        int st = move.getStartSquare();
        int tr = move.getTargetSquare();
	    int movedPiece = board.occupancyPiece(st);

        if (board.occupancy(tr) == EMPTY) {

	        int stTh, trTh;
	        if (color == WHITE) {
	        	stTh = (blackAttacks & (1ull << st)) > 0;
	        	trTh = (blackAttacks & (1ull << tr)) > 0;
	        } else {
	        	stTh = (whiteAttacks & (1ull << st)) > 0;
	        	trTh = (whiteAttacks & (1ull << tr)) > 0;
	        }

	        int history = 0;
	        
	        history += (historyScore[color][st][tr][stTh][trTh]);
	        history += (pieceSquareHistory[color][movedPiece][tr][stTh][trTh]);

	        history += (counterHistory[color][board.lastPs][board.lastSq][movedPiece][tr]);

	        int index = board.zobristKeyPawn & pwnSz;
	        history += pawnHistory[index][color][movedPiece][tr];

	        history /= 4;
	        return history + maxHistoryScore; // to prevent negative values

	    } else {
	    	int capturedPiece = board.occupancyPiece(tr);

	        return (captHistoryScore[color][movedPiece][tr][capturedPiece]) +
	               maxHistoryScore;
	    }
    }
};

struct CorrHistoryHelper {
	const int sznd = (1 << 14) - 1;
	int corrHistTablePawn[2][1 << 14];
	int corrHistTableMinor[2][1 << 14];

	int corrHistTableWhite[2][1 << 14];
	int corrHistTableBlack[2][1 << 14];

	int corrHistLastmove[2][8][64];

	const int maxCorrHistValue = 300;

    void clear() {
        for (int i = 0; i < 2; i++)
            for (int j = 0; j <= sznd; j++){
            	corrHistTablePawn[i][j] = 0;
            	corrHistTableMinor[i][j] = 0;
            	corrHistTableWhite[i][j] = 0;
            	corrHistTableBlack[i][j] = 0;
            }
        memset(corrHistLastmove, 0, sizeof(corrHistLastmove));
    }

	inline void update(int color, Board &board, int score) {
		score = clamp(score, -maxCorrHistValue, maxCorrHistValue);
		int index;
		
		index = board.zobristKeyPawn & sznd;
		corrHistTablePawn[color][index] +=
			score - corrHistTablePawn[color][index] * abs(score) / maxCorrHistValue;
		
		index = board.zobristKeyMinor & sznd;
		corrHistTableMinor[color][index] +=
			score - corrHistTableMinor[color][index] * abs(score) / maxCorrHistValue;
		
		index = board.zobristKeyWhite & sznd;
		corrHistTableWhite[color][index] +=
			score - corrHistTableWhite[color][index] * abs(score) / maxCorrHistValue;
		
		index = board.zobristKeyBlack & sznd;
		corrHistTableBlack[color][index] +=
			score - corrHistTableBlack[color][index] * abs(score) / maxCorrHistValue;
		
		corrHistLastmove[color][board.lastPs][board.lastSq] +=
			score - corrHistLastmove[color][board.lastPs][board.lastSq] * abs(score) / maxCorrHistValue;
	}

    inline int getScore(int color, Board &board) {
    	int index, corrScore = 0;

		index = board.zobristKeyPawn & sznd;
		corrScore += (50 * corrHistTablePawn[color][index]) / 300;

		index = board.zobristKeyMinor & sznd;
		corrScore += (50 * corrHistTableMinor[color][index]) / 300;

		index = board.zobristKeyWhite & sznd;
		corrScore += (50 * corrHistTableWhite[color][index]) / 300;

		index = board.zobristKeyBlack & sznd;
		corrScore += (50 * corrHistTableBlack[color][index]) / 300;

		corrScore += (50 * corrHistLastmove[color][board.lastPs][board.lastSq]) / 300;

		return corrScore;
    }
};