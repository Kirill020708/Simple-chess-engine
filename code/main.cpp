#ifndef SEARCHER
#define SEARCHER

#include "searcher.h"

#endif /* SEARCHER */


#ifndef UCI
#define UCI

#include "uciProtocol.h"

#endif /* UCI */


#ifndef PERFT
#define PERFT

#include "perft.h"

#endif /* PERFT */

string squareToString(int square){
	return string(1,char((square&7)+'a'))+string(1,char(7-(square>>3)+'1'));
}

int main(){

	// cout<<sizeof(Board)<<'\n';
	// board.initFromFEN("8/k7/3p4/p2P1p2/P2P1P2/8/8/K7 w - - 0 1");
	// cout<<board.evaluation<<'\n';
	// board.makeMove(Move("a1d1"));
	// cout<<board.evaluation<<'\n';
	// cout<<pieceSquareTable.getPieceEval(ROOK,56,WHITE)<<' '<<pieceSquareTable.getPieceEval(ROOK,59,WHITE)<<'\n';
	// return 0;
	// board.makeMove(Move("e2e4"));
	// board.makeMove(Move("e7e5"));
	// cout<<board.evaluation<<'\n';
	// cout<<ttt.tableSize<<'\n';
	// perftester.perfTest(256);
	// searcher.iterativeDeepeningSearch(WHITE,100);
	// system("/Users/Apple/Desktop/projects/chesscpp/app");
	uciHelper.listenInput();
}


/*

cd /Users/Apple/Desktop/projects/chessEngv2/Simple-chess-engine/code
c++ main.cpp -o app -std=c++2a -O2
c++ main.cpp -o appPVS -std=c++2a -O2

./fastchess -recover -repeat -games 2 -rounds 50000 -ratinginterval 1 -scoreinterval 1 -autosaveinterval 0\
            -report penta=true -pgnout results.pgn\
            -srand 5895699939700649196 -resign movecount=3 score=600\
            -draw movenumber=34 movecount=8 score=20 -variant standard -concurrency 2\
            -openings file=UHO_Lichess_4852_v1.epd format=epd order=random plies=16 start=15619\
            -engine name=New tc=10+0.1 cmd=./Simple-chess-engine/code/appPVS dir=.\
            -engine name=Base tc=10+0.1 cmd=./Simple-chess-engine/code/app dir=.\
            -each proto=uci -pgnout result.pgn -sprt elo0=0 elo1=10 alpha=0.05 beta=0.05



./fastchess -recover -repeat -games 2 -rounds 50000 -ratinginterval 1 -scoreinterval 1 -autosaveinterval 0\
            -report penta=true -pgnout results.pgn\
            -srand 5895699939700649196 -resign movecount=3 score=600\
            -draw movenumber=34 movecount=8 score=20 -variant standard -concurrency 2\
            -openings file=8moves_v3.pgn format=pgn order=random\
            -engine name=New tc=10+0.1 cmd=./Simple-chess-engine/code/appPVS dir=.\
            -engine name=Base tc=10+0.1 cmd=./Simple-chess-engine/code/app dir=.\
            -each proto=uci -pgnout result.pgn -sprt elo0=0 elo1=10 alpha=0.05 beta=0.05



*/