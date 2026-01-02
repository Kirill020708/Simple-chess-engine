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


struct HistoryHelper{
	int historyScore[2][64][64];
	int maxHistoryScore=511;

	void clear(){
		for(int i=0;i<2;i++)
			for(int j=0;j<64;j++)
				for(int k=0;k<64;k++)
					historyScore[i][j][k]=0;
	}

	inline void update(int color,Move move,int score){
		if(score<-maxHistoryScore)
			score=-maxHistoryScore;
		if(score>maxHistoryScore)
			score=maxHistoryScore; // clamp

		historyScore[color][move.getStartSquare()][move.getTargetSquare()]+=
			score-historyScore[color][move.getStartSquare()][move.getTargetSquare()]*abs(score)/maxHistoryScore;
	}

	inline int getScore(int color,Move move){
		return (historyScore[color][move.getStartSquare()][move.getTargetSquare()])+maxHistoryScore; // to prevent negative values
	}
};


struct CounterHistoryHelper{
	Move prevMove;
	int historyScore[2][64][64][64][64];
	int maxHistoryScore=511;

	void clear(){
		for(int i=0;i<2;i++)
			for(int j=0;j<64;j++)
				for(int k=0;k<64;k++)
					for(int j1=0;j1<64;j1++)
						for(int k1=0;k1<64;k1++)
							historyScore[i][j][k][j1][k1]=0;
	}

	inline void update(int color,Move move,int score){
		if(score<-maxHistoryScore)
			score=-maxHistoryScore;
		if(score>maxHistoryScore)
			score=maxHistoryScore; // clamp

		int pi=prevMove.getStartSquare(),pj=prevMove.getTargetSquare();
		int i=move.getStartSquare(),j=move.getTargetSquare();

		historyScore[color][pi][pj][i][j]+=
			score-historyScore[color][pi][pj][i][j]*abs(score)/maxHistoryScore;
	}

	inline int getScore(int color,Move move){
		int pi=prevMove.getStartSquare(),pj=prevMove.getTargetSquare();
		int i=move.getStartSquare(),j=move.getTargetSquare();
		
		return (historyScore[color][pi][pj][i][j])+maxHistoryScore; // to prevent negative values
	}
};