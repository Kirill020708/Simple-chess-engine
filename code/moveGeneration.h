// generator of bitboards of moves for a piece

#pragma once


#ifndef BOARD
#define BOARD

#include "board.h"

#endif /* BOARD */

#ifndef BOARDHELP
#define BOARDHELP

#include "boardHelper.h"

#endif /* BOARDHELP */

#ifndef MAGIC
#define MAGIC

#include "magicNumbers.h"

#endif /* MAGIC */


struct MoveGeneration{

	inline Bitboard pawnMoves(int square){
		Bitboard moves;
		int col=board.occupancy(square);
		Bitboard pieces=board.whitePieces|board.blackPieces;
		Bitboard shiftedPieces=((col==WHITE) ? ((pieces^(1ull<<square))>>8) : ((pieces^(1ull<<square))<<8));
		Bitboard opponentPieces=(col==BLACK)?board.whitePieces:board.blackPieces;

		moves|=(boardHelper.pawnMoves[col][square]&(~(pieces|shiftedPieces)));

		moves|=(boardHelper.pawnCaptures[col][square]&(opponentPieces));

		int column=boardHelper.getColumnNumber(square);

		if(boardHelper.getRowNumber(square)==(4-(col==WHITE))){
			if(board.enPassantColumn==column-1)
				moves|=boardHelper.pawnCaptureLeft[col][square];

			if(board.enPassantColumn==column+1)
				moves|=boardHelper.pawnCaptureRight[col][square];
		}

		return moves;
/*
		
		
		int occup=board.occupancy(square);
		Bitboard moves;
		if(occup==WHITE){
			if(board.occupancy(square-8)==EMPTY){
				moves|=(1ull<<(square-8));
				if(row==6&&board.occupancy(square-16)==EMPTY)
					moves|=(1ull<<(square-16));
			}
			if(column>0&&(board.occupancy(square-9)==BLACK||(row==3&&board.enPassantColumn==column-1)))
				moves|=(1ull<<(square-9));
			if(column<7&&(board.occupancy(square-7)==BLACK||(row==3&&board.enPassantColumn==column+1)))
				moves|=(1ull<<(square-7));
		}
		if(occup==BLACK){
			if(board.occupancy(square+8)==EMPTY){
				moves|=(1ull<<(square+8));
				if(row==1&&board.occupancy(square+16)==EMPTY)
					moves|=(1ull<<(square+16));
			}
			if(column>0&&(board.occupancy(square+7)==WHITE||(row==4&&board.enPassantColumn==column-1)))
				moves|=(1ull<<(square+7));
			if(column<7&&(board.occupancy(square+9)==WHITE||(row==4&&board.enPassantColumn==column+1)))
				moves|=(1ull<<(square+9));
		}
		return moves;
		*/
	}

	inline Bitboard knightMoves(int square){
		int occup=board.occupancy(square);
		Bitboard moves=boardHelper.knightMoves[square];
		if(occup==WHITE)
			moves&=(~board.whitePieces);
		if(occup==BLACK)
			moves&=(~board.blackPieces);
		return moves;
	}

	inline Bitboard bishopMoves(int square){
        ull key=(((magic.bishopAttackPruned[square]&ull(board.whitePieces|board.blackPieces))*magic.bishopsMagics[square])>>magic.bishopsShifts[square]);
        Bitboard moves=magic.bishopCaptures[square][key];
		int occup=board.occupancy(square);
		if(occup==WHITE)
			moves&=(~board.whitePieces);
		if(occup==BLACK)
			moves&=(~board.blackPieces);
		return moves;
	}

	inline Bitboard rookMoves(int square){
        ull key=(((magic.rookAttackPruned[square]&ull(board.whitePieces|board.blackPieces))*magic.rooksMagics[square])>>magic.rooksShifts[square]);
        Bitboard moves=magic.rookCaptures[square][key];
		int occup=board.occupancy(square);
		if(occup==WHITE)
			moves&=(~board.whitePieces);
		if(occup==BLACK)
			moves&=(~board.blackPieces);
		return moves;
	}

	inline Bitboard queenMoves(int square){
		return bishopMoves(square)|rookMoves(square);
	}

    inline bool isSquareAttackedByWhite();
    inline bool isSquareAttackedByBlack();

    inline bool isWhiteInCheck(){
    	return isSquareAttackedByBlack((board.kings&board.whitePieces).getFirstBitNumber());
    }

