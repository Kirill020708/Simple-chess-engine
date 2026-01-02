// magic bitboards for fast sliding piece's moves generation

#pragma once

#ifndef BITBOARD
#define BITBOARD

#include "bitboard.h"

#endif /* BITBOARD */

struct Magics {
    ull edgeU, edgeD, edgeL, edgeR;
    ull rookAttack[64], bishopAttack[64];
    ull rookAttackPruned[64], bishopAttackPruned[64]; // without piece and edges
    ull rookPrunedNmb[64], bishopPrunedNmb[64];
    ll rookPrunedIndex[64][20], bishopPrunedIndex[64][20];
    ull rooksMagics[64] = {
        10137478479502440012ull, 17145961700703386477ull, 15518305240712109487ull, 15502629082721999800ull,
        4193972124930483676ull,  11477168860624767122ull, 6875237889119500437ull,  16937768485683208750ull,
        8771155528856450741ull,  18171596392305083769ull, 18262352840762349386ull, 10567625886380114701ull,
        9063652596908763250ull,  14875410470049448239ull, 1160670555829171014ull,  1598059397584789198ull,
        6710797665474415114ull,  14276571427806194723ull, 11482100681958118119ull, 16529541798253298775ull,
        13767415680798335099ull, 11531406378669230719ull, 12008938003080613182ull, 7818530391603688382ull,
        17800579523214433270ull, 7883082745702014287ull,  1906495547883635245ull,  8936263829305140268ull,
        11676786485559189845ull, 14760515648899517331ull, 9371448143576448067ull,  7724759376157751864ull,
        9375947990451646996ull,  798761092864132890ull,   1866367944792375470ull,  16680029393589806417ull,
        7466603260574162851ull,  11880658305082196663ull, 16057432455325957836ull, 17896421121976105328ull,
        13985205741552214031ull, 897120395254671038ull,   17682121985142033013ull, 15664789979249321495ull,
        8744765063631905084ull,  333816113109876486ull,   10754498333110435857ull, 9953471137871259278ull,
        1740771429689995075ull,  16136051809681691136ull, 6582511271051475420ull,  6141429976430494821ull,
        13643553376003657932ull, 4686797951527237968ull,  8830110941385212928ull,  5995319684117565952ull,
        8548785413157117626ull,  17574273233881044154ull, 6373783022867189902ull,  617398689101447179ull,
        8929503777273395102ull,  16639220314030567410ull, 13206596106847773100ull, 15096269914834848770ull};
    ull rooksShifts[64] = {50, 51, 51, 51, 51, 51, 51, 50, 51, 53, 52, 52, 52, 52, 53, 52, 52, 52, 52, 52, 52, 52,
                           52, 52, 51, 53, 52, 52, 52, 52, 53, 52, 51, 53, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,
                           52, 52, 53, 52, 52, 53, 52, 52, 52, 52, 53, 52, 51, 52, 51, 51, 51, 51, 52, 52};
    ull bishopsMagics[64] = {
        8874507741750757418ull,  13447810502835863975ull, 10867220087762523649ull, 16908793413501839346ull,
        7285702835922782489ull,  16413934037643260769ull, 17389885981721826ull,    8508880745534064633ull,
        16351339167999035358ull, 6037689421256196359ull,  8604356126437343476ull,  11609334333483998403ull,
        1896151695793620963ull,  15406548265367328429ull, 651028253069723967ull,   17978682258377168910ull,
        10569954779168078663ull, 2989329159977438226ull,  11603527299161849888ull, 13708071024997567152ull,
        5411425754457687672ull,  222865768999875947ull,   11197215954505584212ull, 13038203028665700381ull,
        8402490887401667768ull,  1829251227638999253ull,  5574184035388778944ull,  16421438775577389898ull,
        10915021116010784381ull, 10076246351998087105ull, 16229427831930096643ull, 15586136196577888271ull,
        18125968878698798538ull, 1456898315457197074ull,  10552634401232531641ull, 8407725162962737914ull,
        15228440548658544821ull, 14819616983592810727ull, 12175768719875834945ull, 3041081014508243520ull,
        13101272237816430676ull, 11556379649874261670ull, 10511462585876432320ull, 17531674512545420352ull,
        4653524755219134297ull,  12301670394766306958ull, 5465125879862892948ull,  15535210968434073765ull,
        8857456877579293394ull,  5053603922316201570ull,  1292241750486690785ull,  4064223580744189309ull,
        5692758571436081227ull,  6104383780882187203ull,  14114707315520439767ull, 3383461313946192658ull,
        9650913130966224839ull,  4340942563168821301ull,  17540725033583376611ull, 182700155045476628ull,
        10686127629856093192ull, 6529300064762742817ull,  17484165035488879457ull, 7293973607400696526ull};
    ull bishopsShifts[64] = {57, 59, 59, 59, 59, 59, 59, 57, 58, 59, 59, 59, 58, 59, 58, 59, 59, 59, 56, 56, 56, 56,
                             58, 59, 58, 58, 56, 53, 53, 56, 59, 59, 59, 59, 56, 53, 53, 56, 59, 59, 59, 59, 56, 56,
                             56, 56, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 57, 59, 58, 58, 59, 59, 58, 57};
    Bitboard rookCaptures[64][(1ull << 14)], bishopCaptures[64][(1ull << 14)];

