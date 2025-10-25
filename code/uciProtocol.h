// A unit for uci communication

#pragma once


#ifndef SEARCHER
#define SEARCHER

#include "searcher.h"

#endif /* SEARCHER */


#ifndef BOARD
#define BOARD

#include "board.h"

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
	searcher.stopSearch=false;
	thread th(&Searcher::iterativeDeepeningSearch,&searcher,board.boardColor,256);
	this_thread::sleep_for(std::chrono::milliseconds(timeToThink));
	searcher.stopSearch=true;
	th.join();
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

    ll kl=0;
	thread searcherThread;

	thread waitingThread;
	bool stopWaitingThread;

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
		searcher.stopSearch=false;
		searcherThread=thread(&Searcher::iterativeDeepeningSearch,&searcher,board.boardColor,256);
		// sleepCond(timeToThink);
		this_thread::sleep_for(std::chrono::milliseconds(timeToThink));
		searcher.stopSearch=true;
		searcherThread.join();
		cout<<searcher.bestMove.convertToUCI()<<' '<<searcher.scc<<' '<<searcher.dd<<endl;
	}

	void parseCommand(string command){
        cerr<<command<<'\n';
        kl++;
        if(kl<=3)
            return;
		board.initFromFEN(command);
        waitAndEndSearch(40);
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