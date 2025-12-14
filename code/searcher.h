// searches the best move

#pragma once

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

struct StackState{
	bool excludeTTmove=false;
	Move excludeMove;
	Move bestMove;
};

struct alignas(64) Worker{
	bool stopSearch;
	bool doneSearch;

	int boardCurrentAge;

	int rootScore;
	Move bestMove;

	ll nodes=0,singularExtended=0;

	alignas(64) MoveListGenerator moveListGenerator;
	alignas(64) HistoryHelper historyHelper;

	alignas(64) StackState searchStack[maxDepth];

	std::chrono::steady_clock::time_point searchStartTime;

	float texelSearch(Board& board,int color,float alpha,float beta,int depthFromRoot){
		float staticEval;
		if(moveListGenerator.isStalled(board,color) || evaluator.insufficientMaterialDraw(board))
			staticEval=evaluator.evaluateStalledPosition(board,color,depthFromRoot);
		else{
			staticEval=evaluator.evaluatePositionDeterministic(board);
			if(color==BLACK)
				staticEval=-staticEval;
		}

		float maxEvaluation=staticEval;

		alpha=max(alpha,staticEval);
		if(alpha>=beta)
			return maxEvaluation;

		Board boardCopy=board;
		moveListGenerator.generateMoves(board,historyHelper,color,depthFromRoot,DO_SORT,ONLY_CAPTURES);

		for(int currentMove=0;currentMove<moveListGenerator.moveListSize[depthFromRoot];currentMove++){
			Move move=moveListGenerator.moveList[depthFromRoot][currentMove];
			board.makeMove(move);

			float score=-texelSearch(board,(color==WHITE)?BLACK:WHITE,-beta,-alpha,depthFromRoot+1);

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

	int quiescentSearch(Board& board,int color,int alpha,int beta,int depthFromRoot){
		if(stopSearch)
			return 0;
		nodes++;


		ull currentZobristKey=board.getZobristKey();
		auto [hashTableEvaluation, bestHashMove]=transpositionTableQuiescent.get(board,currentZobristKey,0,alpha,beta);
		int nodeType=transpositionTableQuiescent.getNodeType(currentZobristKey);
		if(hashTableEvaluation!=NO_EVAL){
			return hashTableEvaluation;

			// alpha=max(alpha,hashTableEvaluation);
			// if(alpha>=beta)
			// 	return alpha;
		}
		moveListGenerator.hashMove=bestHashMove;

		int staticEval;
		if(moveListGenerator.isStalled(board,color) || evaluator.insufficientMaterialDraw(board))
			staticEval=evaluator.evaluateStalledPosition(board,color,depthFromRoot);
		else
			staticEval=evaluator.evaluatePosition(board,color);


		auto ttEntry=transpositionTableQuiescent.getEntry(board,currentZobristKey);
		if(ttEntry.evaluation!=NO_EVAL)
			staticEval=ttEntry.evaluation;

		int maxEvaluation=staticEval;

		bool isMovingSideInCheck=moveGenerator.isInCheck(board,color);

		int numOfPiecesOnBoard=board.numberOfPieces();

		alpha=max(alpha,staticEval);
		if(alpha>=beta){
			transpositionTableQuiescent.write(board,currentZobristKey,maxEvaluation,0,LOWER_BOUND,boardCurrentAge,bestHashMove);
			return maxEvaluation;
		}



		// moveListGenerator.killerMove=moveListGenerator.hashMove;
		Board boardCopy=board;
		moveListGenerator.generateMoves(board,historyHelper,color,depthFromRoot,DO_SORT,ONLY_CAPTURES);


		bool isFirstMove=1;
		char type=UPPER_BOUND;
		bestHashMove=Move();
		for(int currentMove=0;currentMove<moveListGenerator.moveListSize[depthFromRoot];currentMove++){
			Move move=moveListGenerator.moveList[depthFromRoot][currentMove];
			int sseScore=(move.score&((1<<10)-1))-15;
			// if(sseScore!=moveGenerator.sseEval(move.getTargetSquare(),color,move.getStartSquare())){
			// 	cout<<move.convertToUCI()<<' '<<sseScore<<' '<<moveGenerator.sseEval(move.getTargetSquare(),color,move.getStartSquare())<<' '<<move.score<<'\n';
			// 	cout<<board.occupancyPiece(move.getStartSquare())<<' '<<board.occupancyPiece(move.getTargetSquare())<<'\n';
			// 	exit(0);
			// }
			board.makeMove(move);
			// transpositionTableQuiescent.prefetch(board.getZobristKey());
			int newStaticEval=evaluator.evaluatePosition(board,color);
			int deltaPruningMargin=200;
			// if(sseScore<=-1)
			// 	deltaPruningMargin-=sseScore*100;
			// assert(sseScore>=0);
			if(((numOfPiecesOnBoard-1)>=6 && newStaticEval+deltaPruningMargin<alpha) || sseScore<=-1){
				board=boardCopy;
				continue;
			}

			int score=-quiescentSearch(board,(color==WHITE)?BLACK:WHITE,-beta,-alpha,depthFromRoot+1);


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
					transpositionTableQuiescent.write(board,currentZobristKey,maxEvaluation,0,LOWER_BOUND,boardCurrentAge,bestHashMove);
					return maxEvaluation;
				}
			}
		}
		transpositionTableQuiescent.write(board,currentZobristKey,maxEvaluation,0,type,boardCurrentAge,bestHashMove);
		return maxEvaluation;
	}

