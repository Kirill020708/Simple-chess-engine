// perfomance test

#pragma once


#ifndef MOVEGENLIST
#define MOVEGENLIST

#include "moveListGeneration.h"

#endif /* MOVEGENLIST */

#ifndef BOARD
#define BOARD

#include "board.h"

#endif /* BOARD */


struct Perftester{
	bool stopPertft;

	int perft(int color,int depth){
		if(depth==0)
			return 1;
		Board boardCopy=board;
		moveListGenerator.generateMovesForPerft(color,depth);
		int sum=0;
		for(int currentMove=0;currentMove<moveListGenerator.moveListSize[depth];currentMove++){
			Move move=moveListGenerator.moveList[depth][currentMove];
			board.makeMove(move);
			sum+=perft(color==WHITE?BLACK:WHITE,depth-1);
			board=boardCopy;
		}
		return sum;
	}

	void perfTest(int maxDepth){
		Board boardCopy=board;
		for(int depth=1;depth<=maxDepth;depth++){
	        std::chrono::steady_clock::time_point timeBegin = std::chrono::steady_clock::now();
			int color=board.boardColor;
			moveListGenerator.generateMoves(color,depth,1,0);
			ll nodes=0;
			for(int currentMove=0;currentMove<moveListGenerator.moveListSize[depth];currentMove++){
				Move move=moveListGenerator.moveList[depth][currentMove];
				board.makeMove(move);
				int nmb=perft(color==WHITE?BLACK:WHITE,depth-1);
				cout<<move.convertToUCI()<<' '<<nmb<<'\n';
				board=boardCopy;
				nodes+=nmb;
			}
			std::chrono::steady_clock::time_point timeEnd = std::chrono::steady_clock::now();
	        ll time=std::chrono::duration_cast<std::chrono::microseconds> (timeEnd - timeBegin).count();
			cout<<"depth "<<depth<<" nodes "<<nodes<<" nodes/sec "<<nodes*(1000000)/time<<" time "<<ld(time)/1e6<<"s\n\n";
		}
	}
};

Perftester perftester;