    void initRook(ll f) {
        unordered_map<ull, ll> mp;
        for (ull msk = 0; msk < (1ull << rookPrunedNmb[f]); msk++) {
            ull walls = 0;
            ll it = 0;
            ull pc = msk;
            ll i = 0;
            while (pc) {
                ll i1 = __builtin_ctzll(pc);
                pc >>= i1;
                pc >>= 1;
                i += i1;
                walls |= (1ull << rookPrunedIndex[f][i]);
                i++;
            }
            // assert(f||walls!=281474976711038ull);
            ull captures = 0;
            for (ll i = f;; i--) {
                captures |= (1ull << i);
                if (!(i & 7) || (walls & (1ull << i)))
                    break;
            }
            for (ll i = f;; i++) {
                captures |= (1ull << i);
                if ((i & 7) == 7 || (walls & (1ull << i)))
                    break;
            }
            for (ll i = f;; i -= 8) {
                captures |= (1ull << i);
                if (!(i >> 3) || (walls & (1ull << i)))
                    break;
            }
            for (ll i = f;; i += 8) {
                captures |= (1ull << i);
                if ((i >> 3) == 7 || (walls & (1ull << i)))
                    break;
            }
            ull key = ((walls * rooksMagics[f]) >> rooksShifts[f]);
            rookCaptures[f][key] = captures;
        }
    }

    void initBishop(ll f) {
        for (ull msk = 0; msk < (1ull << bishopPrunedNmb[f]); msk++) {
            ull walls = 0;
            ll it = 0;
            ull pc = msk;
            ll i = 0;
            while (pc) {
                ll i1 = __builtin_ctzll(pc);
                pc >>= i1;
                pc >>= 1;
                i += i1;
                walls |= (1ull << bishopPrunedIndex[f][i]);
                i++;
            }
            ull captures = 0;
            for (ll i = f;; i -= 9) {
                captures |= (1ull << i);
                if (!(i & 7) || !(i >> 3) || (walls & (1ull << i)))
                    break;
            }
            for (ll i = f;; i -= 7) {
                captures |= (1ull << i);
                if ((i & 7) == 7 || !(i >> 3) || (walls & (1ull << i)))
                    break;
            }
            for (ll i = f;; i += 9) {
                captures |= (1ull << i);
                if ((i & 7) == 7 || (i >> 3) == 7 || (walls & (1ull << i)))
                    break;
            }
            for (ll i = f;; i += 7) {
                captures |= (1ull << i);
                if (!(i & 7) || (i >> 3) == 7 || (walls & (1ull << i)))
                    break;
            }
            ull key = ((walls * bishopsMagics[f]) >> bishopsShifts[f]);
            bishopCaptures[f][key] = captures;
        }
    }

    Magics() {
        for (ll i = 0; i < 64; i++)
            for (ll j = 0; j < (1ll << 14); j++) {
                rookCaptures[i][j] = -1;
                bishopCaptures[i][j] = -1;
            }
        edgeU = ((1ull << 8) - 1);
        edgeD = (edgeU << 56);
        edgeL = 0;
        for (ll i = 0; i < 64; i += 8)
            edgeL |= (1ull << i);
        edgeR = (edgeL << 7);
        for (ll i = 0; i < 8; i++)
            for (ll j = 0; j < 8; j++) {
                ll f = i + j * 8;
                rookAttack[f] = 0;
                bishopAttack[f] = 0;
                rookAttackPruned[f] = 0;
                bishopAttackPruned[f] = 0;
                rookPrunedNmb[f] = 0;
                bishopPrunedNmb[f] = 0;
                for (ll i1 = 0; i1 < 8; i1++)
                    for (ll j1 = 0; j1 < 8; j1++) {
                        ll f2 = i1 + j1 * 8;
                        if (i1 == i || j1 == j)
                            rookAttack[f] |= (1ull << f2);
                        if (abs(i - i1) == abs(j - j1))
                            bishopAttack[f] |= (1ull << f2);
                    }
                rookAttackPruned[f] = rookAttack[f];
                if (i != 0)
                    rookAttackPruned[f] &= (ull(-1) - edgeL);
                if (i != 7)
                    rookAttackPruned[f] &= (ull(-1) - edgeR);
                if (j != 0)
                    rookAttackPruned[f] &= (ull(-1) - edgeU);
                if (j != 7)
                    rookAttackPruned[f] &= (ull(-1) - edgeD);
                rookAttackPruned[f] &= (ull(-1) - (1ull << f));
                ll itt = 0;
                for (ll i1 = 0; i1 < 20; i1++)
                    rookPrunedIndex[f][i1] = -1;
                for (ll i1 = 0; i1 < 64; i1++)
                    if (rookAttackPruned[f] & (1ull << i1)) {
                        rookPrunedNmb[f]++;
                        rookPrunedIndex[f][itt] = i1;
                        itt++;
                    }

                bishopAttackPruned[f] = bishopAttack[f];
                bishopAttackPruned[f] &= (ull(-1) - (edgeU | edgeD | edgeL | edgeR));
                bishopAttackPruned[f] &= (ull(-1) - (1ull << f));
                itt = 0;
                for (ll i1 = 0; i1 < 20; i1++)
                    bishopPrunedIndex[f][i1] = -1;
                for (ll i1 = 0; i1 < 64; i1++)
                    if (bishopAttackPruned[f] & (1ull << i1)) {
                        bishopPrunedNmb[f]++;
                        bishopPrunedIndex[f][itt] = i1;
                        itt++;
                    }
            }
        for (ll i = 0; i < 64; i++) {
            initRook(i);
            initBishop(i);
        }
    }
};

Magics magic;