	#define killMovesNumber 2
	Move killerMovesTable[maxDepth][killMovesNumber];
	int killerMovesCount[maxDepth][killMovesNumber];
	int killerMovesAge[maxDepth][killMovesNumber];

	int staticEvaluationHistory[maxDepth];

	int search(Board& board,int color,int depth,int isRoot,int alpha,int beta,int depthFromRoot){
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

		if(moveListGenerator.isStalled(board,color) || (!isRoot&&evaluator.insufficientMaterialDraw(board)))
			return evaluator.evaluateStalledPosition(board,color,depthFromRoot);


		int staticEval=evaluator.evaluatePosition(board,color);
		bool improving=false;
		bool isMovingSideInCheck=moveGenerator.isInCheck(board,color);

		if(isMovingSideInCheck)
			staticEvaluationHistory[depthFromRoot]=NONE_SCORE;
		else{
			staticEvaluationHistory[depthFromRoot]=staticEval;
			if(depthFromRoot>=2){
				int previousEval=staticEvaluationHistory[depthFromRoot-2];
				if(previousEval==NONE_SCORE||previousEval<staticEval)
					improving=true;
			}
		}


		auto [hashTableEvaluation, bestHashMove]=transpositionTable.get(board,currentZobristKey,depth,alpha,beta);
		if(hashTableEvaluation!=NO_EVAL){
			if(!isPvNode)
				return hashTableEvaluation;

			staticEval=hashTableEvaluation;
		}
		auto ttEntry=transpositionTable.getEntry(board,currentZobristKey);
		if(ttEntry.evaluation!=NO_EVAL)
			staticEval=ttEntry.evaluation;

		// occuredPositions[board.age]=currentZobristKey;


		

		int nodeType=transpositionTable.getNodeType(currentZobristKey);


		// Reverse futility pruning
		if(!isMovingSideInCheck && // position not in check
			(bestHashMove==Move()||board.isQuietMove(bestHashMove)) && // TT move is null or non-capture
			nodeType!=EXACT){ // node type is not PV

			int margin=(150-improving*100)*max(depth,1);

			if(staticEval>=beta+margin)
				return staticEval;
		}


		if(depth<=0)
			return quiescentSearch(board,color,alpha,beta,depthFromRoot);


		int oppositeColor=(color==WHITE)?BLACK:WHITE;

		// Null move pruning
		if(
			!isMovingSideInCheck && // position not in check
			((board.whitePieces|board.blackPieces)^(board.pawns|board.kings))>0 &&  // pieces except kings and pawns exist (to prevent zugzwang)
			staticEval>=beta && // static evaluation >= beta
			!isPvNode
			){

			int R=2;
			if(depth>=6)
				R++;
			R+=(staticEval-beta)/100;
			int prevEnPassColumn=board.makeNullMove();
			int score=-search(board,oppositeColor,depth-1-R,0,-beta,-beta+1,depthFromRoot+1);
			board.makeNullMove();
			board.enPassantColumn=prevEnPassColumn;
			if(score>=beta)
				return score;
		}

		// if(
		// 	depth==1 &&
		// 	!isMovingSideInCheck){ // Razoring
		// 	int margin=500;

		// 	if(staticEval+margin<alpha){
		// 		int qEval=quiescentSearch(color,alpha-1,alpha,depthFromRoot);
		// 		if(qEval<alpha)
		// 			return qEval;
		// 	}
		// }

		Board boardCopy=board;

		moveListGenerator.hashMove=bestHashMove;
		if(isRoot&&depth>1)
			moveListGenerator.hashMove=bestMove;
		Move ttMove=moveListGenerator.hashMove;

		// int bestKillerMove=0;
		// for(int i=0;i<killMovesNumber;i++)
		// 	if(killerMovesAge[depthFromRoot][i]>killerMovesAge[depthFromRoot][bestKillerMove]&&killerMovesCount[depthFromRoot][i]>1)
		// 		bestKillerMove=i;

		int killerMove=-1,killerBackup=-1;

		// for(int i=0;i<killMovesNumber;i++){
		// 	if(killerMove==-1||killerMovesAge[depthFromRoot][i]>killerMovesAge[depthFromRoot][killerMove]){
		// 		killerBackup=killerMove;
		// 		killerMove=i;
		// 	}
		// 	else if(killerBackup==-1||killerMovesAge[depthFromRoot][i]>killerMovesAge[depthFromRoot][killerBackup])
		// 		killerBackup=i;
		// }
		// moveListGenerator.killerMove=killerMovesTable[depthFromRoot][killerMove];
		// moveListGenerator.killerBackup=killerMovesTable[depthFromRoot][killerBackup];

		// if(
		// 	depth>=6 &&
		// 	isPvNode &&
		// 	ttMove==Move()){

		// 	search(board,color,depth/2,0,alpha,beta,depthFromRoot+1);
		// 	moveListGenerator.hashMove=searchStack[depthFromRoot+1].bestMove;
		// }

		// if(
		// 	depth>=6 &&
		// 	isPvNode &&
		// 	ttMove==Move()){

		// 	depth--;
		// }

		moveListGenerator.generateMoves(board,historyHelper,color,depthFromRoot,DO_SORT,ALL_MOVES);

		int extendTTmove=0;
		// if(
		// 	ttMove!=Move() && 
		// 	depth>=6 && 
		// 	!searchStack[depthFromRoot].excludeTTmove && 
		// 	moveListGenerator.moveListSize[depthFromRoot]>1 && 
		// 	nodeType==LOWER_BOUND
		// 	){
		// 	auto ttEntry=transpositionTable.getEntry(board,currentZobristKey);

		// 	searchStack[depthFromRoot+1].excludeTTmove=true;
		// 	searchStack[depthFromRoot+1].excludeMove=ttMove;
		// 	int margin=80;
		// 	int singularScore=search(board,color,depth/2,0,ttEntry.evaluation-margin-1,ttEntry.evaluation-margin,depthFromRoot+1);

		// 	if(singularScore<ttEntry.evaluation-margin){
		// 		extendTTmove=1;
		// 		singularExtended++;
		// 		// cout<<board.generateFEN()<<' '<<ttMove.convertToUCI()<<' '<<ttEntry.evaluation<<' '<<singularScore<<' '<<int(ttEntry.depth)<<'\n';
		// 	}

		// 	searchStack[depthFromRoot+1].excludeTTmove=false;
		// }


		int maxEvaluation=-inf;
		char type=UPPER_BOUND;
		int movesSearched=0;
		int quietMovesSearched=0;
		bestHashMove=Move();
		int numberOfMoves=moveListGenerator.moveListSize[depthFromRoot];
		for(int currentMove=0;currentMove<moveListGenerator.moveListSize[depthFromRoot];currentMove++){
			Move move=moveListGenerator.moveList[depthFromRoot][currentMove];

			int extendDepth=0;

			if(move==searchStack[depthFromRoot].excludeMove){
				if(searchStack[depthFromRoot].excludeTTmove==true)
					continue;
			}

			// if(move==ttMove)
			// 	extendDepth+=extendTTmove;

			bool isMoveInteresting=(
				moveGenerator.isInCheck(board,oppositeColor)|| // checking move
				!board.isQuietMove(move)); // non-quiet move

			bool isCapture=(board.whitePieces|board.blackPieces).getBit(move.getTargetSquare());
			bool inCheck=moveGenerator.isInCheck(board,oppositeColor);
			bool isPromotion=(move.getPromotionFlag()>0);
			int sseEval=0;
			if(isCapture)
				sseEval=moveGenerator.sseEval(board,move.getTargetSquare(),color,move.getStartSquare());

			board.makeMove(move);

			// transpositionTable.prefetch(board.getZobristKey());

			int newStaticEval=evaluator.evaluatePosition(board,color);

			int futilityMargin=100*depth*depth;

			// if(isCapture){
			// 	if(sseEval)
			// }

			// Futility pruning
			if(movesSearched>0 &&
				!isMovingSideInCheck &&
				newStaticEval<alpha-futilityMargin &&
				!isMoveInteresting &&
				abs(MATE_SCORE-beta)>maxDepth && abs(alpha+MATE_SCORE)>maxDepth
				
				){

				board=boardCopy;
				continue;
			}


			int seeMargin[4]={0,2,4,9};

			if(movesSearched>0 &&
				!isPvNode &&
				!isMovingSideInCheck &&
				!inCheck &&
				depth<=3 &&
				sseEval<=-seeMargin[depth]){

				board=boardCopy;
				continue;
			}

			int historyValue=historyHelper.getScore(color,move)-historyHelper.maxHistoryScore;

			if(moveGenerator.isInCheck(board,oppositeColor)) // if in check, search deeper for 1 ply
				extendDepth++;

			int score;
			if(movesSearched){ // Principal variation search

				// Late move reduction
				const int LMR_FULL_MOVES=3; // number of moves to search with full depth
				const int LMR_MIN_DEPTH=3; // don't reduct depth if it's more or equal to this value
				int LMR_DEPTH_REDUCTION=floor(log(float(depth)) * log(float(movesSearched)) / 3+
										0.5*(!isPvNode)-0.5*(isPvNode)-
										1.5*float(historyValue)/historyHelper.maxHistoryScore+
										0.5*(!improving)); // reduction of depth

				if(ttMove!=Move()&&!board.isQuietMove(ttMove))
					LMR_DEPTH_REDUCTION++;
				if(LMR_DEPTH_REDUCTION<0)
					LMR_DEPTH_REDUCTION=0;

				// if(isRoot){
				// 	cout<<move.convertToUCI()<<' '<<LMR_DEPTH_REDUCTION<<' '<<float(historyValue)/historyHelper.maxHistoryScore<<'\n';
				// }


				// const int LMP_MIN_DEPTH=4;
				// const int LMP_FULL_MOVES=6;
				// // Late move pruning
				// if(
				// 	!isPvNode &&
				// 	!isMovingSideInCheck &&
				// 	!isMoveInteresting &&
				// 	depth>=LMP_MIN_DEPTH &&
				// 	movesSearched>=LMP_FULL_MOVES &&
				// 	historyValue<0){

				// 	board=boardCopy;
				// 	continue;
				// }


				if(!isMovingSideInCheck &&
					!isMoveInteresting &&
					LMR_DEPTH_REDUCTION>=depth){
					board=boardCopy;
					continue;
				}

				if(movesSearched>=LMR_FULL_MOVES && 
					!isMovingSideInCheck &&
					depth>=LMR_MIN_DEPTH && 
					!isMoveInteresting // don't do LMR with interesting moves
					// historyHelper.getScore(color,move)<historyHelper.maxHistoryScore // history score is negative
					){
					score=-search(board,oppositeColor,depth-1-LMR_DEPTH_REDUCTION,0,-(alpha+1),-alpha,depthFromRoot+1);
				}
				else
					score=alpha+1; // if LMR is restricted, do this to do PVS

				if(score>alpha){
					score=-search(board,oppositeColor,depth-1+extendDepth,0,-(alpha+1),-alpha,depthFromRoot+1);
					if(score>alpha&&score<beta)
						score=-search(board,oppositeColor,depth-1+extendDepth,0,-beta,-alpha,depthFromRoot+1);
				}
			}else
				score=-search(board,oppositeColor,depth-1+extendDepth,0,-beta,-alpha,depthFromRoot+1);


			board=boardCopy;
			movesSearched++;
			if(!isMoveInteresting)
				quietMovesSearched++;

			if(stopSearch)
				return 0;
			if(maxEvaluation<score){
				if(score>alpha)
					type=EXACT;
				maxEvaluation=score;
				bestHashMove=move;
				searchStack[depthFromRoot].bestMove=move;
				if(isRoot){
					bestMove=move;
					rootScore=score;
				}
				if(alpha<score)
					alpha=score;
				if(alpha>=beta){
					// if(board.isQuietMove(move)&&0){
					// 	// update killer move
					// 	bool killerStored=false; // flag "is killer move stored"
					// 	int badKiller=-1; // if some killer move has only one storing, it can be replaced
					// 	for(int i=0;i<killMovesNumber;i++){
					// 		if(killerMovesTable[depthFromRoot][i]==move){
					// 			killerMovesCount[depthFromRoot][i]++;
					// 			killerMovesAge[depthFromRoot][i]=nodes;
					// 			killerStored=true;
					// 			break;
					// 		}
					// 		// if(killerMovesCount[depthFromRoot][i]==1)
					// 		// 	badKiller=i;
					// 	}
					// 	if(!killerStored){
					// 		for(int i=0;i<killMovesNumber;i++){
					// 			if(killerMovesTable[depthFromRoot][i]==Move()){
					// 				killerMovesTable[depthFromRoot][i]=move;
					// 				killerMovesCount[depthFromRoot][i]=1;
					// 				killerMovesAge[depthFromRoot][i]=nodes;
					// 				killerStored=true;
					// 				break;
					// 			}
					// 		}
					// 	}
					// 	if(!killerStored){
					// 		if(badKiller==-1){
					// 			// get oldest killer
					// 			for(int i=0;i<killMovesNumber;i++)
					// 				if(badKiller==-1||killerMovesAge[depthFromRoot][i]<killerMovesAge[depthFromRoot][badKiller])
					// 					badKiller=i;
					// 		}
					// 		killerMovesTable[depthFromRoot][badKiller]=move;
					// 		killerMovesCount[depthFromRoot][badKiller]=1;
					// 		killerMovesAge[depthFromRoot][badKiller]=nodes;
					// 	}
					// }

					if((board.whitePieces&board.blackPieces).getBit(move.getTargetSquare())==0) // move is not capture
						historyHelper.update(color,move,depth*depth);

					for(int previousMoves=0;previousMoves<currentMove;previousMoves++){ // negate all searched non-capture moves
						Move prevMove=moveListGenerator.moveList[depthFromRoot][previousMoves];
						if((board.whitePieces&board.blackPieces).getBit(prevMove.getTargetSquare())==0) // move is not capture
							historyHelper.update(color,prevMove,-(depth*depth));
					}

					transpositionTable.write(board,currentZobristKey,maxEvaluation,depth,LOWER_BOUND,boardCurrentAge,bestHashMove);
					return maxEvaluation;
				}
			}
		}
		
		transpositionTable.write(board,currentZobristKey,maxEvaluation,depth,type,boardCurrentAge,bestHashMove);
		return maxEvaluation;
	}

