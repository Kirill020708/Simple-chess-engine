// struct for easier work with bitboards


#pragma once

#ifndef DECLARS
#define DECLARS

#include "declars.h"

#endif /* DECLARS */

struct Bitboard {
    ull bitboard = 0;

    constexpr Bitboard() {}

    constexpr Bitboard(ull bitboard_) {
        bitboard = bitboard_;
    }

    constexpr inline bool getBit(ll i) const {
        return (bitboard & (1ull << i)) > 0;
    }

    constexpr inline int getLastBitNumber() const { //returns the number of first enabled bit
        if (!bitboard)
            return -1;
        return 63 - __builtin_clzll(bitboard);
    }

    constexpr inline int getFirstBitNumber() const { //returns the number of first enabled bit
        if (!bitboard)
            return -1;
        return __builtin_ctzll(bitboard);
    }

    constexpr inline int getFirstBitNumberAndExclude() { //returns the number of first enabled bit and disables it
        if (!bitboard)
            return -1;
        int b = __builtin_ctzll(bitboard);
        bitboard ^= (1ull << b);
        return b;
    }

    constexpr inline int popcnt() const {
        return __builtin_popcountll(bitboard);
    }

    constexpr operator ull() const {
        return bitboard;
    }

    constexpr Bitboard operator=(ull x) {
        bitboard = x;
        return Bitboard(bitboard);
    }

    constexpr Bitboard operator&=(ull x) {
        bitboard &= x;
        return Bitboard(bitboard);
    }

    constexpr Bitboard operator|=(ull x) {
        bitboard |= x;
        return Bitboard(bitboard);
    }

    constexpr Bitboard operator^=(ull x) {
        bitboard ^= x;
        return Bitboard(bitboard);
    }

    constexpr Bitboard operator>>=(ull x) {
        bitboard >>= x;
        return Bitboard(bitboard);
    }

    constexpr Bitboard operator<<=(ull x) {
        bitboard <<= x;
        return Bitboard(bitboard);
    }

    constexpr Bitboard operator>>=(int x) {
        bitboard >>= x;
        return Bitboard(bitboard);
    }

    constexpr Bitboard operator<<=(int x) {
        bitboard <<= x;
        return Bitboard(bitboard);
    }

    constexpr Bitboard operator=(Bitboard x) {
        bitboard = x.bitboard;
        return Bitboard(bitboard);
    }

    constexpr Bitboard operator&=(Bitboard x) {
        bitboard &= x.bitboard;
        return Bitboard(bitboard);
    }

    constexpr Bitboard operator|=(Bitboard x) {
        bitboard |= x.bitboard;
        return Bitboard(bitboard);
    }

    Bitboard operator^=(Bitboard x) {
        bitboard ^= x.bitboard;
        return Bitboard(bitboard);
    }
};

constexpr Bitboard operator&(Bitboard x, Bitboard y) {
    return Bitboard(x.bitboard & y.bitboard);
}

constexpr Bitboard operator|(Bitboard x, Bitboard y) {
    return Bitboard(x.bitboard | y.bitboard);
}

constexpr Bitboard operator^(Bitboard x, Bitboard y) {
    return Bitboard(x.bitboard ^ y.bitboard);
}

constexpr Bitboard operator~(Bitboard x) {
    return Bitboard(~x.bitboard);
}

constexpr Bitboard operator&(Bitboard x, ull y) {
    return Bitboard(x.bitboard & y);
}

constexpr Bitboard operator|(Bitboard x, ull y) {
    return Bitboard(x.bitboard | y);
}

constexpr Bitboard operator^(Bitboard x, ull y) {
    return Bitboard(x.bitboard ^ y);
}

constexpr Bitboard operator>>(Bitboard x, ull y) {
    return Bitboard(x.bitboard >> y);
}

constexpr Bitboard operator<<(Bitboard x, ull y) {
    return Bitboard(x.bitboard << y);
}

constexpr Bitboard operator>>(Bitboard x, int y) {
    return Bitboard(x.bitboard >> y);
}

constexpr Bitboard operator<<(Bitboard x, int y) {
    return Bitboard(x.bitboard << y);
}

constexpr bool operator!=(Bitboard x, Bitboard y) {
    return (x.bitboard != y.bitboard);
}

constexpr bool operator==(Bitboard x, Bitboard y) {
    return (x.bitboard == y.bitboard);
}

constexpr bool operator!=(Bitboard x, ull y) {
    return (x.bitboard != y);
}

constexpr bool operator!=(Bitboard x, int y) {
    return (x.bitboard != y);
}

constexpr bool operator==(Bitboard x, ull y) {
    return (x.bitboard == y);
}

constexpr bool operator==(Bitboard x, int y) {
    return (x.bitboard == y);
}
