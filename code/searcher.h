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


#ifndef HISTORY
#define HISTORY

#include "historyHelper.h"

#endif /* HISTORY */

struct Searcher{
	bool UCIout=1;

	bool stopSearch;

	int boardCurrentAge;

	Move bestMove;

	ll nodes=0;

	std::chrono::steady_clock::time_point searchStartTime;

	#define killMovesNumber 2
	Move killerMovesTable[maxDepth][killMovesNumber];
	int killerMovesCount[maxDepth][killMovesNumber];

	int quiescentSearch(int color,int alpha,int beta,int depthFromRoot){
		if(stopSearch)
			return 0;
		nodes++;
		ull currentZobristKey=board.getZobristKey();
		auto [hashTableEvaluation, bestHashMove]=transpositionTableQuiescent.get(currentZobristKey,0,alpha,beta);
		if(hashTableEvaluation!=NO_EVAL){
			alpha=max(alpha,hashTableEvaluation);
			if(alpha>=beta)
				return alpha;
		}
		moveListGenerator.hashMove=bestHashMove;
		// moveListGenerator.killerMove=moveListGenerator.hashMove;
		Board boardCopy=board;
		moveListGenerator.generateMoves(color,depthFromRoot,DO_SORT,ONLY_CAPTURES);

		int evaluation;
		if(moveListGenerator.isStalled(color))
			evaluation=evaluator.evaluateStalledPosition(color,depthFromRoot);
		else
			evaluation=evaluator.evaluatePosition(color);

		int maxEvaluation=evaluation;
		alpha=max(alpha,evaluation);
		if(alpha>=beta){
			transpositionTableQuiescent.write(currentZobristKey,maxEvaluation,0,LOWER_BOUND,boardCurrentAge,bestHashMove);
			return maxEvaluation;
		}

		bool isFirstMove=1;
		char type=UPPER_BOUND;
		bestHashMove=Move();
		for(int currentMove=0;currentMove<moveListGenerator.moveListSize[depthFromRoot];currentMove++){
			Move move=moveListGenerator.moveList[depthFromRoot][currentMove];
			board.makeMove(move);

			int score=-quiescentSearch((color==WHITE)?BLACK:WHITE,-beta,-alpha,depthFromRoot+1);
			isFirstMove=0;
			board=boardCopy;
			if(stopSearch)
				return 0;
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
		if(stopSearch)
			return 0;
		bool isPvNode=((beta-alpha)>1);
		nodes++;

		ull currentZobristKey=board.getZobristKey();

		if(!isRoot){
			for(int repAge=board.age-4;repAge>board.lastIrreversibleMoveAge;repAge-=1)
				if(currentZobristKey==occuredPositionsHelper.occuredPositions[repAge])
					return DRAW_SCORE;
		}

		if(moveListGenerator.isStalled(color))
			return evaluator.evaluateStalledPosition(color,depthFromRoot);


		auto [hashTableEvaluation, bestHashMove]=transpositionTable.get(currentZobristKey,depth,alpha,beta);
		if(hashTableEvaluation!=NO_EVAL){
			alpha=max(alpha,hashTableEvaluation);
			if(alpha>=beta)
				return alpha;
		}

		// occuredPositions[board.age]=currentZobristKey;


		int staticEval=evaluator.evaluatePosition(color);

		bool isMovingSideInCheck=moveGenerator.isInCheck(color);

		int nodeType=transpositionTable.getNodeType(currentZobristKey);


		// Reverse futility pruning
		if(!isMovingSideInCheck && // position not in check
			(bestHashMove==Move()||board.isQuietMove(bestHashMove)) && // TT move is null or non-capture
			nodeType!=EXACT){ // node type is not PV

			if(staticEval>=beta+150*depth)
				return staticEval;
		}


		if(depth<=0)
			return quiescentSearch(color,alpha,beta,depthFromRoot);


		int oppositeColor=(color==WHITE)?BLACK:WHITE;

		// Null move pruning
		if(
			!isMovingSideInCheck && // position not in check
			((board.whitePieces|board.blackPieces)^(board.pawns|board.kings))>0 &&  // pieces except kings and pawns exist (to prevent zugzwang)
			staticEval>=beta // static evaluation >= beta
			){

			const int NULL_MOVE_DEPTH_REDUCTION=3;
			int prevEnPassColumn=board.makeNullMove();
			int score=-search(oppositeColor,depth-NULL_MOVE_DEPTH_REDUCTION,0,-beta,-beta+1,depthFromRoot+1);
			board.makeNullMove();
			board.enPassantColumn=prevEnPassColumn;
			if(score>=beta)
				return score;
		}

		Board boardCopy=board;

		moveListGenerator.hashMove=bestHashMove;
		if(isRoot&&depth>1)
			moveListGenerator.hashMove=bestMove;

		int bestKillerMove=0;
		for(int i=0;i<killMovesNumber;i++)
			if(killerMovesCount[depthFromRoot][i]>killerMovesCount[depthFromRoot][bestKillerMove])
				bestKillerMove=i;
		moveListGenerator.killerMove=killerMovesTable[depthFromRoot][bestKillerMove];

		moveListGenerator.generateMoves(color,depthFromRoot,DO_SORT,ALL_MOVES);
		int maxEvaluation=-inf;
		char type=UPPER_BOUND;
		int movesSearched=0;
		bestHashMove=Move();
		int numberOfMoves=moveListGenerator.moveListSize[depthFromRoot];
		for(int currentMove=0;currentMove<moveListGenerator.moveListSize[depthFromRoot];currentMove++){
			Move move=moveListGenerator.moveList[depthFromRoot][currentMove];

			bool isMoveInteresting=(
				moveGenerator.isInCheck(oppositeColor)|| // checking move
				!board.isQuietMove(move)); // non-quiet move

			board.makeMove(move);

			int newStaticEval=evaluator.evaluatePosition(color);



			// Futility pruning
			// if(depth==1 &&
			// 	movesSearched>0 &&
			// 	!isMovingSideInCheck &&
			// 	newStaticEval<alpha-150 &&
			// 	!isMoveInteresting){

			// 	board=boardCopy;
			// 	continue;
			// }

			int extendDepth=0;
			if(moveGenerator.isInCheck(oppositeColor)) // if in check, search deeper for 1 ply
				extendDepth++;

			int score;
			if(movesSearched){ // Principal variation search

				// Late move reduction
				const int LMR_FULL_MOVES=3; // number of moves to search with full depth
				const int LMR_MIN_DEPTH=4; // don't reduct depth if it's more or equal to this value
				int LMR_DEPTH_REDUCTION=floor(log(float(depth)) * log(float(movesSearched)) / 3); // reduction of depth

				if(movesSearched>=LMR_FULL_MOVES && 
					!isMovingSideInCheck &&
					depth>=LMR_MIN_DEPTH && 
					!isMoveInteresting && // don't do LMR with interesting moves
					!isPvNode
					// historyHelper.getScore(color,move)<historyHelper.maxHistoryScore // history score is negative
					)
					score=-search(oppositeColor,depth-1-LMR_DEPTH_REDUCTION,0,-(alpha+1),-alpha,depthFromRoot+1);
				else
					score=alpha+1; // if LMR is restricted, do this to do PVS

				if(score>alpha){
					score=-search(oppositeColor,depth-1+extendDepth,0,-(alpha+1),-alpha,depthFromRoot+1);
					if(score>alpha&&score<beta)
						score=-search(oppositeColor,depth-1+extendDepth,0,-beta,-alpha,depthFromRoot+1);
				}
			}else
				score=-search(oppositeColor,depth-1+extendDepth,0,-beta,-alpha,depthFromRoot+1);


			board=boardCopy;
			movesSearched++;

			if(stopSearch)
				return 0;
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
					if(!isMoveInteresting){
						// update killer move
						bool killerStored=false; // flag "is killer move stored"
						int badKiller=-1; // if some killer move has only one storing, it can be replaced
						for(int i=0;i<killMovesNumber;i++){
							if(killerMovesTable[depthFromRoot][i]==move){
								killerMovesCount[depthFromRoot][i]++;
								killerStored=true;
								break;
							}
							if(killerMovesCount[depthFromRoot][i]==1)
								badKiller=i;
						}
						if(!killerStored){
							for(int i=0;i<killMovesNumber;i++){
								if(killerMovesTable[depthFromRoot][i]==Move()){
									killerMovesTable[depthFromRoot][i]=move;
									killerMovesCount[depthFromRoot][i]=1;
									killerStored=true;
									break;
								}
							}
						}
						if(!killerStored&&badKiller!=-1){
							killerMovesTable[depthFromRoot][badKiller]=move;
							killerMovesCount[depthFromRoot][badKiller]=1;
						}
					}

					if((board.whitePieces&board.blackPieces).getBit(move.getTargetSquare())==0) // move is not capture
						historyHelper.update(color,move,depth*depth);

					for(int previousMoves=0;previousMoves<currentMove;previousMoves++){ // negate all searched non-capture moves
						Move prevMove=moveListGenerator.moveList[depthFromRoot][previousMoves];
						if((board.whitePieces&board.blackPieces).getBit(prevMove.getTargetSquare())==0) // move is not capture
							historyHelper.update(color,prevMove,-(depth*depth));
					}

					transpositionTable.write(currentZobristKey,maxEvaluation,depth,LOWER_BOUND,boardCurrentAge,bestHashMove);
					return maxEvaluation;
				}
			}
		}
		
