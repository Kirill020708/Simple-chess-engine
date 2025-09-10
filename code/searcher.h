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


#ifndef TRANSPOSTABLE
#define TRANSPOSTABLE

#include "transpositionTable.h"

#endif /* TRANSPOSTABLE */


struct Searcher{
	bool UCIout=1;

	int boardCurrentAge;

	Move bestMove;

	ll nodes=0;

	std::chrono::steady_clock::time_point searchStartTime;

	int quiescentSearch(int color,int alpha,int beta,int depthFromRoot){
		ull currentZobristKey=board.getZobristKey();
		auto [hashTableEvaluation, bestHashMove]=transpositionTableQuiescent.get(currentZobristKey,0,alpha,beta);
		if(hashTableEvaluation!=NO_EVAL){
			alpha=max(alpha,hashTableEvaluation);
			if(alpha>=beta)
				return alpha;
		}
		moveListGenerator.hashMove=bestHashMove;
		nodes++;
		int evaluation=evaluator.evaluatePosition(color);
		Board boardCopy=board;
		moveListGenerator.generateMoves(color,depthFromRoot,DO_SORT,ONLY_CAPTURES);
		int maxEvaluation=evaluation;
		alpha=evaluation;
		if(alpha>=beta){
			transpositionTableQuiescent.write(currentZobristKey,maxEvaluation,0,LOWER_BOUND,boardCurrentAge,bestHashMove);
			return maxEvaluation;
		}
		bool isFirstMove=1;
		char type=UPPER_BOUND;
		for(int currentMove=0;currentMove<moveListGenerator.moveListSize[depthFromRoot];currentMove++){
			Move move=moveListGenerator.moveList[depthFromRoot][currentMove];
			board.makeMove(move);
			int score=-quiescentSearch((color==WHITE)?BLACK:WHITE,-beta,-alpha,depthFromRoot+1);
			isFirstMove=0;
			board=boardCopy;
			if(maxEvaluation<score){
				if(score>alpha)
					type=EXACT;
				maxEvaluation=score;
				bestHashMove=move;
				if(alpha<score)
					alpha=score;
				if(alpha>=beta){
					transpositionTableQuiescent.write(currentZobristKey,maxEvaluation,0,LOWER_BOUND,boardCurrentAge,bestHashMove);
					return maxEvaluation;
				}
			}
		}
		transpositionTableQuiescent.write(currentZobristKey,maxEvaluation,0,type,boardCurrentAge,bestHashMove);
		return maxEvaluation;
	}

	int search(int color,int depth,int isRoot,int alpha,int beta,int depthFromRoot){
		ull currentZobristKey=board.getZobristKey();
		auto [hashTableEvaluation, bestHashMove]=transpositionTable.get(currentZobristKey,depth,alpha,beta);
		if(hashTableEvaluation!=NO_EVAL){
			alpha=max(alpha,hashTableEvaluation);
			if(alpha>=beta)
				return alpha;
		}
		moveListGenerator.hashMove=bestHashMove;
		nodes++;
		if(depth==0)
			return quiescentSearch(color,alpha,beta,depthFromRoot);
		Board boardCopy=board;
		moveListGenerator.generateMoves(color,depthFromRoot,DO_SORT,ALL_MOVES);
		int maxEvaluation=-inf;
		char type=UPPER_BOUND;
		bool isFirstMove=1;
		for(int currentMove=0;currentMove<moveListGenerator.moveListSize[depthFromRoot];currentMove++){
			Move move=moveListGenerator.moveList[depthFromRoot][currentMove];
			board.makeMove(move);
			int score;
			if(!isFirstMove){
				score=-search((color==WHITE)?BLACK:WHITE,depth-1,0,-(alpha+1),-alpha,depthFromRoot+1);
				if(score>alpha&&score<beta)
					score=-search((color==WHITE)?BLACK:WHITE,depth-1,0,-beta,-alpha,depthFromRoot+1);
			}else
				score=-search((color==WHITE)?BLACK:WHITE,depth-1,0,-beta,-alpha,depthFromRoot+1);
			isFirstMove=0;
			board=boardCopy;
			if(maxEvaluation<score){
				if(score>alpha)
					type=EXACT;
				maxEvaluation=score;
				bestHashMove=move;
				if(isRoot)
					bestMove=move;
				if(alpha<score)
					alpha=score;
				if(alpha>=beta){
					transpositionTable.write(currentZobristKey,maxEvaluation,depth,LOWER_BOUND,boardCurrentAge,bestHashMove);
					return maxEvaluation;
				}
			}
		}
		
		transpositionTable.write(currentZobristKey,maxEvaluation,depth,type,boardCurrentAge,bestHashMove);
		return maxEvaluation;
	}

	void iterativeDeepeningSearch(int color,int maxDepth){
		nodes=0;
		boardCurrentAge=board.age;
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