    inline bool isBlackInCheck(){
    	return isSquareAttackedByWhite((board.kings&board.blackPieces).getFirstBitNumber());
    }

    inline bool isInCheck(int color){
    	if(color==WHITE)
    		return isWhiteInCheck();
    	return isBlackInCheck();
    }

    inline bool canWhiteCastleQueenside(){
    	return (!board.castlingWhiteQueensideBroke)&&
    			((boardHelper.generateMask(57,59)&(board.whitePieces|board.blackPieces))==0)&&
    			!isSquareAttackedByBlack(58)&&!isSquareAttackedByBlack(59)&&!isSquareAttackedByBlack(60);
    }

    inline bool canWhiteCastleKingside(){
    	return (!board.castlingWhiteKingsideBroke)&&
    			((boardHelper.generateMask(61,62)&(board.whitePieces|board.blackPieces))==0)&&
    			!isSquareAttackedByBlack(60)&&!isSquareAttackedByBlack(61)&&!isSquareAttackedByBlack(62);
    }

    inline bool canBlackCastleQueenside(){
    	return (!board.castlingBlackQueensideBroke)&&
    			((boardHelper.generateMask(1,3)&(board.whitePieces|board.blackPieces))==0)&&
    			!isSquareAttackedByWhite(2)&&!isSquareAttackedByWhite(3)&&!isSquareAttackedByWhite(4);
    }

    inline bool canBlackCastleKingside(){
    	return (!board.castlingBlackKingsideBroke)&&
    			((boardHelper.generateMask(5,6)&(board.whitePieces|board.blackPieces))==0)&&
    			!isSquareAttackedByWhite(4)&&!isSquareAttackedByWhite(5)&&!isSquareAttackedByWhite(6);
    }

	inline Bitboard kingMoves(int square){
		// this_thread::sleep_for(std::chrono::milliseconds(1));
		int occup=board.occupancy(square);
		Bitboard moves=boardHelper.kingMoves[square];
		if(occup==WHITE){
			if(canWhiteCastleQueenside())
				moves|=(1ull<<58);
			if(canWhiteCastleKingside())
				moves|=(1ull<<62);
			moves&=(~board.whitePieces);
		}
		if(occup==BLACK){
			if(canBlackCastleQueenside())
				moves|=(1ull<<2);
			if(canBlackCastleKingside())
				moves|=(1ull<<6);
			moves&=(~board.blackPieces);
		}
		return moves;
	}

	inline Bitboard moves(int square){
		if(board.pawns.getBit(square))
			return pawnMoves(square);
		if(board.knights.getBit(square))
			return knightMoves(square);
		if(board.bishops.getBit(square))
			return bishopMoves(square);
		if(board.rooks.getBit(square))
			return rookMoves(square);
		if(board.queens.getBit(square))
			return queenMoves(square);
		if(board.kings.getBit(square))
			return kingMoves(square);
		return 0;
	}

	inline bool isSquareAttackedByWhite(int square){
		Bitboard whitePawns=board.pawns&board.whitePieces;
		Bitboard pawnsAttack=(((whitePawns&(~boardHelper.getColumn(7)))>>7)|((whitePawns&(~boardHelper.getColumn(0)))>>9))&(~board.whitePieces);
		if(pawnsAttack.getBit(square))
			return true;
		if(board.whitePieces&board.knights&boardHelper.knightMoves[square])
			return true;
		if(board.whitePieces&board.kings&boardHelper.kingMoves[square])
			return true;


		Bitboard bishopRays=bishopMoves(square);
		if(board.whitePieces&board.bishops&bishopRays)
			return true;

		Bitboard rookRays=rookMoves(square);
		if(board.whitePieces&board.rooks&rookRays)
			return true;

		Bitboard queenRays=bishopRays|rookRays;
		if(board.whitePieces&board.queens&queenRays)
			return true;

		return false;
	}