		transpositionTable.write(currentZobristKey,maxEvaluation,depth,type,boardCurrentAge,bestHashMove);
		return maxEvaluation;
	}

	Move pvLine[256];
	int pvLineSize;

	void getPvLine(int color){
		ull currentZobristKey=board.getZobristKey();

		if(transpositionTable.getNodeType(currentZobristKey)!=EXACT)
			return;

		auto [hashTableEvaluation, bestHashMove]=transpositionTable.get(currentZobristKey,0,0,0);

		if(bestHashMove!=Move()){
			pvLine[pvLineSize++]=bestHashMove;
			Board boardCopy=board;
			board.makeMove(bestHashMove);
			getPvLine((color==WHITE)?BLACK:WHITE);
			board=boardCopy;
		}
	}

	void iterativeDeepeningSearch(int color,int maxDepth){
		for(int i=0;i<maxDepth;i++)
			for(int j=0;j<killMovesNumber;j++){
				killerMovesTable[i][j]=Move();
				killerMovesTable[i][j]=0;
			}
		nodes=0;
		boardCurrentAge=board.age;
        searchStartTime = std::chrono::steady_clock::now();
        bestMove=Move();
        int lastScore;
        const int aspirationWindow=25;
		for(int depth=1;depth<=maxDepth;depth++){
			int alpha=-inf*2,beta=inf*2;
			// if(depth>1){
			// 	alpha=lastScore-aspirationWindow;
			// 	beta=lastScore+aspirationWindow;
			// }
			int score=search(color,depth,1,alpha,beta,0);
			// if(depth>1&&(score<=alpha)||(score>=beta)){
			// 	if(score<=alpha)
			// 		alpha=-inf*2;
			// 	else
			// 		beta=inf*2;
			// 	score=search(color,depth,1,alpha,beta,0);
			// }
			if(stopSearch)
				return;
	        std::chrono::steady_clock::time_point timeNow = std::chrono::steady_clock::now();
	        ll timeThinked=std::chrono::duration_cast<std::chrono::milliseconds> (timeNow - searchStartTime).count();
	        pvLineSize=0;
			lastScore=score;
	        // getPvLine(color);
			if(UCIout){
				cout<<"info depth "<<depth<<
				" score ";
				if(abs(MATE_SCORE-score)>maxDepth)
					cout<<"cp "<<score;
				else{
					cout<<"mate ";
					if(score>0)
						cout<<(MATE_SCORE-score);
					else
						cout<<(-MATE_SCORE-score);
				}
				cout<<" nodes "<<nodes<<
				" nps "<<(nodes*1000)/(timeThinked+1)<<
				" time "<<timeThinked<<
				" pv "<<bestMove.convertToUCI();
				// for(int i=0;i<pvLineSize;i++)
					// cout<<pvLine[i].convertToUCI()<<' ';

				cout<<endl;
			}
		}
	}
};


Searcher searcher;