	void startSearch(Board& board,int depth,int alpha,int beta){
		search(board,board.boardColor,depth,true,alpha,beta,0);
		doneSearch=true;
	}

	Move pvLine[256];
	int pvLineSize;

	void getPvLine(Board& board,int color){
		ull currentZobristKey=board.getZobristKey();

		if(transpositionTable.getNodeType(currentZobristKey)!=EXACT)
			return;

		auto [hashTableEvaluation, bestHashMove]=transpositionTable.get(board,currentZobristKey,0,0,0);

		if(bestHashMove!=Move()){
			pvLine[pvLineSize++]=bestHashMove;
			Board boardCopy=board;
			board.makeMove(bestHashMove);
			getPvLine(board,(color==WHITE)?BLACK:WHITE);
			board=boardCopy;
		}
	}

	Worker(){
		historyHelper.clear();
	}

	void init(){
		historyHelper.clear();
	}
};

struct Searcher{
	int threadNumber=1;
	vector<Worker>workers;
	bool stopIDsearch;
	bool stopWaitingThread;

	Searcher(){
		workers.resize(threadNumber);
	}

	void stopSearch(){
		for(int i=0;i<threadNumber;i++)
			workers[i].stopSearch=true;
	}

	void waitAndEndSearch(int timeToWait){
		stopWaitingThread=false;
	    auto beginSleep=std::chrono::steady_clock::now();
		while(!stopWaitingThread){
	        auto curr=std::chrono::steady_clock::now();
	        if(std::chrono::duration_cast<std::chrono::milliseconds> (curr - beginSleep).count()>=timeToWait)
	            break;
	        std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		stopIDsearch=true;
		stopSearch();
	}

	void iterativeDeepeningSearch(int maxDepth,int softBound,int hardBound){
		stopIDsearch=false;
		int color=mainBoard.boardColor;
		vector<thread>threadPool(threadNumber);
		vector<Board>boards(threadNumber,mainBoard);
		for(int i=0;i<threadNumber;i++){
			workers[i].nodes=0;
			workers[i].stopSearch=false;
		}
        std::chrono::steady_clock::time_point searchStartTime = std::chrono::steady_clock::now();
        thread waitThread(&Searcher::waitAndEndSearch,this,hardBound);
        Move bestMoves[257];
		Move bestMove;
		for(int depth=1;depth<=maxDepth;depth++){
			int alpha=-inf*2,beta=inf*2;
			int score;
			int nodes=0;
			if(depth){
				workers[0].search(mainBoard,mainBoard.boardColor,depth,1,alpha,beta,0);
				score=workers[0].rootScore;
				bestMove=workers[0].bestMove;
				nodes=workers[0].nodes;
				for(int i=1;i<threadNumber;i++)
					workers[i].bestMove=bestMove;
				workers[0].pvLineSize=0;
				// workers[0].getPvLine(mainBoard,mainBoard.boardColor);
			}else{
				for(int i=0;i<threadNumber;i++){
					workers[i].doneSearch=false;
					workers[i].stopSearch=false;
					threadPool[i]=thread(&Worker::startSearch,&workers[i],ref(boards[i]),depth,alpha,beta);
					// threadPool[i]=thread(&Worker::search,&workers[i],ref(boards[i]),boards[i].boardColor,depth,1,alpha,beta,0);
				}

				int firstFinishedThread=0;
				// threadPool[0].join();
				// stopSearch();
				// while(true){
				// 	bool stopped=false;
				// 	for(int i=0;i<threadNumber;i++)
				// 		if(workers[i].doneSearch){
				// 			stopped=true;
				// 			firstFinishedThread=i;
				// 			stopSearch();
				// 			break;
				// 		}
				// 	if(stopped)
				// 		break;
				// 	std::this_thread::sleep_for(std::chrono::milliseconds(1));
				// }

				for(int i=0;i<threadNumber;i++)
					threadPool[i].join();

				score=-inf;

				// score=workers[firstFinishedThread].rootScore;
				// bestMove=workers[firstFinishedThread].bestMove;


				for(int i=0;i<threadNumber;i++){
					if(score<workers[i].rootScore){
						score=workers[i].rootScore;
						bestMove=workers[i].bestMove;
					}
				}
				// cout<<firstFinishedThread<<'\n';
				for(int i=0;i<threadNumber;i++){
					// cout<<workers[i].rootScore<<' '<<workers[i].bestMove.convertToUCI()<<' '<<workers[i].nodes<<'\n';
					nodes+=workers[i].nodes;
					// cout<<workers[i].nodes<<'\n';
					// workers[i].bestMove=bestMove;
				}
			}
	        std::chrono::steady_clock::time_point timeNow = std::chrono::steady_clock::now();
	        ll timeThinked=std::chrono::duration_cast<std::chrono::milliseconds> (timeNow - searchStartTime).count();

			cout<<"info depth "<<depth<<
			" score ";
			if(MATE_SCORE-abs(score)>maxDepth)
				cout<<"cp "<<score;
			else{
				cout<<"mate ";
				if(score>0)
					cout<<(MATE_SCORE-score);
				else
					cout<<(-MATE_SCORE-score);
			}
			// cout<<" sing_extended "<<workers[0].singularExtended;
			cout<<" nodes "<<nodes<<
			" nps "<<(nodes*(long long)(1000))/(timeThinked+1)<<
			" time "<<timeThinked<<
			" pv "<<bestMove.convertToUCI()<<' ';

			bestMoves[depth]=bestMove;

			// for(int i=1;i<workers[0].pvLineSize;i++)
			// 	cout<<workers[0].pvLine[i].convertToUCI()<<' ';

			cout<<endl;
			if(stopIDsearch)
				break;
	        if(timeThinked>=softBound){
	        	if(depth>=3){
	        		if(bestMoves[depth]==bestMoves[depth-1]&&bestMoves[depth]==bestMoves[depth-2]){
	        			stopWaitingThread=true;
	        			break;
	        		}
	        	}
	        }

		}
		cout<<"bestmove "<<bestMove.convertToUCI()<<endl;
		if(waitThread.joinable())
			waitThread.join();
	}
};


Searcher searcher;