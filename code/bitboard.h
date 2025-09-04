#ifndef DECLARS
#define DECLARS

#include "declars.h"

#endif /* DECLARS */

struct Bitboard{
	ull bitboard=0;

	Bitboard(){}

	Bitboard(ull bitboard_){
		bitboard=bitboard_;
	}

	bool getBit(ll i){
		return (bitboard&(1ull<<i))>0;
	}

	int getFirstBitNumber(){
		if(!bitboard)
			return -1;
		return __builtin_ctzll(bitboard);
	}

	int getFirstBitNumberAndExclude(){
		if(!bitboard)
			return -1;
		int b=__builtin_ctzll(bitboard);
		bitboard^=(1ull<<b);
		return b;
	}

	Bitboard operator=(ull x){
		bitboard=x;
	}

	Bitboard operator&=(ull x){
		bitboard&=x;
	}

	Bitboard operator|=(ull x){
		bitboard|=x;
	}

	Bitboard operator^=(ull x){
		bitboard^=x;
	}

	Bitboard operator=(bitboard x){
		bitboard=x;
	}

	Bitboard operator&=(bitboard x){
		bitboard&=x;
	}

	Bitboard operator|=(bitboard x){
		bitboard|=x;
	}

	Bitboard operator^=(bitboard x){
		bitboard^=x;
	}
};

Bitboard operator&(Bitboard x,Bitboard y){
	return Bitboard(x.bitboard&y.bitboard);
}

Bitboard operator|(Bitboard x,Bitboard y){
	return Bitboard(x.bitboard&y.bitboard);
}

Bitboard operator~(Bitboard x){
	return Bitboard(~x.bitboard);
}