	inline bool isSquareAttackedByBlack(int square){
		Bitboard blackPawns=board.pawns&board.blackPieces;
		Bitboard pawnsAttack=(((blackPawns&(~boardHelper.getColumn(0)))<<7)|((blackPawns&(~boardHelper.getColumn(7)))<<9))&(~board.blackPieces);
		if(pawnsAttack.getBit(square))
			return true;
		if(board.blackPieces&board.knights&boardHelper.knightMoves[square])
			return true;
		if(board.blackPieces&board.kings&boardHelper.kingMoves[square])
			return true;

		Bitboard bishopRays=bishopMoves(square);
		if(board.blackPieces&board.bishops&bishopRays)
			return true;

		Bitboard rookRays=rookMoves(square);
		if(board.blackPieces&board.rooks&rookRays)
			return true;

		Bitboard queenRays=bishopRays|rookRays;
		if(board.blackPieces&board.queens&queenRays)
			return true;
		return false;
	}

	inline bool isSquareAttacked(int square,int color){
		if(color==WHITE)
			return isSquareAttackedByWhite(square);
		return isSquareAttackedByBlack(square);
	}

	inline int getSmallestAttacker(int square,int color){
		int oppositeColor=(color==WHITE)?BLACK:WHITE;
		Bitboard friendPieces=(color==WHITE)?board.whitePieces:board.blackPieces;
		Bitboard opponentPieces=(color==WHITE)?board.blackPieces:board.whitePieces;

		if(boardHelper.pawnCaptureLeft[oppositeColor][square]&board.pawns&friendPieces)
			return boardHelper.pawnCaptureLeft[oppositeColor][square].getFirstBitNumber();
		if(boardHelper.pawnCaptureRight[oppositeColor][square]&board.pawns&friendPieces)
			return boardHelper.pawnCaptureRight[oppositeColor][square].getFirstBitNumber();


		if(friendPieces&board.knights&boardHelper.knightMoves[square])
			return (friendPieces&board.knights&boardHelper.knightMoves[square]).getFirstBitNumber();

		Bitboard bishopRays=bishopMoves(square);
		if(friendPieces&board.bishops&bishopRays)
			return (friendPieces&board.bishops&bishopRays).getFirstBitNumber();

		Bitboard rookRays=rookMoves(square);
		if(friendPieces&board.rooks&rookRays)
			return (friendPieces&board.rooks&rookRays).getFirstBitNumber();

		Bitboard queenRays=bishopRays|rookRays;
		if(friendPieces&board.queens&queenRays)
			return (friendPieces&board.queens&queenRays).getFirstBitNumber();

		if((friendPieces&board.kings&boardHelper.kingMoves[square]))
			return (friendPieces&board.kings&boardHelper.kingMoves[square]).getFirstBitNumber();

		return -1;
	}

	int pieceMaterial[7]={0,1,3,3,5,10,100000};
	int evalStack[32];

	inline int sseEval(int square,int color,int firstAttacker){
		Board boardCopy=board;
		int captureNumber=1;
		int eval=0;
		evalStack[0]=0;
		while(true){
			int attacker;
			if(captureNumber==1)
				attacker=firstAttacker;
			else
				attacker=getSmallestAttacker(square,color);

			// cout<<attacker<<'\n';

			int mult=(captureNumber&1)?1:-1;

			if(attacker==-1){
				eval*=mult;
				break;
			}

			int capturedPiece=board.occupancyPiece(square);
			int attackingPiece=board.occupancyPiece(attacker);

			bool isPromotion=0;
			if(attackingPiece==PAWN){
				if(color==WHITE)
					isPromotion=(square<8);
				else
					isPromotion=(square>=56);
			}

			board.makeMove(Move(attacker,square,isPromotion*QUEEN));
			if(attackingPiece==KING){
				if(isInCheck(color))
					break;
			}

			evalStack[captureNumber++]=pieceMaterial[capturedPiece]+isPromotion*(pieceMaterial[QUEEN]-pieceMaterial[PAWN]);

			eval+=evalStack[captureNumber-1];
			if(eval-pieceMaterial[attackingPiece]>0 || attackingPiece==KING){
				break;
			}
			eval=-eval;
			color=(color==WHITE)?BLACK:WHITE;
		}
		board=boardCopy;
		evalStack[captureNumber]=0;

		// for(ll i=0;i<captureNumber;i++)
		// 	cout<<evalStack[i]<<' ';
		// cout<<'\n';

		for(int i=captureNumber-2;i>=2;i--){
			evalStack[i]=max(0,-evalStack[i+1]+evalStack[i]);
		}
		// for(ll i=0;i<captureNumber;i++)
		// 	cout<<evalStack[i]<<' ';
		// cout<<'\n';

		return evalStack[1]-evalStack[2];
	}
};

MoveGeneration moveGenerator;