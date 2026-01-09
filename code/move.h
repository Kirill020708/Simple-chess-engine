// struct for move

#pragma once

#ifndef DECLARS
#define DECLARS

#include "declars.h"

#endif /* DECLARS */

struct Move {
    int16_t move = 0; // 0-5b: start, 6-11: end; 11-...:flag for promotion
    ll score;

    Move() {}

    Move(int move_) {
        move = move_;
        score = 0;
    }

    Move(int startSq, int targetSq, int flag) {
        move = startSq + (targetSq << 6) + (flag << 12);
        score = 0;
    }

    Move(int startSq, int targetSq, int flag, int score_) {
        move = startSq + (targetSq << 6) + (flag << 12);
        score = score_;
    }

    Move(string uciMove) {
        int startColumn = uciMove[0] - 'a', startRow = uciMove[1] - '1', targetColumn = uciMove[2] - 'a',
            targetRow = uciMove[3] - '1';
        startRow = 7 - startRow;
        targetRow = 7 - targetRow;
        int promotion = 0;
        if (uciMove.size() == 5) {
            if (uciMove[4] == 'n')
                promotion = KNIGHT;
            if (uciMove[4] == 'b')
                promotion = BISHOP;
            if (uciMove[4] == 'r')
                promotion = ROOK;
            if (uciMove[4] == 'q')
                promotion = QUEEN;
        }
        move = (startColumn + startRow * 8) + ((targetColumn + targetRow * 8) << 6) + (promotion << 12);
    }

    inline int getStartSquare() {
        return move & 63;
    }

    inline int getTargetSquare() {
        return (move >> 6) & 63;
    }

    inline int getPromotionFlag() {
        return (move >> 12);
    }

    string convertToUCI() {
        string ans = squareNumberToUCI(getStartSquare()) + squareNumberToUCI(getTargetSquare());
        char proms[6] = {'0', '0', 'n', 'b', 'r', 'q'};
        if (getPromotionFlag() != 0)
            ans.push_back(proms[getPromotionFlag()]);
        return ans;
    }
};

// reverse because if move1 < move2 then move2 is better

bool operator<(Move x, Move y) {
    return (x.score > y.score);
}

bool operator<=(Move x, Move y) {
    return (x.score >= y.score);
}

bool operator>(Move x, Move y) {
    return (x.score < y.score);
}

bool operator>=(Move x, Move y) {
    return (x.score <= y.score);
}

bool operator==(Move x, Move y) {
    return (x.move == y.move);
}

bool operator!=(Move x, Move y) {
    return (x.move != y.move);
}