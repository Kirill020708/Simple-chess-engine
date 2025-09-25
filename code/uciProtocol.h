// A unit for uci communication


#ifndef SEARCHER
#define SEARCHER

#include "searcher.h"

#endif /* SEARCHER */


#ifndef BOARD
#define BOARD

#include "board.h"

#endif /* BOARD */


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

	void parseCommand(string command){
		if(command=="")
			return;
		vector<string>tokens=splitStr(command," ");
		string mainCommand=tokens[0];
		if(mainCommand=="quit")
			exit(0);
		if(mainCommand=="DBG"){
			// printDesk01(board.rooks);
			// perftester.perfTest(256);
			for(ll i=0;i<board.age;i++)
				cout<<occuredPositionsHelper.occuredPositions[i]<<'\n';
			return;
		}
		if(mainCommand=="uci"){
			cout<<"uciok\n";
			return;
		}
		if(mainCommand=="isready"){
			cout<<"readyok\n";
			return;
		}
		if(mainCommand=="makemove"){
			board.makeMove(Move(tokens[1]));
			return;
		}
		if(mainCommand=="position"){
			int movesIter=tokens.size();
			if(tokens[1]=="startpos"){
				movesIter=3;
				board=Board();
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
				board.initFromFEN(fen);
			}
			for(;movesIter<tokens.size();movesIter++)
				board.makeMove(Move(tokens[movesIter]));
		}
		if(mainCommand=="go"){
			int wtime=0,btime=0,winc=0,binc=0;
			int movetime=-1;
			int depth=256;
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
			}
			int timeToThink;
			if(board.boardColor==WHITE)
				timeToThink=wtime*0.025+winc;
			if(board.boardColor==BLACK)
				timeToThink=btime*0.025+binc;
			if(movetime!=-1)
				timeToThink=movetime;
			waitAndEndSearch(timeToThink);
			cout<<"bestmove "<<searcher.bestMove.convertToUCI()<<'\n';

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