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
    int historyScore[2][64][64];
    int maxHistoryScore = 511;

    void clear() {
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 64; j++)
                for (int k = 0; k < 64; k++)
                    historyScore[i][j][k] = 0;
    }

    inline void update(int color, Move move, int score) {
        if (score < -maxHistoryScore)
            score = -maxHistoryScore;
        if (score > maxHistoryScore)
            score = maxHistoryScore; // clamp

        historyScore[color][move.getStartSquare()][move.getTargetSquare()] +=
            score - historyScore[color][move.getStartSquare()][move.getTargetSquare()] * abs(score) / maxHistoryScore;
    }

    inline int getScore(int color, Move move) {
        return (historyScore[color][move.getStartSquare()][move.getTargetSquare()]) +
               maxHistoryScore; // to prevent negative values
    }
};

struct CorrHistoryHelper {
	const int sznd = (1 << 14) - 1;
	int corrHistTablePawn[2][1 << 14];
	int corrHistTableMinor[2][1 << 14];

	int corrHistTableWhite[2][1 << 14];
	int corrHistTableBlack[2][1 << 14];

	int corrHistTableLastMove[2][64][64];

	Move lastMove;

	const int maxCorrHistValue = 300;

    void clear() {
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j <= sznd; j++){
            	corrHistTablePawn[i][j] = 0;
            	corrHistTableMinor[i][j] = 0;
            	corrHistTableWhite[i][j] = 0;
            	corrHistTableBlack[i][j] = 0;
            }
            for (int j = 0; j < 64; j++)
            	for (int k = 0; k < 64; k++)
            		corrHistTableLastMove[i][i][j] = 0;
        }

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

		if (lastMove != Move()) {
			int st = lastMove.getStartSquare();
			int tr = lastMove.getTargetSquare();
			corrHistTableLastMove[color][st][tr] +=
				score - corrHistTableLastMove[color][st][tr] * abs(score) / maxCorrHistValue;
		}
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

		if (lastMove != Move()) {
			int st = lastMove.getStartSquare();
			int tr = lastMove.getTargetSquare();
			corrScore += (50 * corrHistTableLastMove[color][st][tr]) / 300;
		}

		return corrScore;
    }
};