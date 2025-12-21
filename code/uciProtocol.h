// A unit for uci communication

#pragma once


#ifndef SEARCHER
#define SEARCHER

#include "searcher.h"

#endif /* SEARCHER */


#ifndef BOARD
#define BOARD

#include "mainBoard.h"

#endif /* BOARD */


#ifndef EVALUATOR
#define EVALUATOR

#include "evaluation.h"

#endif /* EVALUATOR */


#ifndef PERFT
#define PERFT

#include "perft.h"

#endif /* PERFT */


void waitAndEndSearch(int timeToThink){
	// searcher.stopSearch=false;
	// thread th(&Searcher::iterativeDeepeningSearch,&searcher,mainBoard.boardColor,256);
	// this_thread::sleep_for(std::chrono::milliseconds(timeToThink));
	// searcher.stopSearch=true;
	// th.join();
}
// position startpos moves b1c3 b8c6 g1f3 d7d5 e2e3 e7e6 f1b5 c8d7 d2d4 g8f6 b5c6 d7c6 f3e5 f8d6 e5c6 b7c6 e1g1 e8g8 d1f3 e6e5 c1d2 e5e4 f3f5 d8b8 a1b1 b8c8 f5c8 a8c8 f2f3 c6c5 f3e4 d5e4 c3b5 a7a6 b5d6 c7d6 b1c1 f8e8 f1e1 d6d5 a2a3 c5c4 d2c3 c8d8 c1d1 d8b8 a3a4 b8d8 c3b4 d8a8 b4c3 a8d8 a4a5 d8a8 d1d2 a8d8 d2f2 d8a8 f2d2

void printDesk01(ull x){
	for(ll i=0;i<64;i++){
		cout<<((x&(1ull<<i))>0);
		if(i%8==7)
			cout<<'\n';
	}
}
#ifndef PERFT
#define PERFT

#include "perft.h"

#endif /* PERFT */

struct UCIcommunicationHepler{

	thread searcherThread;

	thread waitingThread;
	bool stopWaitingThread;
	int hardNodesOpt=1e9;

	void sleepCond(int ms){
		stopWaitingThread=0;
	    auto beginSleep=std::chrono::steady_clock::now();
		while(!stopWaitingThread){
	        auto curr=std::chrono::steady_clock::now();
	        if(std::chrono::duration_cast<std::chrono::milliseconds> (curr - beginSleep).count()>=ms)
	            break;
		}
	}

	void waitAndEndSearch(int timeToThink){
		// searcher.stopSearch=false;
		// searcherThread=thread(&Searcher::iterativeDeepeningSearch,&searcher,mainBoard.boardColor,256);
		// sleepCond(timeToThink);
		this_thread::sleep_for(std::chrono::milliseconds(timeToThink));
		// searcher.stopSearch=true;
		searcherThread.join();
	}

	void parseCommand(string command){
		if(command=="")
			return;
		vector<string>tokens=splitStr(command," ");
		string mainCommand=tokens[0];
		if(mainCommand=="quit")
			exit(0);
		if(mainCommand=="dbg"){
			// printDesk01(mainBoard.rooks);
			// perftester.perfTest(256);
			// for(ll i=0;i<mainBoard.age;i++)
				// cout<<occuredPositionsHelper.occuredPositions[i]<<'\n';
			// inline int sseEval(int square,int color,int firstAttacker){
			// cout<<evaluator.evaluateDoubledPawns()<<'\n';
			return;
		}
		if(mainCommand=="uci"){
			cout<<"uciok"<<endl;
			return;
		}
		if(mainCommand=="isready"){
			cout<<"readyok"<<endl;
			return;
		}
		if(mainCommand=="makemove"){
			stopWaitingThread=1;
			if(waitingThread.joinable())
				waitingThread.join();

			mainBoard.makeMove(Move(tokens[1]));
			return;
		}
		if(mainCommand=="eval"){
			cout<<"endgame weight: "<<mainBoard.endgameWeight()<<'\n';
			if(tokens[1]=="info")
				evaluator.showInfo=true;
			cout<<evaluator.evaluatePosition(mainBoard)<<" cp (white's perspective)"<<endl;
			evaluator.showInfo=false;
			return;
		}
		if(mainCommand=="position"){
			stopWaitingThread=1;
			if(waitingThread.joinable())
				waitingThread.join();

			int movesIter=tokens.size();
			if(tokens[1]=="startpos"){
				movesIter=3;
				mainBoard=Board();
			}
			if(tokens[1]=="fen"){
				string fen;
				for(int i=2;i<tokens.size();i++){
					if(tokens[i]=="moves"){
						movesIter=i+1;
						break;
					}
					fen+=tokens[i]+" ";
				}
				mainBoard.initFromFEN(fen);
			}
			for(;movesIter<tokens.size();movesIter++){
				mainBoard.makeMove(Move(tokens[movesIter]));
			}
		}
		if(mainCommand=="go"){
			stopWaitingThread=1;
			if(waitingThread.joinable())
				waitingThread.join();

			int wtime=-1,btime=-1,winc=-1,binc=0;
			int movetime=-1;
			int depth=256;
			int nodes=1e9;
			int nodesh=1e9;
			for(int i=1;i<tokens.size();i++){
				if(tokens[i]=="wtime")
					wtime=stoi(tokens[i+1]);
				if(tokens[i]=="btime")
					btime=stoi(tokens[i+1]);
				if(tokens[i]=="winc")
					winc=stoi(tokens[i+1]);
				if(tokens[i]=="binc")
					binc=stoi(tokens[i+1]);

				if(tokens[i]=="movetime")
					movetime=stoi(tokens[i+1]);

				if(tokens[i]=="depth")
					depth=stoi(tokens[i+1]);

				if(tokens[i]=="nodes")
					nodes=stoi(tokens[i+1]);

				if(tokens[i]=="nodesh")
					nodesh=stoi(tokens[i+1]);
			}
			int timeToThink=1e9;
			int basetime=0;
			if(mainBoard.boardColor==WHITE && wtime!=-1){
				timeToThink=wtime*0.025+winc;
				basetime=wtime;
			}
			if(mainBoard.boardColor==BLACK && btime!=-1){
				timeToThink=btime*0.025+binc;
				basetime=btime;
			}
			if(movetime!=-1)
				timeToThink=movetime;
			// cout<<timeToThink<<'\n';
			int softBound=inf,hardBound=inf;
			if(wtime!=-1){
				softBound=timeToThink*0.75;
				hardBound=min(int(timeToThink*2),basetime-100);
			}
			if(movetime!=-1){
				softBound=hardBound=timeToThink=movetime;

			}
			searcher.iterativeDeepeningSearch(256,softBound,hardBound,nodes,hardNodesOpt);
			// waitAndEndSearch(timeToThink);
			// waitingThread=thread(&UCIcommunicationHepler::waitAndEndSearch,this,timeToThink);
		}
		if(mainCommand=="perft"){
			perftester.perfTest(stoi(tokens[1]));
		}
		if(mainCommand=="stop"){
			stopWaitingThread=1;
			if(waitingThread.joinable())
				waitingThread.join();
		}
		if(mainCommand=="setoption"){
			if(tokens[2]=="HardNodesLimit"){
				hardNodesOpt=stoi(tokens[3]);
			}
		}
	}

	bool stopListening;

	void listenInput(){
		stopListening=0;
		while(true){
			string command;
			getline(cin,command);
			parseCommand(command);
		}
	}
};

UCIcommunicationHepler uciHelper;