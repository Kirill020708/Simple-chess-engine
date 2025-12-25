#ifndef SEARCHER
#define SEARCHER

#include "searcher.h"

#endif /* SEARCHER */


struct DataGenerator{
	string outputPath="/Users/Apple/Desktop/projects/chessEngv2/NNUE/datagen.txt";
	int softNodesLimit=5000,hardNodesLimit=50000;
	int threadNumber=1;

	int randomMoveCount=8;

	int resignMoveCount=3,resignScore=600;

	int drawMoveCount=8,minDrawMoveCount=34,drawScore=20;

	vector<Worker>workers;
	vector<Board>boards;

	vector<vector<string>>results;

	vector<bool>finished;

	void playGame(int workerIdx){
		int curMoveNumber=0;
		int result=-10;

		int winningStreakW,winningStreakB;
		int drawStreak;

		winningStreakW=winningStreakB=drawStreak=0;

		int curRandomMoveCount=randomMoveCount;
		if(rng()%2==0)
			curRandomMoveCount++;

		while(true){
			curMoveNumber++;
			if(curMoveNumber<=curRandomMoveCount){

				workers[workerIdx].moveListGenerator.generateMoves(boards[workerIdx],workers[workerIdx].historyHelper,
																   boards[workerIdx].boardColor,0,DONT_SORT,ALL_MOVES);
				int movesCount=workers[workerIdx].moveListGenerator.moveListSize[0];
				Move randomMove=workers[workerIdx].moveListGenerator.moveList[0][rngT()%movesCount];
				boards[workerIdx].makeMove(randomMove,workers[workerIdx].nnueEvaluator);

			}else{

				workers[workerIdx].IDsearch(boards[workerIdx],256,softNodesLimit,hardNodesLimit);
				int score=workers[workerIdx].rootScore;
				if(boards[workerIdx].boardColor==BLACK)
					score=-score;
				if(abs(score)<=9000)
					results[workerIdx].push_back(boards[workerIdx].generateFEN()+" | "+to_string(score)+" | ");
				boards[workerIdx].makeMove(workers[workerIdx].bestMove,workers[workerIdx].nnueEvaluator);


				if(score>=resignScore)
					winningStreakW++;
				else
					winningStreakW=0;

				if(score<=-resignScore)
					winningStreakB++;
				else
					winningStreakB=0;

				if(abs(score)<=drawScore)
					drawStreak++;
				else
					drawStreak=0;
			}
				// cout<<curMoveNumber<<'\n';

			if(workers[workerIdx].moveListGenerator.isStalled(boards[workerIdx],boards[workerIdx].boardColor) || 
			   evaluator.insufficientMaterialDraw(boards[workerIdx])){

				int resultEval=evaluator.evaluateStalledPosition(boards[workerIdx],boards[workerIdx].boardColor,0);
				if(boards[workerIdx].boardColor==BLACK)
					resultEval=-resultEval;

				if(resultEval>0)
					result=1;
				else if(resultEval<0)
					result=-1;
				else
					result=0;
				break;
			}

			int moves50=(boards[workerIdx].age-boards[workerIdx].lastIrreversibleMoveAge-1);
			if(moves50>=50){
				result=0;
				break;
			}

			if(winningStreakW>=resignMoveCount){
				result=1;
				break;
			}

			if(winningStreakB>=resignMoveCount){
				result=-1;
				break;
			}

			if(drawStreak>=drawMoveCount && curMoveNumber>=minDrawMoveCount){
				result=0;
				break;
			}

			if(finished[workerIdx]==true)
				break;
		}

		string resultStr;
		if(result==1)
			resultStr="1.0";
		if(result==0)
			resultStr="0.5";
		if(result==-1)
			resultStr="0.0";

		for(auto &str:results[workerIdx])
			str+=resultStr;
		finished[workerIdx]=true;
	}

	void generateData(int numberOfGames){
		alwaysReplace=true;
		workers.resize(threadNumber);
		boards.resize(threadNumber,mainBoard);
		results.resize(threadNumber);
		finished.resize(threadNumber,false);
		vector<thread>threadPool(threadNumber);

		for(int i=0;i<threadNumber;i++){
			workers[i].nnueEvaluator=mainNnueEvaluator;
			mainBoard.initNNUE(workers[i].nnueEvaluator);
			for(ll j=0;j<256;j++){
				for(ll j1=0;j1<2;j1++){
					workers[i].killerMovesTable[j][j1]=Move();
					workers[i].killerMovesCount[j][j1]=0;
					workers[i].killerMovesAge[j][j1]=0;
				}
			}
		}

		ofstream out(outputPath);

		for(int i=0;i<threadNumber;i++)
			threadPool[i]=thread(&DataGenerator::playGame,this,i);

		int curGame=0,positionsNumber=0;


	    std::chrono::steady_clock::time_point timeStart = std::chrono::steady_clock::now();

	    cout<<endl;

		while(true){
			int finishedThread=-1;
			for(int i=0;i<threadNumber;i++)
				if(finished[i]){
					finishedThread=i;
					break;
				}
			if(finishedThread==-1){
				std::this_thread::sleep_for(std::chrono::microseconds(100));
				continue;
			}
			curGame++;
			threadPool[finishedThread].join();
			positionsNumber+=results[finishedThread].size();
			for(auto str:results[finishedThread])
				out<<str<<'\n';

			boards[finishedThread]=mainBoard;
			results[finishedThread]=vector<string>();
			finished[finishedThread]=false;

			workers[finishedThread].nnueEvaluator=mainNnueEvaluator;
			mainBoard.initNNUE(workers[finishedThread].nnueEvaluator);

	        std::chrono::steady_clock::time_point timeNow = std::chrono::steady_clock::now();
	        int elapsedTime=std::chrono::duration_cast<std::chrono::milliseconds> (timeNow - timeStart).count();

	        if(curGame>1){
	        	cout << "\033[2K";
		        cout<<"\033[1A\r";
	        	cout << "\033[2K";
		        cout<<"\033[1A\r";
	        	cout << "\033[2K";
		        cout<<"\033[1A\r";
	        	cout << "\033[2K";
		        cout<<"\033[1A\r";
	        	cout << "\033[2K";
		        cout<<"\033[1A\r";
	        	cout << "\033[2K";
		        cout<<"\033[1A\r";
	        	cout << "\033[2K";
		        cout.flush();
		    }

	        cout<<"Games played: "<<curGame<<endl;
	        cout<<"Positions saved: "<<positionsNumber<<endl;
	        cout<<"Positions per game: "<<positionsNumber/curGame<<endl;
	        int sec=elapsedTime/1000;
	        int min=sec/60;
	        sec%=60;
	        int hrs=min/60;
	        min%=60;
	        cout<<"Total time: "<<hrs<<"h "<<min<<"m "<<sec<<"s"<<endl;
	        cout<<"Games per sec: "<<fixed<<setprecision(1)<<curGame*1000.0/elapsedTime<<endl;
	        cout<<"Positions per sec: "<<positionsNumber*1000/elapsedTime<<endl;
	        if(curGame==numberOfGames){
	        	for(int i=0;i<threadNumber;i++)
	        		if(i!=finishedThread){
	        			finished[i]=true;
	        			threadPool[i].join();
	        		}

	        	cout<<"Done!\n"<<endl;
	        	break;
	        }
			threadPool[finishedThread]=thread(&DataGenerator::playGame,this,finishedThread);
		}
		alwaysReplace=false;
	}

};

DataGenerator dataGenerator;