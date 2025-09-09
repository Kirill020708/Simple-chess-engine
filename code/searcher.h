// searches the best move

#ifndef MOVEGENLIST
#define MOVEGENLIST

#include "moveListGeneration.h"

#endif /* MOVEGENLIST */


#ifndef BOARD
#define BOARD

#include "board.h"

#endif /* BOARD */


#ifndef EVALUATOR
#define EVALUATOR

#include "evaluation.h"

#endif /* EVALUATOR */


struct Searcher{
	bool UCIout=1;

	Move bestMove;

	ll nodes=0;

	std::chrono::steady_clock::time_point searchStartTime;

	int quiescentSearch(int color,int alpha,int beta,int depthFromRoot){
		nodes++;
		int evaluation=evaluator.evaluatePosition(color);
		Board boardCopy=board;
		moveListGenerator.generateMoves(color,depthFromRoot,DO_SORT,ONLY_CAPTURES);
		int maxEvaluation=evaluation;
		alpha=evaluation;
		if(alpha>=beta)
			return maxEvaluation;
		for(int currentMove=0;currentMove<moveListGenerator.moveListSize[depthFromRoot];currentMove++){
			Move move=moveListGenerator.moveList[depthFromRoot][currentMove];
			board.makeMove(move);
			int score=-quiescentSearch((color==WHITE)?BLACK:WHITE,-beta,-alpha,depthFromRoot+1);
			board=boardCopy;
			if(maxEvaluation<score){
				maxEvaluation=score;
				if(alpha<score)
					alpha=score;
				if(alpha>=beta)
					return maxEvaluation;
			}
		}
		return maxEvaluation;
	}

	int search(int color,int depth,int isRoot,int alpha,int beta,int depthFromRoot){
		nodes++;
		if(depth==0)
			return quiescentSearch(color,alpha,beta,depthFromRoot);
		Board boardCopy=board;
		moveListGenerator.generateMoves(color,depthFromRoot,DO_SORT,ALL_MOVES);
		int maxEvaluation=-inf;
		for(int currentMove=0;currentMove<moveListGenerator.moveListSize[depthFromRoot];currentMove++){
			Move move=moveListGenerator.moveList[depthFromRoot][currentMove];
			board.makeMove(move);
			int score=-search((color==WHITE)?BLACK:WHITE,depth-1,0,-beta,-alpha,depthFromRoot+1);
			board=boardCopy;
			if(maxEvaluation<score){
				maxEvaluation=score;
				if(isRoot)
					bestMove=move;
				if(alpha<score)
					alpha=score;
				if(alpha>=beta)
					return maxEvaluation;
			}
		}
		return maxEvaluation;
	}

	void iterativeDeepeningSearch(int color,int maxDepth){
		nodes=0;
        searchStartTime = std::chrono::steady_clock::now();
		for(int depth=1;depth<=maxDepth;depth++){
			int score=search(color,depth,1,-inf*2,inf*2,0);
	        std::chrono::steady_clock::time_point timeNow = std::chrono::steady_clock::now();
	        ll timeThinked=std::chrono::duration_cast<std::chrono::milliseconds> (timeNow - searchStartTime).count();
			if(UCIout){
				cout<<"info depth "<<depth<<
				" score cp "<<score<<
				" nodes "<<nodes<<
				" nps "<<(nodes*1000)/(timeThinked+1)<<
				" time ms "<<timeThinked<<
				" pv "<<bestMove.convertToUCI()<<'\n';
			}
		}
	}
};


Searcher searcher;