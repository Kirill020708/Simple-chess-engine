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

	int getFirstBitNumber(){//returns the number of first enabled bit
		if(!bitboard)
			return -1;
		return __builtin_ctzll(bitboard);
	}

	int getFirstBitNumberAndExclude(){//returns the number of first enabled bit and disables it
		if(!bitboard)
			return -1;
		int b=__builtin_ctzll(bitboard);
		bitboard^=(1ull<<b);
		return b;
	}

	operator ull(){
		return bitboard;
	}

	Bitboard operator=(ull x){
		bitboard=x;
		return Bitboard(bitboard);
	}

	Bitboard operator&=(ull x){
		bitboard&=x;
		return Bitboard(bitboard);
	}

	Bitboard operator|=(ull x){
		bitboard|=x;
		return Bitboard(bitboard);
	}

	Bitboard operator^=(ull x){
		bitboard^=x;
		return Bitboard(bitboard);
	}

	Bitboard operator>>=(ull x){
		bitboard>>=x;
		return Bitboard(bitboard);
	}

	Bitboard operator<<=(ull x){
		bitboard<<=x;
		return Bitboard(bitboard);
	}

	Bitboard operator>>=(int x){
		bitboard>>=x;
		return Bitboard(bitboard);
	}

	Bitboard operator<<=(int x){
		bitboard<<=x;
		return Bitboard(bitboard);
	}

	Bitboard operator=(Bitboard x){
		bitboard=x.bitboard;
		return Bitboard(bitboard);
	}

	Bitboard operator&=(Bitboard x){
		bitboard&=x.bitboard;
		return Bitboard(bitboard);
	}

	Bitboard operator|=(Bitboard x){
		bitboard|=x.bitboard;
		return Bitboard(bitboard);
	}

	Bitboard operator^=(Bitboard x){
		bitboard^=x.bitboard;
		return Bitboard(bitboard);
	}
};

Bitboard operator&(Bitboard x,Bitboard y){
	return Bitboard(x.bitboard&y.bitboard);
}

Bitboard operator|(Bitboard x,Bitboard y){
	return Bitboard(x.bitboard|y.bitboard);
}

Bitboard operator^(Bitboard x,Bitboard y){
	return Bitboard(x.bitboard^y.bitboard);
}

Bitboard operator~(Bitboard x){
	return Bitboard(~x.bitboard);
}

Bitboard operator&(Bitboard x,ull y){
	return Bitboard(x.bitboard&y);
}

Bitboard operator|(Bitboard x,ull y){
	return Bitboard(x.bitboard|y);
}

Bitboard operator^(Bitboard x,ull y){
	return Bitboard(x.bitboard^y);
}

Bitboard operator>>(Bitboard x,ull y){
	return Bitboard(x.bitboard>>y);
}

Bitboard operator<<(Bitboard x,ull y){
	return Bitboard(x.bitboard<<y);
}

Bitboard operator>>(Bitboard x,int y){
	return Bitboard(x.bitboard>>y);
}

Bitboard operator<<(Bitboard x,int y){
	return Bitboard(x.bitboard<<y);
}

bool operator!=(Bitboard x,Bitboard y){
	return (x.bitboard!=y.bitboard);
}

bool operator==(Bitboard x,Bitboard y){
	return (x.bitboard==y.bitboard);
}

bool operator!=(Bitboard x,ull y){
	return (x.bitboard!=y);
}

bool operator!=(Bitboard x,int y){
	return (x.bitboard!=y);
}

bool operator==(Bitboard x,ull y){
	return (x.bitboard==y);
}

bool operator==(Bitboard x,int y){
	return (x.bitboard==y);
}