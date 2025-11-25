// Transposition table

#pragma once


#ifndef DECLARS
#define DECLARS

#include "declars.h"

#endif /* DECLARS */

struct TableEntry{
	ull key;
	int evaluation;
	char depth,type=NONE;
	int age;
	Move bestMove;

	TableEntry(){}

	TableEntry(ull key_,int evaluation_,char depth_,char type_,int age_,Move bestMove_){
		key=key_;
		evaluation=evaluation_;
		depth=depth_;
		type=type_;
		age=age_;
		bestMove=bestMove_;
	}
};

struct TranspositionTable{
	const ll tableSize=ll(memoryUsageMB)*1024*1024/sizeof(TableEntry);
	TableEntry table[ll(memoryUsageMB)*1024*1024/sizeof(TableEntry)];

	// mutex TTmutex;

	inline void write(Board& board,ull key,int evaluation,int depth,int type,int age,Move bestMove){
		// if(MATE_SCORE-abs(evaluation)<=maxDepth){
		// 	if(evaluation>0)
		// 		evaluation=MATE_SCORE;
		// 	else
		// 		evaluation=-MATE_SCORE;
		// }
		int index=key%tableSize;
		if(table[index].type!=NONE){
			if(table[index].key!=key){
				if(table[index].age>=board.age)
					return;
			}else{
				if(table[index].depth>depth)
					return;
				if(table[index].depth==depth&&table[index].type==EXACT)
					return;
			}
		}
		// TTmutex.lock();
		table[index]={key,evaluation,char(depth),char(type),(age),bestMove};
		// TTmutex.unlock();
	}

	inline pair<int,Move> get(Board& board,ull key,int depth,int alpha,int beta,int depthFromRoot){
		int index=key%tableSize;
		if(table[index].type==NONE)
			return {NO_EVAL,Move()};
		if(table[index].key!=key)
			return {NO_EVAL,Move()};
		if(table[index].depth<depth)
			return {NO_EVAL,table[index].bestMove};

		int eval=NO_EVAL;
		if(table[index].type==EXACT)
			eval=table[index].evaluation;

		if(table[index].type==LOWER_BOUND && table[index].evaluation>=beta) // when calculated TT node we got alpha>=beta
			eval=table[index].evaluation;

		if(table[index].type==UPPER_BOUND && table[index].evaluation<alpha)
			eval=table[index].evaluation;

		// if(abs(eval)==MATE_SCORE){
		// 	if(eval>0)
		// 		eval-=depthFromRoot;
		// 	else
		// 		eval+=depthFromRoot;
		// }

		return {eval,table[index].bestMove};
	}

	inline TableEntry getEntry(Board& board,ull key,int depthFromRoot){
		int index=key%tableSize;
		if(table[index].type==NONE)
			return TableEntry();
		if(table[index].key!=key)
			return TableEntry();

		auto entry=table[index];
		// if(abs(entry.evaluation)==MATE_SCORE){
		// 	if(entry.evaluation>0)
		// 		entry.evaluation-=depthFromRoot;
		// 	else
		// 		entry.evaluation+=depthFromRoot;
		// }

		return entry;
	}

	inline int getDepth(Board& board,ull key){
		int index=key%tableSize;
		if(table[index].type==NONE)
			return -10;
		if(table[index].key!=key)
			return -10;
		// if(table[index].depth<depth)
		// 	return {NO_EVAL,table[index].bestMove};

		return table[index].depth;
	}

	inline int getNodeType(ull key){
		int index=key%tableSize;
		if(table[index].key!=key)
			return NONE;
		return table[index].type;
	}

	inline void prefetch(ull key){
		int index=key%tableSize;
		__builtin_prefetch(&table[index]);
	}
};

TranspositionTable transpositionTable;

TranspositionTable transpositionTableQuiescent;
