#ifndef DECLARS
#define DECLARS

#include "declars.h"

#endif /* DECLARS */

const int KNIGHT_PROM=1,BISHOP_PROM=2,ROOK_PROM=3,QUEEN_PROM=4;

struct Move{
	int move;//0-5b: start, 6-11: end; 11-...:flag for promotion

	Move(){}

	Move(int move_){
		move=move_;
	}

	Move(int startSq,int targetSq,int flag){
		move=startSq+(targetSq<<6)+(flag<<12);
	}

	int getStartSquare(){
		return move&63;
	}

	int getTargetSquare(){
		return (move>>6)&63;
	}

	int getPromotionFlag(){
		return (move>>12);
	}
};