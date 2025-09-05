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

	Bitboard pawnMoves(int square){
		int column=boardHelper.getColumnNumber(square);
		int row=boardHelper.getRowNumber(square);
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
			if(column>0&&(board.occupancy(square+7)==WHITE||(row==3&&board.enPassantColumn==column-1)))
				moves|=(1ull<<(square+7));
			if(column<7&&(board.occupancy(square+9)==WHITE||(row==3&&board.enPassantColumn==column+1)))
				moves|=(1ull<<(square+9));
		}
		return moves;
	}

	Bitboard knightMoves(int square){
		int occup=board.occupancy(square);
		Bitboard moves=boardHelper.knightMoves[square];
		if(occup==WHITE)
			moves&=(~board.whitePieces);
		if(occup==BLACK)
			moves&=(~board.blackPieces);
		return moves;
	}

	Bitboard bishopMoves(int square){
        ull key=(((magic.bishopAttackPruned[square]&ull(board.whitePieces|board.blackPieces))*magic.bishopsMagics[square])>>magic.bishopsShifts[square]);
        Bitboard moves=magic.bishopCaptures[square][key];
		int occup=board.occupancy(square);
		if(occup==WHITE)
			moves&=(~board.whitePieces);
		if(occup==BLACK)
			moves&=(~board.blackPieces);
		return moves;
	}

	Bitboard rookMoves(int square){
        ull key=(((magic.rookAttackPruned[square]&ull(board.whitePieces|board.blackPieces))*magic.rooksMagics[square])>>magic.rooksShifts[square]);
        Bitboard moves=magic.rookCaptures[square][key];
		int occup=board.occupancy(square);
		if(occup==WHITE)
			moves&=(~board.whitePieces);
		if(occup==BLACK)
			moves&=(~board.blackPieces);
		return moves;
	}

	Bitboard queenMoves(int square){
		return bishopMoves(square)|rookMoves(square);
	}

    bool isSquareAttackedByWhite();
    bool isSquareAttackedByBlack();

    bool isWhiteInCheck(){
    	return isSquareAttackedByWhite((board.kings&board.whitePieces).getFirstBitNumber());
    }

    bool isBlackInCheck(){
    	return isSquareAttackedByBlack((board.kings&board.blackPieces).getFirstBitNumber());
    }

    bool canWhiteCastleQueenside(){
    	return (!board.castlingWhiteQueensideBroke)&&
    			((boardHelper.generateMask(57,59)&(board.whitePieces|board.blackPieces))==0)&&
    			!isSquareAttackedByBlack(58)&&!isSquareAttackedByBlack(59)&&!isSquareAttackedByBlack(60);
    }

    bool canWhiteCastleKingside(){
    	return (!board.castlingWhiteKingsideBroke)&&
    			((boardHelper.generateMask(61,62)&(board.whitePieces|board.blackPieces))==0)&&
    			!isSquareAttackedByBlack(60)&&!isSquareAttackedByBlack(61)&&!isSquareAttackedByBlack(62);
    }

    bool canBlackCastleQueenside(){
    	return (!board.castlingBlackQueensideBroke)&&
    			((boardHelper.generateMask(1,3)&(board.whitePieces|board.blackPieces))==0)&&
    			!isSquareAttackedByWhite(2)&&!isSquareAttackedByWhite(3)&&!isSquareAttackedByWhite(4);
    }

    bool canBlackCastleKingside(){
    	return (!board.castlingBlackKingsideBroke)&&
    			((boardHelper.generateMask(5,6)&(board.whitePieces|board.blackPieces))==0)&&
    			!isSquareAttackedByWhite(4)&&!isSquareAttackedByWhite(5)&&!isSquareAttackedByWhite(6);
    }

	Bitboard kingMoves(int square){
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

	Bitboard moves(int square){
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

	bool isSquareAttackedByWhite(int square){
		Bitboard whitePawns=board.pawns&board.whitePieces;
		Bitboard pawnsAttack=(((whitePawns&(~boardHelper.getColumn(7)))>>7)|((whitePawns&(~boardHelper.getColumn(0)))>>9))&(~board.whitePieces);
		if(pawnsAttack.getBit(square))
			return true;
		if((board.whitePieces&board.knights&boardHelper.knightMoves[square]).getBit(square))
			return true;
		if((board.whitePieces&board.kings&boardHelper.kingMoves[square]).getBit(square))
			return true;
		Bitboard pieces=board.whitePieces&(~board.pawns)&(~board.knights)&(~board.kings)&queenMoves(square);
		while(pieces>0){
			int currentSquare=pieces.getFirstBitNumberAndExclude();
			if(moves(currentSquare).getBit(square))
				return true;
		}
		return false;
	}

	bool isSquareAttackedByBlack(int square){
		Bitboard blackPawns=board.pawns&board.blackPieces;
		Bitboard pawnsAttack=(((blackPawns&(~boardHelper.getColumn(0)))<<7)|((blackPawns&(~boardHelper.getColumn(7)))<<9))&(~board.blackPieces);
		if(pawnsAttack.getBit(square))
			return true;
		if((board.blackPieces&board.knights&boardHelper.knightMoves[square]).getBit(square))
			return true;
		if((board.blackPieces&board.kings&boardHelper.kingMoves[square]).getBit(square))
			return true;
		Bitboard pieces=board.blackPieces&(~board.pawns)&(~board.knights)&(~board.kings)&queenMoves(square);
		while(pieces>0){
			int currentSquare=pieces.getFirstBitNumberAndExclude();
			if(moves(currentSquare).getBit(square))
				return true;
		}
		return false;
	}
};

MoveGeneration moveGenerator;