// keys for Zobrist hashing

#pragma once


#ifndef DECLARS
#define DECLARS

#include "declars.h"

#endif /* DECLARS */


struct ZobristKeys{
    ull pieceKeys[64][2][7];
    ull colorKey;
    ull canCastle[16];//wh-lf,wh-rt,bl-lf,bl-rt
    ull enPassant[16];

    ZobristKeys(){
        for(ll i=0;i<64;i++)
            for(ll j=0;j<2;j++)
            	for(ll k=0;k<7;k++)
                	pieceKeys[i][j][k]=rng();
        colorKey=rng();
        for(ll i=0;i<16;i++)
            canCastle[i]=rng();
        for(ll i=0;i<16;i++)
            enPassant[i]=rng();
    }
};

// ZobristKeys zobristKeys;