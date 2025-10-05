// struct for board

#ifndef MOVE
#define MOVE

#include "move.h"

#endif /* MOVE */


#ifndef BITBOARD
#define BITBOARD

#include "bitboard.h"

#endif /* BITBOARD */


#ifndef BOARDHELP
#define BOARDHELP

#include "boardHelper.h"

#endif /* BOARDHELP */


#ifndef PIECESQTABLE
#define PIECESQTABLE

#include "pieceSquareTable.h"

#endif /* PIECESQTABLE */


#ifndef ZOBRIST
#define ZOBRIST

#include "zobrist.h"

#endif /* ZOBRIST */

struct OccuredPositionsHelper{
	ull occuredPositions[256]; // positions occured in current variation; for testing repetition
};

OccuredPositionsHelper occuredPositionsHelper;

struct Board{
	char boardColor;
	int evaluation;

	Bitboard whitePieces,blackPieces;
	Bitboard pawns,knights,bishops,rooks,queens,kings;

	bool castlingWhiteQueensideBroke,castlingWhiteKingsideBroke,castlingBlackQueensideBroke,castlingBlackKingsideBroke;

	char enPassantColumn;

	ull zobristKey;

	char age;

	int lastIrreversibleMoveAge=-1; // age of last irreversible move (capture/pawn move), for testing repetition

	inline int numberOfPieces(){
		return (whitePieces|blackPieces).popcnt();
	}

	inline int occupancy(int square){
		if(square<0||square>63)
			return ERROR;

		return WHITE*(whitePieces.getBit(square))+
			BLACK*(blackPieces.getBit(square))+
			EMPTY*(!((whitePieces|blackPieces).getBit(square)));


		if(square<0||square>63)
			return ERROR;
		if(whitePieces.getBit(square))
			return WHITE;
		if(blackPieces.getBit(square))
			return BLACK;
		return EMPTY;
	}

	inline int occupancyPiece(int square){
		return ERROR*(square<0||square>63)+
			PAWN*(pawns.getBit(square))+
			KNIGHT*(knights.getBit(square))+
			BISHOP*(bishops.getBit(square))+
			ROOK*(rooks.getBit(square))+
			QUEEN*(queens.getBit(square))+
			KING*(kings.getBit(square))+
			NOPIECE;

		if(square<0||square>63)
			return ERROR;
		if(pawns.getBit(square))
			return PAWN;
		if(knights.getBit(square))
			return KNIGHT;
		if(bishops.getBit(square))
			return BISHOP;
		if(rooks.getBit(square))
			return ROOK;
		if(queens.getBit(square))
			return QUEEN;
		if(kings.getBit(square))
			return KING;
		return NOPIECE;
	}

	inline bool isQuietMove(Move move){ // is move quiet (no capture/promotion)
		return !(occupancy(move.getTargetSquare())!=EMPTY||move.getPromotionFlag()!=0);
	}

	void initZobristKey(){
		for(int square=0;square<64;square++){
			int piece=occupancyPiece(square);
			int pieceColor=occupancy(square);
			if(pieceColor!=EMPTY)
				zobristKey^=zobristKeys.pieceKeys[square][pieceColor][piece];
		}
	}

	ull getZobristKey(){
		return zobristKey^
		
				((boardColor==WHITE)?0:zobristKeys.colorKey)^

				zobristKeys.canCastle[castlingWhiteQueensideBroke+
									(castlingWhiteKingsideBroke<<1)+
									(castlingBlackQueensideBroke<<2)+
									(castlingBlackKingsideBroke<<3)]^

				zobristKeys.enPassant[enPassantColumn];
	}

	inline void clearPosition(int square){
		int piece=occupancyPiece(square);
		int pieceColor=occupancy(square);
		if(pieceColor!=EMPTY)
			zobristKey^=zobristKeys.pieceKeys[square][pieceColor][piece];

		evaluation-=pieceSquareTable.getPieceEval(piece,square,pieceColor,numberOfPieces());
		whitePieces&=(~(1ull<<square));
		blackPieces&=(~(1ull<<square));
		pawns&=(~(1ull<<square));
		knights&=(~(1ull<<square));
		bishops&=(~(1ull<<square));
		rooks&=(~(1ull<<square));
		queens&=(~(1ull<<square));
		kings&=(~(1ull<<square));
	}

	inline void putPiece(int square,int color,int pieceType){
		evaluation+=pieceSquareTable.getPieceEval(pieceType,square,color,numberOfPieces());
		if(color==WHITE)
			whitePieces|=(1ull<<square);
		if(color==BLACK)
			blackPieces|=(1ull<<square);
		if(pieceType==PAWN)
			pawns|=(1ull<<square);
		if(pieceType==KNIGHT)
			knights|=(1ull<<square);
		if(pieceType==BISHOP)
			bishops|=(1ull<<square);
		if(pieceType==ROOK)
			rooks|=(1ull<<square);
		if(pieceType==QUEEN)
			queens|=(1ull<<square);
		if(pieceType==KING)
			kings|=(1ull<<square);

		if(color!=EMPTY)
			zobristKey^=zobristKeys.pieceKeys[square][color][pieceType];
	}

	inline void movePiece(int startSquare,int targetSquare){
		int color=occupancy(startSquare);
		int movingPiece=occupancyPiece(startSquare);
		clearPosition(startSquare);
		clearPosition(targetSquare);
		putPiece(targetSquare,color,movingPiece);
	}

	inline int makeNullMove(){
		boardColor=(boardColor==WHITE)?BLACK:WHITE;
		int enPassantColumnPrev=enPassantColumn;
		enPassantColumn=NO_EN_PASSANT;
		return enPassantColumnPrev;
	}

