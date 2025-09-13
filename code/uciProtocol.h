// A unit for uci communication


#ifndef SEARCHER
#define SEARCHER

#include "searcher.h"

#endif /* SEARCHER */


#ifndef BOARD
#define BOARD

#include "board.h"

#endif /* BOARD */

void waitAndEndSearch(int ms){
	if(ms<10)
		ms=10;
	thread th(&Searcher::iterativeDeepeningSearch,&searcher,board.boardColor,256);
	this_thread::sleep_for(std::chrono::milliseconds(ms));
	searcher.stopSearch=true;
	th.join();
}

struct UCIcommunicationHepler{

	void parseCommand(string command){
		if(command=="")
			return;
		vector<string>tokens=splitStr(command," ");
		string mainCommand=tokens[0];
		if(mainCommand=="quit")
			exit(0);
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
					movetime+=stoi(tokens[i+1]);
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