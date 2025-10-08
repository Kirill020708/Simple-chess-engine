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
	// for(ll depth=1;depth<=15;depth++){
		// cout<<depth<<": ";
		// for(ll j=1;j<=40;j++)
			// cout<<int(floor(log(float(depth)) * log(float(j)) / 3))<<' ';
		// cout<<'\n';
	// }
	// cout<<sizeof(Board)<<'\n';
	// board.initFromFEN("4rrk1/1pqb1pb1/p2p3p/4pPp1/2P1B1P1/1P3N1P/P3Q3/1R2K2R w K - 5 22");

	// cout<<pieceSquareTable.getPiecePositionEval(KING,52,WHITE,32-3)<<'\n';
	// cout<<pieceSquareTable.getPiecePositionEval(KING,54,WHITE,32-3)<<'\n';
	// cout<<pieceSquareTable.getPiecePositionEval(KING,52-8,WHITE,32-3)<<'\n';
	// cout<<board.numberOfPieces()<<'\n';

	// moveListGenerator.generateMoves(WHITE,0,1,0);
	// 	for(int currentMove=0;currentMove<moveListGenerator.moveListSize[0];currentMove++){
	// 		Move move=moveListGenerator.moveList[0][currentMove];
	// 		cout<<move.convertToUCI()<<' '<<move.score<<'\n';
	// 		}
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
	// printDesk01(boardHelper.neighborColumns[34].bitboard);
	uciHelper.listenInput();
}


/*

cd /Users/Apple/Desktop/projects/chessEngv2/apps
c++ ../Simple-chess-engine/code/main.cpp -o app -std=c++2a -O2
c++ main.cpp -o appPVS -std=c++2a -O2

./fastchess -recover -repeat -games 2 -rounds 50000 -ratinginterval 1 -scoreinterval 1 -autosaveinterval 0\
            -report penta=true -pgnout results.pgn\
            -srand 5895699939700649196 -resign movecount=3 score=600\
            -draw movenumber=34 movecount=8 score=20 -variant standard -concurrency 2\
            -openings file=UHO_Lichess_4852_v1.epd format=epd order=random plies=16 start=15619\
            -engine name=New tc=10+0.1 cmd=./Simple-chess-engine/code/appPVS restart=on dir=.\
            -engine name=Base tc=10+0.1 cmd=./Simple-chess-engine/code/app restart=on dir=.\
            -each proto=uci -pgnout result.pgn -sprt elo0=0 elo1=10 alpha=0.05 beta=0.05

go movetime 1000
position startpos moves e2e4 e7e5
go movetime 1000

position fen 5r2/4k2P/R7/8/8/5PK1/8/8 w - - 0 82
go movetime 100

./fastchess -recover -repeat -games 2 -rounds 50000 -ratinginterval 1 -scoreinterval 1 -autosaveinterval 0\
            -report penta=true -pgnout results.pgn\
            -srand 5895699939700649196 -resign movecount=3 score=600\
            -draw movenumber=34 movecount=8 score=20 -variant standard -concurrency 2\
            -openings file=8moves_v3.pgn format=pgn order=random\
            -engine name=New tc=10+0.1 cmd=./apps/appV12_PWN2 restart=on dir=.\
            -engine name=Base tc=10+0.1 cmd=./apps/appV12_PWN restart=on dir=.\
            -each proto=uci -pgnout result.pgn -sprt elo0=0 elo1=10 alpha=0.05 beta=0.05



*/