	inline void makeMove(Move move){
		if((whitePieces|blackPieces).getBit(move.getTargetSquare())||pawns.getBit(move.getStartSquare())) // check if move is irreversible
			lastIrreversibleMoveAge=age;

		occuredPositionsHelper.occuredPositions[age++]=getZobristKey();
		boardColor=(boardColor==WHITE)?BLACK:WHITE;
		int startSquare=move.getStartSquare();
		int targetSquare=move.getTargetSquare();
		int color=occupancy(startSquare);
		int movingPiece=occupancyPiece(startSquare);
		enPassantColumn=NO_EN_PASSANT;
		if(movingPiece==PAWN){
			if((abs(targetSquare-startSquare)&1)&&occupancy(targetSquare)==EMPTY){//enPassant capture
				if(color==WHITE)
					clearPosition(targetSquare+8);
				if(color==BLACK)
					clearPosition(targetSquare-8);
			}
			clearPosition(startSquare);
			clearPosition(targetSquare);
			if(move.getPromotionFlag()!=NOPIECE)
				movingPiece=move.getPromotionFlag();
			putPiece(targetSquare,color,movingPiece);
			if(abs(targetSquare-startSquare)==16)//updEnPassant
				enPassantColumn=boardHelper.getColumnNumber(startSquare);
		}else if(movingPiece==KING){
			movePiece(startSquare,targetSquare);
			if(startSquare==60&&targetSquare==58)//white left castling
				movePiece(56,59);
			if(startSquare==60&&targetSquare==62)//white right castling
				movePiece(63,61);
			if(startSquare==4&&targetSquare==2)//black left castling
				movePiece(0,3);
			if(startSquare==4&&targetSquare==6)//black right castling
				movePiece(7,5);
			if(color==WHITE)
				castlingWhiteQueensideBroke=castlingWhiteKingsideBroke=1;
			if(color==BLACK)
				castlingBlackQueensideBroke=castlingBlackKingsideBroke=1;
		}else{
			movePiece(startSquare,targetSquare);
		}
		if(startSquare==56||targetSquare==56)
			castlingWhiteQueensideBroke=1;
		if(startSquare==63||targetSquare==63)
			castlingWhiteKingsideBroke=1;
		if(startSquare==0||targetSquare==0)
			castlingBlackQueensideBroke=1;
		if(startSquare==7||targetSquare==7)
			castlingBlackKingsideBroke=1;
	}

	void initFromFEN(string fen){
		age=0;
		vector<string>tokens=splitStr(fen," ");
		string position=tokens[0],currentColor=tokens[1],castlingAvailability=tokens[2],enPassantSquare=tokens[3];
		whitePieces=blackPieces=pawns=knights=bishops=rooks=queens=kings=0;
		castlingWhiteQueensideBroke=castlingWhiteKingsideBroke=castlingBlackQueensideBroke=castlingBlackKingsideBroke=1;
		enPassantColumn=NO_EN_PASSANT;

		int currentSquare=0;
		for(auto piece:position){
			if(piece=='/')
				continue;
			if(piece>='0'&&piece<='9'){
				currentSquare+=piece-'0';
				continue;
			}
			ull currentSquareBit=(1ull<<currentSquare);
			if(piece>='A'&&piece<='Z'){
				whitePieces|=currentSquareBit;
				piece+='a'-'A';
			}else
				blackPieces|=currentSquareBit;
			if(piece=='p')
				pawns|=currentSquareBit;
			if(piece=='n')
				knights|=currentSquareBit;
			if(piece=='b')
				bishops|=currentSquareBit;
			if(piece=='r')
				rooks|=currentSquareBit;
			if(piece=='q')
				queens|=currentSquareBit;
			if(piece=='k')
				kings|=currentSquareBit;
			currentSquare++;
		}

		if(currentColor=="w")
			boardColor=WHITE;
		else
			boardColor=BLACK;

		for(auto cst:castlingAvailability){
			if(cst=='Q')
				castlingWhiteQueensideBroke=0;
			if(cst=='K')
				castlingWhiteKingsideBroke=0;
			if(cst=='q')
				castlingBlackQueensideBroke=0;
			if(cst=='k')
				castlingBlackKingsideBroke=0;
		}

		if(enPassantSquare!="-")
			enPassantColumn=enPassantSquare[0]-'a';

		evaluation=0;
		for(int square=0;square<64;square++)
			if(occupancy(square)!=EMPTY){
				evaluation+=pieceSquareTable.getPieceEval(occupancyPiece(square),square,occupancy(square),numberOfPieces());
			}
		initZobristKey();
	}

	Board(){
		age=0;
		boardColor=WHITE;
		whitePieces=boardHelper.generateMask(48,63);
		blackPieces=boardHelper.generateMask(0,15);
		pawns=boardHelper.generateMask(8,15)|boardHelper.generateMask(48,55);
		knights=(1ull<<1)|(1ull<<6)|(1ull<<57)|(1ull<<62);
		bishops=(1ull<<2)|(1ull<<5)|(1ull<<58)|(1ull<<61);
		rooks=(1ull<<0)|(1ull<<7)|(1ull<<56)|(1ull<<63);
		queens=(1ull<<3)|(1ull<<59);
		kings=(1ull<<4)|(1ull<<60);
		castlingWhiteQueensideBroke=castlingWhiteKingsideBroke=castlingBlackQueensideBroke=castlingBlackKingsideBroke=0;
		enPassantColumn=NO_EN_PASSANT;
		evaluation=0;
		initZobristKey();
	}
};

Board board;